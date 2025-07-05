#pragma once

#include <array>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <list>
#include "pop.hpp"
#include "province/province_properties.hpp"

class tag;
class river;
class unit;

class province {
    std::list<std::unique_ptr<pop> > pops_;
    bool distances_processed_ = false;
    tag *owner_;
    std::map<province *, double_t> neighbors_;
    std::map<province *, uint_fast8_t> river_boundaries_;
    std::vector<river *> rivers_;
    uint_fast32_t size_;
    std::array<uint_fast32_t, 4> bounds_;
    std::array<uint_fast32_t, 2> center_;
    koppen_t koppen_;
    elevation_t elevation_;
    vegetation_t vegetation_;
    soil_t soil_;
    sea_t sea_;
    uint_fast32_t base_color_;
    uint_fast32_t color_;

public:
    province();

    province(uint_fast32_t color, koppen_t koppen, elevation_t elevation, vegetation_t vegetation,
             soil_t soil, sea_t sea);

    province(const province &) = delete;

    province &operator=(const province &) = delete;

    // Do final calculations after all pixels have been added
    void finalize(const std::vector<std::array<uint_fast32_t, 2> > &pixels);

    // Calculate the distances to all neighbor provinces
    void process_distances();

    // Change the owner of the province
    void set_owner(tag *new_owner);

    // Remove the owner of the province
    void remove_owner();

    // Get the owner of the province
    [[nodiscard]] tag *owner() const;

    // Check if the province has an owner
    [[nodiscard]] bool has_owner() const;

    // Add a pop to the province
    void add_pop(std::unique_ptr<pop> &&new_pop);

    // Add a pop to the province
    void add_pop(pop new_pop);

    // Add a river boundary to the province with a specific size, linking it to a neighboring province
    void add_river_boundary(province *neighbor, uint_fast8_t size);

    // Add a river to the province
    void add_river(river *r);

    // Add a neighboring province to this province
    void add_neighbor(province *neighbor);

    // Expand the bounds of the province to include a new pixel
    void expand_bounds(uint_fast32_t x, uint_fast32_t y);

    // Recolor the province based on the current map mode
    void recolor(map_modes mode);

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
    [[nodiscard]] double distance(const province &other) const;

    // Find the shortest path to another province using Dijkstra's algorithm
    template<typename T>
    [[nodiscard]] std::list<province *> path_to(const province &destination,
        std::function<bool(const province &start, const province &end, T *param)> accessible,
        std::function<double_t(const province &start, const province &end, T *param)> cost_modifier,
                                                  T *param);

    // Get the bounds of the province as an array of [min_x, min_y, max_x, max_y]
    [[nodiscard]] const std::array<uint_fast32_t, 4> &bounds() const;

    // Get the center of the province as an array of [x, y]
    [[nodiscard]] const std::array<uint_fast32_t, 2> &center() const;

    // List of pops that are in this province
    [[nodiscard]] const std::list<std::unique_ptr<pop> > &pops() const;

    // Get the neighbors of this province as a map of neighbor province pointers to distances
    [[nodiscard]] const std::map<province *, double_t> &neighbors() const;

    // Get the rivers that flow through this province as a map of neighbor province pointers to river sizes
    [[nodiscard]] const std::map<province *, uint_fast8_t> &river_boundaries() const;

    // Get the rivers that flow through this province as a vector of river pointers
    [[nodiscard]] const std::vector<river *> &rivers() const;

    // Tick function to update the province state
    void tick();
};

#include <queue>

template<typename T>
std::list<province *> province::path_to(const province &destination,
                                        std::function<bool(const province &start, const province &end, T *param)>
                                        accessible,
                                        std::function<double_t(const province &start, const province &end, T *param)>
                                        cost_modifier,
                                        T *param) {
    std::map<province *, double_t> distances;
    std::map<province *, province *> previous;
    std::priority_queue<std::pair<double_t, province *>, std::vector<std::pair<double_t,
                province *> >, std::greater<> >
            queue;

    distances[this] = 0;
    queue.emplace(0, this);

    auto reached = false;

    while (!queue.empty()) {
        auto [current_distance, current_province] = queue.top();
        queue.pop();

        if (current_province == &destination) {
            reached = true;
            break;
        }

        for (const auto &[neighbor_province, neighbor_distance]: current_province->neighbors()) {
            if (!accessible(*this, *neighbor_province, param) ||
                cost_modifier(*this, *neighbor_province, param) < 0) {
                continue;
            }

            if (double_t new_distance = current_distance + cost_modifier(*this, *neighbor_province, param) *
                                        neighbor_distance;
                !distances.contains(neighbor_province) || new_distance < distances[neighbor_province]) {
                distances[neighbor_province] = new_distance;
                previous[neighbor_province] = current_province;
                queue.emplace(new_distance, neighbor_province);
            }
        }
    }

    if (!reached) {
        return {};
    }

    std::list<province *> path;
    for (province *at = &const_cast<province &>(destination); at != nullptr; at = previous[at]) {
        path.insert(path.begin(), at);
    }

    return path;
}
