#ifndef SOCIALGRAPH_H
#define SOCIALGRAPH_H

#include <string>
#include <map>
#include <set>

class SocialGraph {
private:
    std::map<std::string, std::set<std::string>> follows; // key: follower, value: set of usernames they follow
    std::string filePath;

public:
    SocialGraph();

    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path) const;

    void follow(const std::string& follower, const std::string& following);
    void unfollow(const std::string& follower, const std::string& following);

    bool isFollowing(const std::string& follower, const std::string& following) const;
    std::set<std::string> getFollowing(const std::string& username) const;
    std::set<std::string> getFollowers(const std::string& username) const;
};

#endif // SOCIALGRAPH_H
