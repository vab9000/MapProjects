#pragma once

#include <array>
#include <map>
#include <set>
#include <vector>
#include <functional>
#include "pop.hpp"
#include "province/province_properties.hpp"

class tag;
class river;
class unit;

class province {
    pop_container pops_;
    tag *owner_ = nullptr;
    std::map<province *, std::pair<double_t, uint_fast8_t> > neighbors_;
    std::set<province *> impassable_neighbors_;
    std::map<province *, uint_fast8_t> river_neighbors_;
    uint_fast32_t size_ = 0;
    bool distances_processed_ = false;
    std::array<uint_fast32_t, 4> bounds_{0, 0, 0, 0};
    std::array<uint_fast32_t, 2> center_{0, 0};
    koppen_t koppen_;
    elevation_t elevation_;
    vegetation_t vegetation_;
    soil_t soil_;
    sea_t sea_;
    uint_fast8_t value_ = 0;
    uint_fast32_t base_color_;
    uint_fast32_t color_;

public:
    province();

    province(uint_fast32_t color, koppen_t koppen, elevation_t elevation, vegetation_t vegetation,
             soil_t soil, sea_t sea);

    province(const province &) = delete;

    province &operator=(const province &) = delete;

    province &operator=(province &&) = default;

    // Do final calculations after all pixels have been added
    void finalize(const std::vector<std::array<uint_fast32_t, 2> > &pixels);

    // Calculate the distances to all neighbor provinces
    void process_distances();

    // Change the owner of the province
    void set_owner(tag *new_owner);

    // Remove the owner of the province
    void remove_owner();

    // Get the owner of the province
    [[nodiscard]] const tag &owner() const;

    // Check if the province has an owner
    [[nodiscard]] bool has_owner() const;

    // Add a pop to the province
    void add_pop(pop new_pop);

    // Remove a pop from the province and return a moved pop object that can be added elsewhere
    void remove_pop(pop *p);

    // Add a river boundary to the province with a specific size, linking it to a neighboring province
    void add_river_neighbor(province *neighbor, uint_fast8_t size);

    // Add a neighboring province to this province
    void add_neighbor(province *neighbor);

    // Expand the bounds of the province to include a new pixel
    void expand_bounds(uint_fast32_t x, uint_fast32_t y);

    // Recolor the province based on the current map mode
    void recolor(map_mode_t mode);

    // Get the base color of the province
    [[nodiscard]] uint_fast32_t base_color() const;

    // Get the color of the province based on the current map mode
    [[nodiscard]] uint_fast32_t color() const;

    // Get the koppen climate classification of the province
    [[nodiscard]] koppen_t koppen() const;

    // Get the elevation classification of the province
    [[nodiscard]] elevation_t elevation() const;

    // Get the vegetation classification of the province
    [[nodiscard]] vegetation_t vegetation() const;

    // Get the soil classification of the province
    [[nodiscard]] soil_t soil() const;

    // Get the sea classification of the province
    [[nodiscard]] sea_t sea() const;

    // Get the distance to another province
    [[nodiscard]] double_t distance(province *other) const;

    // Find the shortest path to another province using Dijkstra's algorithm
    template<typename T>
    [[nodiscard]] std::list<province *> path_to(province *destination,
                                                std::function<bool
                                                    (const province &start, const province &end, T *param)> accessible,
                                                std::function<double_t(const province &start, const province &end,
                                                                       T *param)> cost_modifier,
                                                T *param);

    // Get the bounds of the province as an array of [min_x, min_y, max_x, max_y]
    [[nodiscard]] const std::array<uint_fast32_t, 4> &bounds() const;

    // Get the center of the province as an array of [x, y]
    [[nodiscard]] const std::array<uint_fast32_t, 2> &center() const;

    // List of pops that are in this province
    [[nodiscard]] const pop_container &pops() const;

    // List of pops that are in this province
    pop_container &pops();

    // Get the neighbors of this province as a map of neighbor province pointers to distances
    [[nodiscard]] const std::map<province *, std::pair<double_t, uint_fast8_t> > &neighbors() const;

    // Get the neighbor provinces that are divided by rivers, with the river size
    [[nodiscard]] const std::map<province *, uint_fast8_t> &river_neighbors() const;

    // Get the neighbors of this province that are impassable
    [[nodiscard]] const std::set<province *> &impassable_neighbors() const;

    // Add an impassable neighbor to this province
    void add_impassable_neighbor(province *neighbor);

    [[nodiscard]] uint_fast32_t size() const;

    // Get some arbitrary value that is associated with the province, such as the size for a river province
    [[nodiscard]] uint_fast8_t value() const;

    void set_value(uint_fast8_t value);

    // Tick function to update the province state
    void tick();
};

#include <queue>

template<typename T>
std::list<province *> province::path_to(province *destination,
                                        std::function<bool(const province &start, const province &end, T *param)>
                                        accessible,
                                        std::function<double_t(const province &start, const province &end, T *param)>
                                        cost_modifier,
                                        T *param) {
    std::map<province *, double_t> distances;
    std::map<province *, province *> previous;
    std::priority_queue<std::pair<double_t, province *>, std::vector<std::pair<double_t,
                province *> >, std::greater<> >
            paths;

    distances[this] = 0;
    paths.emplace(0, this);

    auto reached = false;

    while (!paths.empty()) {
        auto [current_distance, current_province] = paths.top();
        paths.pop();

        if (current_province == destination) {
            reached = true;
            break;
        }

        for (const auto &[neighbor_province, neighbor_distance]: current_province->neighbors()) {
            if (!accessible(*this, *neighbor_province, param) ||
                cost_modifier(*this, *neighbor_province, param) < 0) {
                continue;
            }
            const double_t new_distance = current_distance + cost_modifier(*this, *neighbor_province, param) *
                                          neighbor_distance.first * (current_province->impassable_neighbors_.contains(
                                                                   neighbor_province)
                                                                   ? 10.0
                                                                   : 1.0);
            if (distances.contains(neighbor_province) && new_distance >= distances[neighbor_province]) {
                continue;
            }
            distances[neighbor_province] = new_distance;
            previous[neighbor_province] = current_province;
            paths.emplace(new_distance, neighbor_province);
        }
    }

    std::list<province *> path;
    if (!reached) {
        return path;
    }

    for (province *current_pos = destination; previous.contains(current_pos); current_pos = previous.at(current_pos)) {
        path.insert(path.begin(), current_pos);
    }

    return path;
}
