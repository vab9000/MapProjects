#pragma once

#include <array>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <list>
#include "utils.hpp"
#include "../populations/pop.hpp"
#include "province_properties.hpp"

class tag;
class river;
class unit;
enum class map_mode;

class province {
    std::list<std::unique_ptr<pop> > pops_;
    bool distances_processed_ = false;
    tag *owner_;
    std::unordered_map<province *, double> neighbors_;
    std::unordered_map<province *, int> river_boundaries_;
    std::vector<river *> rivers_;
    unsigned int size_;
    std::array<int, 4> bounds_;
    std::array<int, 2> center_;
    std::string name_;
    koppen_t koppen_;
    elevation_t elevation_;
    vegetation_t vegetation_;
    soil_t soil_;
    sea_t sea_;
    unsigned int base_color_;
    unsigned int color_;

public:
    province();

    province(std::string name, unsigned int color, koppen_t koppen, elevation_t elevation, vegetation_t vegetation,
             soil_t soil, sea_t sea);

    province(const province &) = delete;

    province &operator=(const province &) = delete;

    // Do final calculations after all pixels have been added
    void finalize(const std::vector<std::array<int, 2> > &pixels);

    // Calculate the distances to all neighbor provinces
    void process_distances();

    // Get the name of the province
    std::string_view name();

    // Change the owner of the province
    void set_owner(tag *new_owner);

    // Remove the owner of the province
    void remove_owner();

    // Get the owner of the province
    [[nodiscard]] tag *owner() const;

    // Check if the province has an owner
    [[nodiscard]] bool has_owner() const;

    // Add a river boundary to the province with a specific size, linking it to a neighboring province
    void add_river_boundary(province *neighbor, int size);

    // Add a river to the province
    void add_river(river *r);

    // Add a neighboring province to this province
    void add_neighbor(province *neighbor);

    // Expand the bounds of the province to include a new pixel
    void expand_bounds(int x, int y);

    // Recolor the province based on the current map mode
    void recolor(map_modes mode);

    // Get the base color of the province
    [[nodiscard]] unsigned int base_color() const;

    // Get the color of the province based on the current map mode
    [[nodiscard]] unsigned int color() const;

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
    [[nodiscard]] std::vector<province *> path_to(const province &destination,
                                                  bool (*accessible)(const province &start, const province &end,
                                                                     void *param),
                                                  double (*cost_modifier)(
                                                      const province &start, const province &end, void *param),
                                                  void *param);

    // Get the bounds of the province as an array of [min_x, min_y, max_x, max_y]
    [[nodiscard]] const std::array<int, 4> &bounds() const;

    // Get the center of the province as an array of [x, y]
    [[nodiscard]] const std::array<int, 2> &center() const;

    // List of pops that are in this province
    [[nodiscard]] const std::list<std::unique_ptr<pop> > &pops() const;

    // Get the neighbors of this province as a map of neighbor province pointers to distances
    [[nodiscard]] const std::unordered_map<province *, double> &neighbors() const;

    // Get the rivers that flow through this province as a map of neighbor province pointers to river sizes
    [[nodiscard]] const std::unordered_map<province *, int> &river_boundaries() const;

    // Get the rivers that flow through this province as a vector of river pointers
    [[nodiscard]] const std::vector<river *> &rivers() const;

    // Tick function to update the province state
    void tick();
};
