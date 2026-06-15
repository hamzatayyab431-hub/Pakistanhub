#ifndef POST_H
#define POST_H

#include <string>
#include <ctime>

class Post {
private:
    int postId;
    std::string authorUsername;
    std::string content;
    std::time_t timestamp;
    int likeCount;

public:
    Post();
    Post(int postId, const std::string& authorUsername, const std::string& content, std::time_t timestamp, int likeCount = 0);

    // Getters and Setters
    int getPostId() const;
    void setPostId(int id);

    std::string getAuthorUsername() const;
    void setAuthorUsername(const std::string& author);

    std::string getContent() const;
    void setContent(const std::string& text);

    std::time_t getTimestamp() const;
    void setTimestamp(std::time_t time);

    int getLikeCount() const;
    void setLikeCount(int count);

    // Serialization & Date Helpers
    std::string getFormattedDate() const;
    std::string serialize() const;
    static Post deserialize(const std::string& data);
};

#endif // POST_H
