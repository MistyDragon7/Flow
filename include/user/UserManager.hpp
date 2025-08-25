#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP

#include "User.hpp"
#include <unordered_map>
#include <string>
#include <optional>

// Manages user data, including registration, authentication, friend requests, and chat history.
class UserManager {
private:
    // Stores user data with username as key.
    std::unordered_map<std::string, User> users;
    std::string dataFile;

    void loadFromFile();

public:
    explicit UserManager(const std::string& filename = "users.json");
    void saveToFile() const; 

    bool userExists(const std::string& username) const;
    bool registerUser(const std::string& username, const std::string& password);
    bool authenticateUser(const std::string& username, const std::string& password) const;

    std::optional<std::reference_wrapper<User>> getUser(const std::string& username);
    std::optional<std::reference_wrapper<const User>> getUser(const std::string& username) const;

    bool sendFriendRequest(const std::string& from, const std::string& to);
    bool acceptFriendRequest(const std::string& username, const std::string& from);
    bool rejectFriendRequest(const std::string& rejecting_username, const std::string& sender_username);

    std::optional<std::reference_wrapper<const std::unordered_set<std::string>>> getIncomingFriendRequests(const std::string& username) const;

    void storeMessage(const std::string& sender, const std::string& receiver, const std::string& content);
};

#endif // USER_MANAGER_HPP
