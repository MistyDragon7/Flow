#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP

#include <string>
#include <thread>
#include <atomic>

class ChatClient {
public:
    ChatClient(const std::string &server_ip, int port);
    ~ChatClient();
    void run();
    void cleanup();

private:
    void connect_to_server();
    void receive_messages();
    void send_messages();

    std::string server_ip_;
    int port_;
    int sock_;
    std::string username_;
    std::thread receiver_thread_;
    std::atomic<bool> connected_;
};

#endif // CHAT_CLIENT_HPP
