#include "rombongan.h"
#include "spatial.h"
#include "entity.h"
#include "utils.h"
#include "io.h"
#include "parser.h"
#include <cmath>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <map>
#include <iostream>

typedef std::unordered_map<unsigned int, std::vector<std::vector<double> > > trajectory_map;
typedef std::unordered_map<unsigned int, std::vector<double> > direction_map;
typedef std::map<std::vector<unsigned int>, std::vector<std::pair<unsigned int, unsigned int> > > rombongan_lifespan;

bool on_interval(
    const Entity& target_entity,
    const std::pair<unsigned int, unsigned int>& interval
) {
    std::vector<std::vector<double> > trajectory = {
        target_entity.trajectories.begin() + interval.first,
        target_entity.trajectories.begin() + interval.second
    };

    for (size_t t_itr = 0; t_itr < trajectory.size(); t_itr++) {
        if (std::isnan(trajectory[t_itr][0])) {
            return false;
        }
    }

    return true;
}

/**
 * Get sub trajectories from all entities for a time interval
 * 
 * @param entities input entities
 * @param interval time interval
 */
trajectory_map get_sub_trajectories(
    const std::vector<Entity>& entities,
    const std::pair<unsigned int, unsigned int>& interval
) {
    trajectory_map sub_trajectories;

    for (size_t itr = 0; itr < entities.size(); itr++) {
        Entity curr = entities[itr];

        sub_trajectories[curr.id] = {
            curr.trajectories.begin() + interval.first,
            curr.trajectories.begin() + interval.second
        };
    }

    return sub_trajectories;
}

/**
 * Get directional vectors for all entities in a time interval
 * 
 * @param entities list of entities
 * @param interval time interval
 */
direction_map get_directional_vectors(
    const std::vector<Entity>& entities,
    const std::pair<unsigned int, unsigned int>& interval
) {
    unsigned int dimension = entities[0].trajectories[0].size();

    direction_map directional_vectors;

    for (size_t itr = 0; itr < entities.size(); itr++) {
        Entity curr = entities[itr];
        std::vector<double> end_pos = curr.trajectories[interval.second - 1];
        std::vector<double> start_pos = curr.trajectories[interval.first];

        for (size_t curr_dimension = 0; curr_dimension < dimension; curr_dimension++) {
            directional_vectors[curr.id].push_back(
                end_pos[curr_dimension] - start_pos[curr_dimension]
            );
        }
    }

    return directional_vectors;
}

void extend_current_rombongan(
    std::vector<std::vector<unsigned int> >& groups,
    const Entity& other,
    trajectory_map& sub_trajectories,
    direction_map& direction_vector,
    const Parameter& params
) {
    double r = params.r;
    double cs = params.cs;

    for (size_t groups_itr = 0; groups_itr < groups.size(); groups_itr++) {
        bool is_similar_to_all_members = true;

        for (size_t member_itr = 0; member_itr < groups[groups_itr].size(); member_itr++) {
            unsigned int member_id = groups[groups_itr][member_itr];
        
            double dtw_distance = calculate_dtw_distance(
                sub_trajectories[other.id],
                sub_trajectories[member_id]
            );

            double cosine_similarity = calculate_cosine_similarity(
                direction_vector[other.id],
                direction_vector[member_id]
            );

            // make sure that the distance is not zero to avoid
            // similarity checking when two entities
            // doesn't appear in the current timeframe.
            is_similar_to_all_members = member_id != other.id && 
                !std::isnan(dtw_distance) &&
                dtw_distance <= r &&
                cosine_similarity >= cs;

                if (!is_similar_to_all_members) {
                    break;
                }
        }

        if (is_similar_to_all_members) {
            groups[groups_itr].push_back(other.id);
        }
    }
}

void extend_rombongan_duration(
    rombongan_lifespan& groups,
    std::vector<std::vector<unsigned int> >& current_groups,
    const std::pair<unsigned int, unsigned int>& interval
) {
    unsigned int start = interval.first;
    unsigned int end = interval.second;

    for (std::vector<unsigned int> group: current_groups) {
        std::vector<std::pair<unsigned int, unsigned int> > time_list = groups[group];

        if (
            time_list.size() == 0 ||
            start == 0 ||
            (end - 1) != time_list[time_list.size() - 1].second
        ) {
            groups[group].push_back({ start, end });
        } else {
            groups[group][time_list.size() - 1] = { 
                groups[group][time_list.size() - 1].first,
                end
            };
        }
    }
}

/**
 * Identify rombongan from a set of moving entities.
 * 
 * @param entities - list of moving entities in two-dimensional space
 */
std::vector<Rombongan> identify_rombongan(
    const std::vector<Entity>& entities,
    const Parameter& params
) {
    int m = params.m;
    int k = params.k;
    double r = params.r;
    double cs = params.cs;

    rombongan_lifespan rombongan_list;
    
    unsigned int frame_count = entities[0].trajectories.size();

    for (size_t end = k; end < frame_count; end++) {
        unsigned int start = end - k;

        std::vector<std::vector<unsigned int> > current_rombongan;

        trajectory_map sub_trajectories = get_sub_trajectories(
            entities,
            { start, end }
        );
        direction_map direction_vector = get_directional_vectors(
            entities,
            { start, end }
        );

        for (size_t curr_itr = 0; curr_itr < entities.size() - 1; curr_itr++) {
            Entity curr = entities[curr_itr];

            if (!on_interval(curr, { start, end })) {
                continue;
            }

            std::vector<std::vector<unsigned int> > group_ids;

            for (size_t other_itr = curr_itr + 1; other_itr < entities.size(); other_itr++) {
                Entity other = entities[other_itr];

                if (!on_interval(other, { start, end })) {
                    continue;
                }

                extend_current_rombongan(
                    group_ids,
                    other,
                    sub_trajectories,
                    direction_vector,
                    params
                );

                // can entity `curr` create a subgroup with `other`?
                double dtw_distance = calculate_dtw_distance(
                    sub_trajectories[other.id],
                    sub_trajectories[curr.id]
                );

                double cosine_similarity = calculate_cosine_similarity(
                    direction_vector[other.id],
                    direction_vector[curr.id]
                );

                if (
                    !std::isnan(dtw_distance) &&
                    dtw_distance <= r &&
                    cosine_similarity >= cs
                ) {
                    group_ids.push_back({ curr.id, other.id });
                }
            }

            for (size_t itr_group = 0; itr_group < group_ids.size(); itr_group++) {
                if (group_ids[itr_group].size() >= m) {
                    current_rombongan.push_back(group_ids[itr_group]);
                }
            }
        }

        if (current_rombongan.size() > 0) {
            extend_rombongan_duration(
                rombongan_list,
                current_rombongan,
                { start, end }
            );
        }

        std::cout << "Finished processing range ";
        std::cout << "[" << start << ", " << end << "]" << std::endl;
    }

    std::vector<Rombongan> raw_result;

    for (auto const& [group, duration]: rombongan_list) {
        raw_result.push_back({
            group,
            duration
        });
    }

    return raw_result;
}
