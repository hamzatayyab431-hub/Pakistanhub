#include "SocialGraph.h"
#include <fstream>
#include <sstream>

SocialGraph::SocialGraph() {}

bool SocialGraph::loadFromFile(const std::string& path) {
    filePath = path;
    follows.clear();

    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string follower, following;
        std::getline(ss, follower, '|');
        std::getline(ss, following, '|');

        if (!follower.empty() && !following.empty()) {
            follows[follower].insert(following);
        }
    }
    file.close();
    return true;
}

bool SocialGraph::saveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    for (const auto& pair : follows) {
        const std::string& follower = pair.first;
        for (const auto& following : pair.second) {
            file << follower << "|" << following << "\n";
        }
    }
    file.close();
    return true;
}

void SocialGraph::follow(const std::string& follower, const std::string& following) {
    if (follower.empty() || following.empty() || follower == following) {
        return;
    }
    follows[follower].insert(following);
    if (!filePath.empty()) {
        saveToFile(filePath);
    }
}

void SocialGraph::unfollow(const std::string& follower, const std::string& following) {
    if (follower.empty() || following.empty()) {
        return;
    }
    auto it = follows.find(follower);
    if (it != follows.end()) {
        it->second.erase(following);
        if (!filePath.empty()) {
            saveToFile(filePath);
        }
    }
}

bool SocialGraph::isFollowing(const std::string& follower, const std::string& following) const {
    auto it = follows.find(follower);
    if (it != follows.end()) {
        return it->second.find(following) != it->second.end();
    }
    return false;
}

std::set<std::string> SocialGraph::getFollowing(const std::string& username) const {
    auto it = follows.find(username);
    if (it != follows.end()) {
        return it->second;
    }
    return std::set<std::string>();
}

std::set<std::string> SocialGraph::getFollowers(const std::string& username) const {
    std::set<std::string> followersList;
    for (const auto& pair : follows) {
        if (pair.second.find(username) != pair.second.end()) {
            followersList.insert(pair.first);
        }
    }
    return followersList;
}
