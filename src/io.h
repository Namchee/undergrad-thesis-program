#ifndef io
#define io

#include "eval.h"
#include "rombongan.h"
#include <vector>
#include <string>

/**
 * Parameters to be used when identifying rombongan from trajectory data.
 */
typedef struct Arguments {
    std::string source;
    std::string path;
    unsigned int entities;
    double interval;
    double range;
    double angle;
    double fps;
} Arguments;

Arguments read_arguments(int, char *[]);

void write_result(
    const std::vector<Rombongan>&,
    const std::vector<double>&,
    const Arguments&,
    const Score&
);

#endif