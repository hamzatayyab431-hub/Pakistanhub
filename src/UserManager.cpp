#include "UserManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

UserManager::UserManager() {}

bool UserManager::loadFromFile(const std::string& path) {
    filePath = path;
    users.clear();
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        try {
            users.push_back(User::deserialize(line));
        } catch (const std::exception& e) {
            std::cerr << "Warning: Skipping malformed user line: " << line << " (" << e.what() << ")\n";
        }
    }
    file.close();
    return true;
}

bool UserManager::saveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    for (const auto& user : users) {
        file << user.serialize() << "\n";
    }
    file.close();
    return true;
}

bool UserManager::registerUser(const std::string& username, const std::string& password, const std::string& displayName) {
    if (username.empty() || password.empty() || displayName.empty()) {
        return false;
    }

    // Check if username already exists (case-insensitive check)
    for (const auto& user : users) {
        std::string existingUser = user.getUsername();
        std::string registerUser = username;
        std::transform(existingUser.begin(), existingUser.end(), existingUser.begin(), ::tolower);
        std::transform(registerUser.begin(), registerUser.end(), registerUser.begin(), ::tolower);
        if (existingUser == registerUser) {
            return false; // Username taken
        }
    }

    // Validate that inputs do not contain character delimiters '|'
    if (username.find('|') != std::string::npos || 
        password.find('|') != std::string::npos || 
        displayName.find('|') != std::string::npos) {
        return false;
    }

    users.push_back(User(username, password, displayName));

    // Save to file if we have a valid path loaded
    if (!filePath.empty()) {
        saveToFile(filePath);
    }

    return true;
}

User* UserManager::loginUser(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) {
        return nullptr;
    }

    for (auto& user : users) {
        if (user.getUsername() == username && user.getPassword() == password) {
            return &user;
        }
    }
    return nullptr;
}

const std::vector<User>& UserManager::getUsers() const {
    return users;
}

User* UserManager::findUser(const std::string& username) {
    for (auto& user : users) {
        if (user.getUsername() == username) {
            return &user;
        }
    }
    return nullptr;
}
