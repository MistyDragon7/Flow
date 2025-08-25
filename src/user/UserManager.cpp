#include "../../include/user/UserManager.hpp"
#include "../../include/user/User.hpp"
#include "../../include/nlohmann/json.hpp"
#include <fstream>
#include <stdexcept>

UserManager::UserManager(const std::string& filename) : dataFile(filename) {
    loadFromFile();
}

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
