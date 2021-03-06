#ifndef io
#define io

#include "eval.h"
#include "rombongan.h"
#include <vector>
#include <string>

struct Rombongan;
struct Score;

/**
 * User inputs
 */
typedef struct Arguments {
    std::string source;
    std::string path;
    unsigned int entities;
    unsigned int closeness;
    double interval;
    double range;
    double angle;
    bool redundant;
} Arguments;

Arguments read_arguments(int, char *[]);

void write_result(
    const std::vector<Rombongan>&,
    const std::vector<double>&,
    const Arguments&,
    const Score&
);

#endif