#ifndef COMMENT_H
#define COMMENT_H

#include <string>
#include <ctime>

class Comment {
private:
    int commentId;
    int postId;
    std::string authorUsername;
    std::string content;
    std::time_t timestamp;

public:
    Comment() : commentId(0), postId(0), timestamp(0) {}
    Comment(int cId, int pId, const std::string& author, const std::string& text, std::time_t ts);

    int getCommentId() const;
    int getPostId() const;
    std::string getAuthorUsername() const;
    std::string getContent() const;
    std::time_t getTimestamp() const;
    std::string getFormattedDate() const;

    std::string toString() const;
    static Comment fromString(const std::string& line);
};

#endif // COMMENT_H
