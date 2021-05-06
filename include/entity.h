#ifndef entity
#define entity

#include <vector>

typedef struct Entity {
    unsigned int id;
    std::vector<std::vector<double> > trajectories;

    bool operator<(const Entity& o) const {
        return id < o.id;
    }
} Entity;

#endif