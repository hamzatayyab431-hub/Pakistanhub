#ifndef POSTMANAGER_H
#define POSTMANAGER_H

#include "Post.h"
#include <vector>
#include <string>

class PostManager {
private:
    std::vector<Post> posts;
    std::string filePath;
    int nextPostId;

public:
    PostManager();

    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path) const;
    Post createPost(const std::string& author, const std::string& content);

    std::vector<Post> getPostsByUser(const std::string& username) const;
    std::vector<Post> getAllPostsSorted() const;

    bool toggleLike(int postId, bool liked);

    const std::vector<Post>& getPosts() const;
};

#endif // POSTMANAGER_H
