#ifndef eval
#define eval

#include "rombongan.h"
#include <vector>

struct Rombongan;

typedef struct Score {
    double precision, recall, f1_score;
} Score;

Score calculate_score(
    const std::vector<std::vector<unsigned int> >&,
    const std::vector<Rombongan>&
);

#endif
