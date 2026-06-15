#include "PostManager.h"
#include <fstream>
#include <algorithm>
#include <iostream>

PostManager::PostManager() : nextPostId(1) {}

bool PostManager::loadFromFile(const std::string& path) {
    filePath = path;
    posts.clear();
    nextPostId = 1;

    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        Post post = Post::deserialize(line);
        posts.push_back(post);
        if (post.getPostId() >= nextPostId) {
            nextPostId = post.getPostId() + 1;
        }
    }
    file.close();
    return true;
}

bool PostManager::saveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    for (const auto& post : posts) {
        file << post.serialize() << "\n";
    }
    file.close();
    return true;
}

Post PostManager::createPost(const std::string& author, const std::string& content) {
    std::time_t now = std::time(nullptr);
    Post newPost(nextPostId++, author, content, now, 0);
    posts.push_back(newPost);

    if (!filePath.empty()) {
        saveToFile(filePath);
    }
    return newPost;
}

std::vector<Post> PostManager::getPostsByUser(const std::string& username) const {
    std::vector<Post> userPosts;
    for (const auto& post : posts) {
        if (post.getAuthorUsername() == username) {
            userPosts.push_back(post);
        }
    }
    return userPosts;
}

std::vector<Post> PostManager::getAllPostsSorted() const {
    std::vector<Post> sortedPosts = posts;
    std::sort(sortedPosts.begin(), sortedPosts.end(), [](const Post& a, const Post& b) {
        if (a.getTimestamp() == b.getTimestamp()) {
            return a.getPostId() > b.getPostId(); // Fallback to higher ID first
        }
        return a.getTimestamp() > b.getTimestamp(); // Newest first
    });
    return sortedPosts;
}

bool PostManager::toggleLike(int postId, bool liked) {
    for (auto& post : posts) {
        if (post.getPostId() == postId) {
            if (liked) {
                post.setLikeCount(post.getLikeCount() + 1);
            } else {
                post.setLikeCount(std::max(0, post.getLikeCount() - 1));
            }

            if (!filePath.empty()) {
                saveToFile(filePath);
            }
            return true;
        }
    }
    return false;
}

const std::vector<Post>& PostManager::getPosts() const {
    return posts;
}
