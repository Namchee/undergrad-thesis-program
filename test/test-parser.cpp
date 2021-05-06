#include "parser.h"
#include "entity.h"
#include <string>
#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <stdexcept>
#include <limits>

std::vector<std::vector<double> > b_trajectory{
    { std::nan(""), std::nan("") },
    { std::nan(""), std::nan("") },
    { 1.5, 1.5 },
    { std::nan(""), std::nan("") }
};

std::vector<std::vector<double> > a_trajectory{
    { 1.0, 1.0 },
    { 2.0, 2.0 },
    { std::nan(""), std::nan("") },
    { 3.0, 5.55 }
};

std::vector<std::vector<unsigned int> > expected_data{
    { 1, 2, 3 },
    { 4, 5 }
};

void data_should_parse_correctly() {
    std::string source("test_data");
    std::string path("test/fixtures");

    MovementData data = parse_data(source, path);
    std::vector<Entity> entities = data.entities;

    for (unsigned int i = 0; i < entities.size(); i++) {
        if (entities[i].id == 2) {
            for (unsigned int j = 0; j < entities[i].trajectories.size(); j++) {
                if (std::isnan(entities[i].trajectories[j][0])) {
                    assert(std::isnan(b_trajectory[j][0]));
                } else {
                    assert(entities[i].trajectories[j][0] == b_trajectory[j][0]);
                }
                
                if (std::isnan(entities[i].trajectories[j][1])) {
                    assert(std::isnan(b_trajectory[j][1]));
                } else {
                    assert(entities[i].trajectories[j][1] == b_trajectory[j][1]);
                }
            }
        } else {
            for (unsigned int j = 0; j < entities[i].trajectories.size(); j++) {
                if (std::isnan(entities[i].trajectories[j][0])) {
                    assert(std::isnan(a_trajectory[j][0]));
                } else {
                    assert(entities[i].trajectories[j][0] == a_trajectory[j][0]);
                }
                
                if (std::isnan(entities[i].trajectories[j][1])) {
                    assert(std::isnan(a_trajectory[j][1]));
                } else {
                    assert(entities[i].trajectories[j][1] == a_trajectory[j][1]);
                }
            }
        }
    }
}

void data_should_throw_an_error_if_file_not_found() {
    std::string source("not_exist");
    std::string path("test/fixtures");

    try {
        parse_data(source, path);
        throw std::logic_error("Should throw an error as file does not exist.");
    } catch(const std::invalid_argument &err) {
        assert(err.what() == std::string("Source data doesn't exist!"));
    }
}

void expected_should_parse_correctly() {
    std::string source("test_data");
    std::string path("test/fixtures");

    auto expected_result = parse_expected_result(source, path);

    assert(expected_data == expected_result);
}

void expected_should_throw_an_error_if_file_not_found() {
    std::string source("not_exist");
    std::string path("test/fixtures");

    try {
        parse_expected_result(source, path);
        throw std::logic_error("Should throw an error as file does not exist.");
    } catch(const std::invalid_argument &err) {
        assert(err.what() == std::string("Expected result doesn't exist!"));
    }
}

int main() {
    data_should_parse_correctly();
    // data_should_throw_an_error_if_file_not_found();
    // expected_should_parse_correctly();
    // expected_should_throw_an_error_if_file_not_found();

    return 0;
}