#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>

// Magic string for client handshake to ensure proper protocol communication.
const std::string CLIENT_HANDSHAKE_MAGIC = "CHAT_HS_V1\n";
const std::string USERNAME_ACCEPTED_MAGIC = "_USERNAME_ACCEPTED_\n";

#define BUFFER_SIZE 4096
#define MESSAGE_DELIMITER "\n"

#endif // COMMON_HPP