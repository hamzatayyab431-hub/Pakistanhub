#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "User.h"
#include <vector>
#include <string>

class UserManager {
private:
    std::vector<User> users;
    std::string filePath;

public:
    UserManager();

    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path) const;
    bool registerUser(const std::string& username, const std::string& password, const std::string& displayName);
    User* loginUser(const std::string& username, const std::string& password);

    const std::vector<User>& getUsers() const;
    User* findUser(const std::string& username);
};

#endif // USERMANAGER_H
