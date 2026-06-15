#ifndef SOCIALGRAPH_H
#define SOCIALGRAPH_H

#include <string>
#include <map>
#include <set>

class SocialGraph {
private:
    std::map<std::string, std::set<std::string>> follows;

public:
    SocialGraph();
};

#endif // SOCIALGRAPH_H
