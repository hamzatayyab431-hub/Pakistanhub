#include "User.h"
#include <sstream>
#include <vector>

User::User() : followerCount(0), followingCount(0) {}

User::User(const std::string& username, const std::string& password, const std::string& displayName, const std::string& bio, const std::string& city, int followerCount, int followingCount)
    : username(username), password(password), displayName(displayName), bio(bio), city(city), followerCount(followerCount), followingCount(followingCount) {}

std::string User::getUsername() const { return username; }
void User::setUsername(const std::string& username) { this->username = username; }

std::string User::getPassword() const { return password; }
void User::setPassword(const std::string& password) { this->password = password; }

std::string User::getDisplayName() const { return displayName; }
void User::setDisplayName(const std::string& displayName) { this->displayName = displayName; }

std::string User::getBio() const { return bio; }
void User::setBio(const std::string& bio) { this->bio = bio; }

std::string User::getCity() const { return city; }
void User::setCity(const std::string& city) { this->city = city; }

int User::getFollowerCount() const { return followerCount; }
void User::setFollowerCount(int count) { this->followerCount = count; }

int User::getFollowingCount() const { return followingCount; }
void User::setFollowingCount(int count) { this->followingCount = count; }

std::string User::serialize() const {
    return username + "|" + password + "|" + displayName + "|" + bio + "|" + city + "|" + std::to_string(followerCount) + "|" + std::to_string(followingCount);
}

User User::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::vector<std::string> parts;
    std::string token;

    while (std::getline(ss, token, '|')) {
        parts.push_back(token);
    }

    std::string username = parts.size() > 0 ? parts[0] : "";
    std::string password = parts.size() > 1 ? parts[1] : "";
    std::string displayName = parts.size() > 2 ? parts[2] : "";
    std::string bio = parts.size() > 3 ? parts[3] : "";

    std::string city = "";
    int followerCount = 0;
    int followingCount = 0;

    if (parts.size() == 6) {
        followerCount = parts[4].empty() ? 0 : std::stoi(parts[4]);
        followingCount = parts[5].empty() ? 0 : std::stoi(parts[5]);
    } else if (parts.size() >= 7) {
        city = parts[4];
        followerCount = parts[5].empty() ? 0 : std::stoi(parts[5]);
        followingCount = parts[6].empty() ? 0 : std::stoi(parts[6]);
    }

    return User(username, password, displayName, bio, city, followerCount, followingCount);
}
