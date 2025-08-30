#include "../../include/user/UserManager.hpp"
#include "../../include/user/User.hpp"
#include "../../include/nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>


using json = nlohmann::json;
// Initialises UserManager and ensures file is valid.
UserManager::UserManager(const std::string& filename) : dataFile(filename) {
    if (!std::filesystem::exists(dataFile) || std::filesystem::file_size(dataFile)==0) {
        std::ofstream outfile(dataFile);
        if (outfile.is_open()) {
            outfile<<"{}";
        }
    }
    else {
        std::ifstream checkFile(dataFile);
        std::string content((std::istreambuf_iterator<char>(checkFile)),
                            std::istreambuf_iterator<char>());
        checkFile.close();
        try {
            json::parse(content); // Try to parse to validate
        } catch (const json::parse_error& e) {
            std::cerr << "Corrupted JSON file detected: " << dataFile << ". Overwriting with empty object. Error: " << e.what() << std::endl;
            std::ofstream outFile(dataFile);
            if (outFile.is_open()) {
                outFile << "{}";
            }
        }
    }
    loadFromFile();
}

//Loads user data from json into memory
void UserManager::loadFromFile() {
    // Basic implementation: does nothing for now.
    // Will be expanded in Medium-Term Vision.
}

void UserManager::saveToFile() const {
    // Basic implementation: does nothing for now.
    // Will be expanded in Medium-Term Vision.
}

bool UserManager::userExists(const std::string& username) const {
    // For short-term, assume user does not exist persistently.
    // Uniqueness for *currently connected* users is handled in ChatServer.
    return false; 
}

bool UserManager::registerUser(const std::string& username, const std::string& password) {
    // Not part of short-term vision (persistent registration).
    return false;
}

bool UserManager::authenticateUser(const std::string& username, const std::string& password) const {
    // Not part of short-term vision.
    return false;
}

std::optional<std::reference_wrapper<User>> UserManager::getUser(const std::string& username) {
    return std::nullopt;
}

std::optional<std::reference_wrapper<const User>> UserManager::getUser(const std::string& username) const {
    return std::nullopt;
}

bool UserManager::sendFriendRequest(const std::string& from, const std::string& to) {
    return false;
}

bool UserManager::acceptFriendRequest(const std::string& username, const std::string& from) {
    return false;
}

bool UserManager::rejectFriendRequest(const std::string& rejecting_username, const std::string& sender_username) {
    return false;
}

std::optional<std::reference_wrapper<const std::unordered_set<std::string>>> UserManager::getIncomingFriendRequests(const std::string& username) const {
    return std::nullopt;
}

void UserManager::storeMessage(const std::string& sender, const std::string& receiver, const std::string& content) {
    // Not part of short-term vision.
}
