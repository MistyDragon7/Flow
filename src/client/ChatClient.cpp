#include "../../include/client/ChatClient.hpp"
#include "../../include/Common.hpp"
#include "../../include/Color.hpp"
#include <iostream>
#include <cstring>
#include <csignal>
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

ChatClient *global_client = nullptr;

void signal_handler(int) {
    if (global_client) global_client->cleanup();
    std::cout << COLOR_YELLOW << "\nDisconnected from server." << COLOR_RESET << std::endl;
    exit(0);
}

ChatClient::ChatClient(const std::string &server_ip, int port)
    : server_ip_(server_ip), port_(port), sock_(-1), connected_(false) {}

ChatClient::~ChatClient() {
    cleanup();
}

void ChatClient::run() {
    global_client = this;
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
#ifdef SIGQUIT
    std::signal(SIGQUIT, signal_handler);
#endif
    connect_to_server();
    send_messages();
}

void ChatClient::connect_to_server() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << COLOR_RED << "WSAStartup failed" << COLOR_RESET << std::endl;
        exit(EXIT_FAILURE);
    }
#endif
    sock_ = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
    if (sock_ < 0) {
        std::cerr << COLOR_RED << "Socket creation failed" << COLOR_RESET << std::endl;
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_);
    if (inet_pton(AF_INET, server_ip_.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << COLOR_RED << "Invalid address/ Address not supported" << COLOR_RESET << std::endl;
        exit(EXIT_FAILURE);
    }
    if (connect(sock_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << COLOR_RED << "Connection Failed" << COLOR_RESET << std::endl;
        exit(EXIT_FAILURE);
    }
    connected_ = true;

    // Username negotiation
    // Using a do-while loop for clarity in re-connection
    do {
        if (!connected_) { // Reconnect if needed
            cleanup(); // Ensure old socket is closed
            sock_ = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
            if (sock_ < 0) {
                std::cerr << COLOR_RED << "Socket recreation failed" << COLOR_RESET << std::endl;
                exit(EXIT_FAILURE);
            }
            if (connect(sock_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                std::cerr << COLOR_RED << "Reconnection Failed" << COLOR_RESET << std::endl;
                exit(EXIT_FAILURE);
            }
            connected_ = true;
        }

        std::cout << COLOR_CYAN << "Enter your desired username: " << COLOR_RESET;
        std::getline(std::cin, username_);
        if (username_.empty()) continue;

        std::string to_send = username_ + MESSAGE_DELIMITER;
        send(sock_, to_send.c_str(), to_send.length(), 0);

        char buffer[BUFFER_SIZE] = {0};
        int bytes = recv(sock_, buffer, BUFFER_SIZE, 0);

        if (bytes > 0) {
            std::string response(buffer, bytes);
            if (response.find("is taken") != std::string::npos) {
                std::cout << COLOR_RED << response << COLOR_RESET;
                connected_ = false; // Mark as disconnected to trigger reconnection
                continue;
            } else if (response == USERNAME_ACCEPTED_MAGIC) {
                std::cout << COLOR_GREEN << "Connected as '" << username_ << "'!" << COLOR_RESET << std::endl;
                break; // Exit username negotiation loop
            } else {
                std::cerr << COLOR_RED << "Unexpected server response: " << response << COLOR_RESET << std::endl;
                cleanup();
                exit(EXIT_FAILURE);
            }
        } else {
            std::cerr << COLOR_RED << "Server closed connection during username negotiation." << COLOR_RESET << std::endl;
            cleanup();
            exit(EXIT_FAILURE);
        }
    } while(true);

    receiver_thread_ = std::thread(&ChatClient::receive_messages, this);
}

void ChatClient::send_messages() {
    std::string input;
    while (connected_) {
        std::getline(std::cin, input);
        if (!connected_) break;
        if (input == "_quit" || std::cin.eof()) {
            std::string quit_msg = "_quit" + std::string(MESSAGE_DELIMITER);
            send(sock_, quit_msg.c_str(), quit_msg.length(), 0);
            std::cout << COLOR_YELLOW << "Disconnected from server." << COLOR_RESET << std::endl; // Added for explicit feedback
            break;
        }
        std::string to_send = input + MESSAGE_DELIMITER;
        send(sock_, to_send.c_str(), to_send.length(), 0);
    }
    cleanup();
}

void ChatClient::receive_messages() {
    char buffer[BUFFER_SIZE];
    std::string leftover;
    while (connected_) {
        int bytes = recv(sock_, buffer, BUFFER_SIZE, 0);
        if (bytes <= 0) {
            std::cout << COLOR_YELLOW << "\nDisconnected from server." << COLOR_RESET << std::endl;
            connected_ = false;
            break;
        }
        leftover.append(buffer, bytes);
        size_t pos;
        while ((pos = leftover.find(MESSAGE_DELIMITER)) != std::string::npos) {
            std::string msg = leftover.substr(0, pos);
            leftover.erase(0, pos + strlen(MESSAGE_DELIMITER));
            std::cout << msg << std::endl;
        }
    }
}

void ChatClient::cleanup() {
    if (connected_) {
        connected_ = false;
#ifdef _WIN32
        closesocket(sock_);
        WSACleanup();
#else
        close(sock_);
#endif
        if (receiver_thread_.joinable()) receiver_thread_.join();
    }
}
