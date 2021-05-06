#ifndef rombongan
#define rombongan

#include "entity.h"
#include <vector>
typedef struct Rombongan {
    std::vector<unsigned int> members;
    std::vector<std::pair<unsigned int, unsigned int> > duration;

    bool operator<(const Rombongan& other) const {
        return members.size() > other.members.size();
    }
} Rombongan;

class Parameter;

std::vector<Rombongan> identify_rombongan(
    const std::vector<Entity>& entities,
    const Parameter& params
);

#endif