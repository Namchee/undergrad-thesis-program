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
#include <algorithm>
#include <iostream>
#include <map>

typedef std::unordered_map<unsigned int, std::vector<std::vector<double> > > trajectory_map;
typedef std::unordered_map<unsigned int, std::vector<double> > direction_map;
typedef std::map<std::vector<unsigned int>, std::vector<std::pair<unsigned int, unsigned int> > > rombongan_lifespan;

/**
 * Determine is a list is a imperfect sublist of another.
 *
 * @param a - first list
 * @param b - second list
 * @return `true` if a list is a imperfect sublist of another,
 * `false` otherwise
 */
bool is_sublist(
    const std::vector<unsigned int>& a,
    const std::vector<unsigned int>& b
) {
    std::set<unsigned int> temp_container;
    temp_container.insert(a.begin(), a.end());
    temp_container.insert(b.begin(), b.end());

    return a.size() != b.size() &&
        (
            temp_container.size() == a.size() ||
            temp_container.size() == b.size()
        );
}

/**
 * Determine if an entity is present on a time interval
 * 
 * @param target_entity - target entity
 * @param interval - time interval
 * 
 * @return `true` if target is present in all timestamps,
 * `false` otherwise.
 */
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
 * Merge similar duration from child to parent, a.k.a deleting it
 * 
 * @param parent parent duration
 * @param child child duration
 */
void deduplicate_duration(
    std::vector<std::pair<unsigned int, unsigned int> >& parent,
    std::vector<std::pair<unsigned int, unsigned int> >& child
) {
    std::vector<std::pair<unsigned int, unsigned int> > deleted;

    for (size_t parent_itr = 0; parent_itr < parent.size(); parent_itr++) {
        std::pair<unsigned int, unsigned int> parent_duration = parent[parent_itr];

        for (size_t child_itr = 0; child_itr < child.size(); child_itr++) {
            std::pair<unsigned int, unsigned int> child_duration = child[child_itr];

            if (
                (parent_duration == child_duration) ||
                (child_duration.first > parent_duration.first && child_duration.second < parent_duration.second)
            ) {
                deleted.push_back(child_duration);
            }
        }
    }

    for (size_t del_itr = 0; del_itr < deleted.size() && child.size() > 0; del_itr++) {
        auto position = std::find(
            child.begin(),
            child.end(),
            deleted[del_itr]
        );

        if (position != child.end()) {
            child.erase(position);
        }
    }
}

/**
 * Clean rombongan identification result by merging
 * sub-rombongan into parent rombongan.
 * 
 * @param raw_result raw idenfication result
 *
 * @return cleaned identification result
 */
std::vector<Rombongan> deduplicate(
    std::vector<Rombongan>& raw_result
) {
    if (raw_result.size() == 0) {
        return raw_result;
    }

    std::sort(raw_result.begin(), raw_result.end());

    for (size_t curr_itr = 0; curr_itr < raw_result.size() - 1; curr_itr++) {
        if (raw_result[curr_itr].duration.size() == 0) {
            continue;
        }

        for (size_t other_itr = curr_itr + 1; other_itr < raw_result.size(); other_itr++) {
            if (raw_result[other_itr].duration.size() == 0) {
                continue;
            }

            if (
                is_sublist(raw_result[curr_itr].members, raw_result[other_itr].members)
            ) {
                deduplicate_duration(
                    raw_result[curr_itr].duration,
                    raw_result[other_itr].duration
                );

                
            }
        }
    }

    std::vector<Rombongan> clean_result;

    for (auto member: raw_result) {
        if (member.duration.size() > 0) {
            clean_result.push_back(member);
        }
    }

    sort(
        clean_result.begin(),
        clean_result.end(),
        [](const Rombongan& a, const Rombongan& b) -> bool {
            return a.duration[0].first < b.duration[0].first;
        }
    );

    return clean_result;
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

/**
 * Extends a rombongan duration
 * 
 * @param groups list of possible rombongan
 * @param other tested entity
 * @param sub_trajectories List of sub trajectories for the current time interval
 * @param direction_vector List of directional vectors for the current time interval
 * @param params Identification parameters
 */
void extend_current_rombongan(
    std::vector<std::vector<unsigned int> >& groups,
    const Entity& other,
    trajectory_map& sub_trajectories,
    direction_map& direction_vector,
    const Parameter& params
) {
    double r = params.r;
    double cs = params.cs;
    unsigned int m = params.m;

    for (size_t groups_itr = 0; groups_itr < groups.size(); groups_itr++) {
        unsigned int similarity_count = 0;
        unsigned int limit = params.c == 0 ? groups[groups_itr].size() : params.c;

        for (size_t member_itr = 0; member_itr < groups[groups_itr].size() && similarity_count < limit; member_itr++) {
            unsigned int member_id = groups[groups_itr][member_itr];

            if (member_id == other.id) {
                continue;
            }
        
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
            similarity_count += !std::isnan(dtw_distance) &&
                dtw_distance <= r &&
                cosine_similarity >= cs;
        }

        if (similarity_count >= limit) {
            groups[groups_itr].push_back(other.id);
        }
    }
}

/**
 * Extend identified rombongan duration by either appending
 * or replacing duration pairs
 * 
 * @param groups - list of identified rombongan
 * @param current_groups - list of newly identified rombongan
 * @param interval - time interval
 */
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
 * Determine if a rombongan candidate is not a subrombongan
 * of another
 * 
 * @param candidate - candidate rombongan
 * @param groups - list of identified rombongan
 * 
 * @return `true` if candidate is not a subrombongan,
 * `false` otherwise
 */
bool is_new_rombongan(
    const std::vector<unsigned int>& candidate,
    const std::vector<std::vector<unsigned int> >& groups
) {
    for (size_t group_itr = 0; group_itr < groups.size(); group_itr++) {
        if (
            groups[group_itr].size() > candidate.size() &&
            is_sublist(groups[group_itr], candidate)
        ) {
            return false;
        }
    }

    return true;
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
    unsigned int m = params.m;
    unsigned int k = params.k;
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
                    cosine_similarity >= cs &&
                    is_new_rombongan({ curr.id, other.id }, group_ids)
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

    return params.redundant ? raw_result : deduplicate(raw_result);
}
