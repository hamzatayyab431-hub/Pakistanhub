#include "Feed.h"

Feed::Feed() {}

void Feed::addPost(const Post& post) {
    posts.push_back(post);
}

const std::vector<Post>& Feed::getPosts() const {
    return posts;
}
