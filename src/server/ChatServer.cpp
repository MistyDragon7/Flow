#include "../../include/Common.hpp"
#include "../../include/server/ChatServer.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <thread>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#define CLOSE_SOCKET closesocket
#else
#define CLOSE_SOCKET close
#endif

#include "../../include/Color.hpp"
#include "../../include/user/UserManager.hpp"

ChatServer::ChatServer(int port)
    : port_(port), server_fd_(-1) {}
ChatServer::~ChatServer() {
  if (server_fd_ != -1) {
    CLOSE_SOCKET(server_fd_);
  }
#ifdef _WIN32
  WSACleanup();
#endif
}

void ChatServer::start() {
#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "WSAStartup failed\n";
    exit(EXIT_FAILURE);
  }
#endif
  struct sockaddr_in address;
  int opt = 1;
  socklen_t addrlen = sizeof(address);

  // Create socket fd
  server_fd_ = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
  if (server_fd_ <= 0) {
    perror("SOCKET FAILURE");
    exit(EXIT_FAILURE);
  }
#ifndef _WIN32
  setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
             sizeof(opt));
#else
  setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt,
             sizeof(opt));
#endif
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);
    if (bind(server_fd_, (struct sockaddr *) &address, sizeof(address))<0) {
      perror("BIND FAILURE");
      exit(EXIT_FAILURE);
    }

    if (listen(server_fd_, 10)<0) {
      perror("LISTEN FAILURE");
      exit(EXIT_FAILURE);
    }
    running_ = true;
    std::cout<<"Server Listening on Port: "<<port_<<std::endl;
    accept_clients();
}

void ChatServer::accept_clients() {
  while (running_) {
    int client_socket = static_cast<int>(accept(server_fd_, nullptr, nullptr));
    if (client_socket<0) {
      perror("Accept Failed");
      continue;
    }
    std::thread(&ChatServer::handle_client, this, client_socket).detach();

  }
}

void ChatServer::handle_client(int client_socket) {
  std::cout << COLOR_CYAN << "[Server] Handling new client on socket: " << client_socket << COLOR_RESET << std::endl;
  char buffer[BUFFER_SIZE];
  std::string username = "";
  std::string leftover_buffer;
  bool needs_disconnect_after_lock = false; // Flag to indicate disconnection needed

  // Receive username
  auto optional_username = read_delimited_message(client_socket, leftover_buffer);
  if (!optional_username) {
    std::cerr << COLOR_RED << "[Server] Failed to read username from socket: " << client_socket << COLOR_RESET << std::endl;
    disconnect_client(client_socket, username);
    return;
  }
  username = optional_username.value();
  std::cout << COLOR_CYAN << "[Server] Received username '" << username << "' from socket: " << client_socket << COLOR_RESET << std::endl;

  // Check for username uniqueness (non-persistent)
  { // Lock for clients_mutex_
    std::lock_guard<std::mutex> lock(clients_mutex_);
    std::cout << COLOR_CYAN << "[Server] Acquiring mutex for socket: " << client_socket << COLOR_RESET << std::endl;
    bool username_taken = false;
    for (const auto& pair : clients_) {
      if (pair.second == username) {
        username_taken = true;
        break;
      }
    }
    if (username_taken) {
      std::string msg = std::string(COLOR_RED) + "Username \"" + username + "\" is taken. Please choose another." + COLOR_RESET + MESSAGE_DELIMITER;
      send(client_socket, msg.c_str(), msg.length(), 0);
      needs_disconnect_after_lock = true; // Set flag
    } else {
      clients_[client_socket] = username;
      std::cout << COLOR_CYAN << "[Server] Client '" << username << "' added to active list. Socket: " << client_socket << COLOR_RESET << std::endl;
      std::string ack_msg = USERNAME_ACCEPTED_MAGIC;
      send(client_socket, ack_msg.c_str(), ack_msg.length(), 0);
      std::cout << COLOR_GREEN << username << " has joined. Socket: " << client_socket << COLOR_RESET << std::endl;
    }
  } // Lock ends here - clients_mutex_ is released here.

  if (needs_disconnect_after_lock) { // Perform disconnection after mutex is released
    disconnect_client(client_socket, username);
    return;
  }

  while (running_) {
    auto optional_message = read_delimited_message(client_socket, leftover_buffer);
    if (!optional_message) {
      break; 
    }
    std::string message = optional_message.value();

    if (message == "_quit") {
      break; 
    }
    process_chat_command(client_socket, username, message);
  }
  disconnect_client(client_socket, username);
}

std::optional<std::string> ChatServer::read_delimited_message(
    int client_socket, std::string& leftover_buffer) {
  while (true) {
    size_t delimiter_pos = leftover_buffer.find(MESSAGE_DELIMITER);
    if (delimiter_pos != std::string::npos) {
      std::string message = leftover_buffer.substr(0, delimiter_pos);
      leftover_buffer.erase(0, delimiter_pos + strlen(MESSAGE_DELIMITER));
      return message;
    }

    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);

    if (bytes_received <= 0) {
      if (bytes_received == 0) {
        std::cout << "Client disconnected: " << client_socket << std::endl;
      } else {
        perror("recv failed");
      }
      return std::nullopt; // Client disconnected or error
    }
    leftover_buffer.append(buffer, bytes_received);
  }
}

void ChatServer::broadcast(const std::string &message, int sender_socket) {
  std::lock_guard<std::mutex> lock(clients_mutex_);
  for (auto const& [client_socket, username] : clients_) {
    if (client_socket != sender_socket) {
      std::string formatted_message = message + MESSAGE_DELIMITER;
      send(client_socket, formatted_message.c_str(), formatted_message.length(), 0);
    }
  }
}

void ChatServer::remove_client(int client_socket) {
  std::lock_guard<std::mutex> lock(clients_mutex_);
  clients_.erase(client_socket);
}

void ChatServer::disconnect_client(int client_socket, const std::string& username) {
  if (!username.empty()) {
    std::cout << COLOR_YELLOW << username << " has left the chat. Socket: " << client_socket << COLOR_RESET << std::endl;
    broadcast(std::string(COLOR_YELLOW) + username + " has left the chat." + COLOR_RESET, client_socket);
  } else {
    std::cout << COLOR_YELLOW << "Client disconnected. Socket: " << client_socket << COLOR_RESET << std::endl;
  }
  remove_client(client_socket);
  CLOSE_SOCKET(client_socket);
}

void ChatServer::process_chat_command(int client_socket, const std::string& sender_username, const std::string& message) {
    std::string full_message = "[" + sender_username + "]: " + message;
    std::cout << full_message << std::endl;
    broadcast(full_message, client_socket);
}
