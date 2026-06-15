#include "User.h"
#include <sstream>

User::User() : followerCount(0), followingCount(0) {}

User::User(const std::string& username, const std::string& password, const std::string& displayName, const std::string& bio, int followerCount, int followingCount)
    : username(username), password(password), displayName(displayName), bio(bio), followerCount(followerCount), followingCount(followingCount) {}

std::string User::getUsername() const { return username; }
void User::setUsername(const std::string& username) { this->username = username; }

std::string User::getPassword() const { return password; }
void User::setPassword(const std::string& password) { this->password = password; }

std::string User::getDisplayName() const { return displayName; }
void User::setDisplayName(const std::string& displayName) { this->displayName = displayName; }

std::string User::getBio() const { return bio; }
void User::setBio(const std::string& bio) { this->bio = bio; }

int User::getFollowerCount() const { return followerCount; }
void User::setFollowerCount(int count) { this->followerCount = count; }

int User::getFollowingCount() const { return followingCount; }
void User::setFollowingCount(int count) { this->followingCount = count; }

std::string User::serialize() const {
    return username + "|" + password + "|" + displayName + "|" + bio + "|" + std::to_string(followerCount) + "|" + std::to_string(followingCount);
}

User User::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string username, password, displayName, bio, followerStr, followingStr;

    std::getline(ss, username, '|');
    std::getline(ss, password, '|');
    std::getline(ss, displayName, '|');
    std::getline(ss, bio, '|');
    std::getline(ss, followerStr, '|');
    std::getline(ss, followingStr, '|');

    int followerCount = followerStr.empty() ? 0 : std::stoi(followerStr);
    int followingCount = followingStr.empty() ? 0 : std::stoi(followingStr);

    return User(username, password, displayName, bio, followerCount, followingCount);
}
