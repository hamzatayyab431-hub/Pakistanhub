#include "Post.h"
#include <sstream>

Post::Post() : postId(0), timestamp(0), likeCount(0) {}

Post::Post(int postId, const std::string& authorUsername, const std::string& content, std::time_t timestamp, int likeCount)
    : postId(postId), authorUsername(authorUsername), content(content), timestamp(timestamp), likeCount(likeCount) {}

int Post::getPostId() const { return postId; }
void Post::setPostId(int id) { this->postId = id; }

std::string Post::getAuthorUsername() const { return authorUsername; }
void Post::setAuthorUsername(const std::string& author) { this->authorUsername = author; }

std::string Post::getContent() const { return content; }
void Post::setContent(const std::string& text) { this->content = text; }

std::time_t Post::getTimestamp() const { return timestamp; }
void Post::setTimestamp(std::time_t time) { this->timestamp = time; }

int Post::getLikeCount() const { return likeCount; }
void Post::setLikeCount(int count) { this->likeCount = count; }

std::string Post::getFormattedDate() const {
    char buffer[80];
    // Convert time_t to tm structure safely
    std::tm* timeinfo = std::localtime(&timestamp);
    if (timeinfo) {
        std::strftime(buffer, sizeof(buffer), "%b %d, %Y", timeinfo);
        return std::string(buffer);
    }
    return "01 Jan 1970";
}

std::string Post::serialize() const {
    return std::to_string(postId) + "|" + authorUsername + "|" + content + "|" + std::to_string(timestamp) + "|" + std::to_string(likeCount);
}

Post Post::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string idStr, author, content, timeStr, likeStr;

    std::getline(ss, idStr, '|');
    std::getline(ss, author, '|');
    std::getline(ss, content, '|');
    std::getline(ss, timeStr, '|');
    std::getline(ss, likeStr, '|');

    int id = idStr.empty() ? 0 : std::stoi(idStr);
    std::time_t timeVal = timeStr.empty() ? 0 : static_cast<std::time_t>(std::stoll(timeStr));
    int likes = likeStr.empty() ? 0 : std::stoi(likeStr);

    return Post(id, author, content, timeVal, likes);
}
