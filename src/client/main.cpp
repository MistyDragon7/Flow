#include "../../include/client/ChatClient.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <server_ip> <port>\n";
        return 1;
    }
    std::string server_ip = argv[1];
    int port = std::stoi(argv[2]);
    ChatClient client(server_ip, port);
    client.run();
    return 0;
}
