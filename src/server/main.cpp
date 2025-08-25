#include "../../include/server/ChatServer.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }
    int port = std::stoi(argv[1]);
    ChatServer server(port);
    server.start();
    return 0;
}
