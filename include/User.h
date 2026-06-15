#ifndef USER_H
#define USER_H

#include <string>

class User {
private:
    std::string username;
    std::string password;
    std::string displayName;
    std::string bio;
    int followerCount;
    int followingCount;

public:
    User();
    User(const std::string& username, const std::string& password, const std::string& displayName, const std::string& bio = "", int followerCount = 0, int followingCount = 0);

    // Getters and Setters
    std::string getUsername() const;
    void setUsername(const std::string& username);

    std::string getPassword() const;
    void setPassword(const std::string& password);

    std::string getDisplayName() const;
    void setDisplayName(const std::string& displayName);

    std::string getBio() const;
    void setBio(const std::string& bio);

    int getFollowerCount() const;
    void setFollowerCount(int count);

    int getFollowingCount() const;
    void setFollowingCount(int count);

    // Serialization & Deserialization
    std::string serialize() const;
    static User deserialize(const std::string& data);
};

#endif // USER_H
