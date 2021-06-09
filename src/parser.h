#ifndef parser
#define parser

#include "io.h"
#include "entity.h"
#include <vector>
#include <string>

struct Arguments;

typedef struct MovementData {
    std::vector<Entity> entities;
    std::vector<double> frames;
} MovementData;

typedef struct Parameter {
    unsigned int m, k, c;
    double r, cs;
    bool redundant;
} Parameter;

MovementData parse_data(const std::string&, const std::string&);
Parameter parse_arguments(const Arguments&);
std::vector<std::vector<unsigned int> > parse_expected_result(
    const std::string&,
    const std::string&
);

#endif
