#ifndef COMMENTMANAGER_H
#define COMMENTMANAGER_H

#include "Comment.h"
#include <vector>
#include <string>

class CommentManager {
private:
    std::vector<Comment> comments;
    int nextCommentId;

public:
    CommentManager();

    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path);

    Comment& addComment(int postId, const std::string& author, const std::string& content);
    std::vector<Comment> getCommentsForPost(int postId) const;
    int getCommentCountForPost(int postId) const;
};

#endif // COMMENTMANAGER_H
