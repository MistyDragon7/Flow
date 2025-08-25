#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

struct Message {
    std::string sender;
    std::string content;
};

class User {
    public: 
    friend class UserManager;
    User(std::string &username, const std::string &password);
    const std::string &getUsername() const;
    bool checkPassword(const std::string &password) const;
    bool hasFriend(const std::string& other) const;
    bool hasPendingRequestFrom(const std::string& other) const;
    bool hasSentRequestTo(const std::string& other) const;

    void sendFriendRequestTo(const std::string& other);
    void receiveFriendRequestFrom(const std::string& other);
    bool acceptFriendRequestFrom(const std::string& other);
    void rejectFriendRequestFrom(const std::string& sender_username);
    void completeOutgoingFriendRequest(const std::string& other);
    void cancelOutgoingFriendRequest(const std::string& other);

    const std::unordered_set<std::string>& getFriends() const;
    const std::unordered_set<std::string>& getIncomingFriendRequests() const;
    void storeMessage(const std::string& chatPartner, const std::string& sender, const std::string& content);
    const std::vector<Message>& getChatHistoryWith(const std::string& friendUsername) const;

private:
    std::string username;     
    std::string passwordHash; // Hashed password for authentication.

    std::unordered_set<std::string> friends;         // Set of friends' usernames.
    std::unordered_set<std::string> incomingRequests; // Set of incoming friend requests.
    std::unordered_set<std::string> outgoingRequests; // Set of outgoing friend requests.

    std::unordered_map<std::string, std::vector<Message>> chatHistory;
};
#endif