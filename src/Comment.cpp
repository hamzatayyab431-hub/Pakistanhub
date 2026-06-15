#include "Comment.h"
#include <sstream>
#include <iomanip>

Comment::Comment(int cId, int pId, const std::string& author, const std::string& text, std::time_t ts)
    : commentId(cId), postId(pId), authorUsername(author), content(text), timestamp(ts) {}

int Comment::getCommentId() const { return commentId; }
int Comment::getPostId() const { return postId; }
std::string Comment::getAuthorUsername() const { return authorUsername; }
std::string Comment::getContent() const { return content; }
std::time_t Comment::getTimestamp() const { return timestamp; }

std::string Comment::getFormattedDate() const {
    std::tm* timeinfo = std::localtime(&timestamp);
    if (timeinfo == nullptr) return "";
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%b %d, %Y %I:%M %p", timeinfo);
    return std::string(buffer);
}

std::string Comment::toString() const {
    std::stringstream ss;
    ss << commentId << "|" << postId << "|" << authorUsername << "|" << content << "|" << timestamp;
    return ss.str();
}

Comment Comment::fromString(const std::string& line) {
    std::stringstream ss(line);
    std::string item;
    
    int cId = 0;
    int pId = 0;
    std::string author = "";
    std::string text = "";
    std::time_t ts = 0;

    if (std::getline(ss, item, '|')) cId = std::stoi(item);
    if (std::getline(ss, item, '|')) pId = std::stoi(item);
    if (std::getline(ss, author, '|')) {}
    if (std::getline(ss, text, '|')) {}
    if (std::getline(ss, item, '|')) ts = std::stoll(item);

    return Comment(cId, pId, author, text, ts);
}
