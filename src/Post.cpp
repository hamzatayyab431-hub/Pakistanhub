#include "Post.h"

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
