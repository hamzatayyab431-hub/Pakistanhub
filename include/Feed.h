#ifndef FEED_H
#define FEED_H

#include "Post.h"
#include <vector>

class Feed {
private:
    std::vector<Post> posts;

public:
    Feed();
    void addPost(const Post& post);
    const std::vector<Post>& getPosts() const;
};

#endif // FEED_H
