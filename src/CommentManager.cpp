#include "CommentManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>

CommentManager::CommentManager() : nextCommentId(1) {}

bool CommentManager::loadFromFile(const std::string& path) {
    comments.clear();
    nextCommentId = 1;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        try {
            Comment c = Comment::fromString(line);
            comments.push_back(c);
            if (c.getCommentId() >= nextCommentId) {
                nextCommentId = c.getCommentId() + 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: Skipping malformed comment line: " << line << " (" << e.what() << ")\n";
        }
    }
    return true;
}

bool CommentManager::saveToFile(const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& comment : comments) {
        file << comment.toString() << "\n";
    }
    return true;
}

Comment& CommentManager::addComment(int postId, const std::string& author, const std::string& content) {
    std::time_t now = std::time(nullptr);
    Comment comment(nextCommentId++, postId, author, content, now);
    comments.push_back(comment);
    return comments.back();
}

std::vector<Comment> CommentManager::getCommentsForPost(int postId) const {
    std::vector<Comment> result;
    for (const auto& comment : comments) {
        if (comment.getPostId() == postId) {
            result.push_back(comment);
        }
    }
    
    // Sort comments chronologically (oldest first)
    std::sort(result.begin(), result.end(), [](const Comment& a, const Comment& b) {
        return a.getTimestamp() < b.getTimestamp();
    });

    return result;
}

int CommentManager::getCommentCountForPost(int postId) const {
    int count = 0;
    for (const auto& comment : comments) {
        if (comment.getPostId() == postId) {
            count++;
        }
    }
    return count;
}
