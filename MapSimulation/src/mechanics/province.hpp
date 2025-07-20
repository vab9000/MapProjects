#pragma once
#include <array>
#include <functional>
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <vector>
#include "map_mode.hpp"
#include "pop.hpp"
#include "province/province_properties.hpp"

namespace mechanics {
    class tag;
    class river;
    class unit;

    class province {
        pop_container pops_;
        std::optional<std::reference_wrapper<tag> > owner_{std::nullopt};
        std::map<std::reference_wrapper<province>, std::pair<double_t, uint_fast8_t> > neighbors_;
        std::set<std::reference_wrapper<province> > impassable_neighbors_;
        std::map<std::reference_wrapper<province>, uint_fast8_t> river_neighbors_;
        uint_fast32_t size_{0};
        bool distances_processed_{false};
        std::array<uint_fast32_t, 4> bounds_{0, 0, 0, 0};
        std::array<uint_fast32_t, 2> center_{0, 0};
        koppen_t koppen_;
        elevation_t elevation_;
        vegetation_t vegetation_;
        soil_t soil_;
        sea_t sea_;
        uint_fast8_t value_{0};
        uint_fast32_t base_color_;
        uint_fast32_t color_;

    public:
        province();

        province(uint_fast32_t color, koppen_t koppen, elevation_t elevation, vegetation_t vegetation,
                 soil_t soil, sea_t sea);

        province(const province &) = delete;

        auto operator=(const province &) -> province & = delete;

        auto operator=(province &&) -> province & = default;

        friend auto operator==(const std::reference_wrapper<province> &lhs,
                               const std::reference_wrapper<province> &rhs) -> bool;

        friend auto operator==(const std::reference_wrapper<province> &lhs,
                               const std::reference_wrapper<const province> &rhs) -> bool;

        friend auto operator<=>(const std::reference_wrapper<const province> &lhs,
                                const std::reference_wrapper<const province> &rhs) -> std::strong_ordering;

        // Do final calculations after all pixels have been added
        auto finalize(const std::vector<std::array<uint_fast32_t, 2> > &pixels) -> void;

        // Calculate the distances to all neighbor provinces
        auto process_distances() -> void;

        // Change the owner of the province
        auto set_owner(tag &new_owner) -> void;

        // Remove the owner of the province
        auto remove_owner() -> void;

        // Get the owner of the province
        [[nodiscard]] auto owner() const -> std::optional<std::reference_wrapper<tag> >;

        // Add a pop to the province
        auto add_pop(pop new_pop) -> void;

        // Remove a pop from the province and return a moved pop object that can be added elsewhere
        auto remove_pop(pop &p) -> void;

        // Add a river boundary to the province with a specific size, linking it to a neighboring province
        auto add_river_neighbor(province &neighbor, uint_fast8_t size) -> void;

        // Add a neighboring province to this province
        auto add_neighbor(province &neighbor) -> void;

        // Expand the bounds of the province to include a new pixel
        auto expand_bounds(std::array<uint_fast32_t, 2> coords) -> void;

        // Recolor the province based on the current map mode
        auto recolor(map_mode_t mode) -> void;

        // Get the base color of the province
        [[nodiscard]] auto base_color() const -> uint_fast32_t;

        // Get the color of the province based on the current map mode
        [[nodiscard]] auto color() const -> uint_fast32_t;

        // Get the koppen climate classification of the province
        [[nodiscard]] auto koppen() const -> koppen_t;

        // Get the elevation classification of the province
        [[nodiscard]] auto elevation() const -> elevation_t;

        // Get the vegetation classification of the province
        [[nodiscard]] auto vegetation() const -> vegetation_t;

        // Get the soil classification of the province
        [[nodiscard]] auto soil() const -> soil_t;

        // Get the sea classification of the province
        [[nodiscard]] auto sea() const -> sea_t;

        // Get the distance to another province
        [[nodiscard]] auto distance(province &other) const -> double_t;

        // Find the shortest path to another province using Dijkstra's algorithm
        template<typename T>
        [[nodiscard]] auto path_to(province &destination,
                                   std::function<bool(
                                       std::pair<std::reference_wrapper<const province>, std::reference_wrapper<const
                                           province> > c,
                                       const T &param)> accessible,
                                   std::function<double_t(
                                       std::pair<std::reference_wrapper<const province>, std::reference_wrapper<const
                                           province> > c,
                                       const T &param)> cost_modifier,
                                   const T &param) -> std::list<std::reference_wrapper<province> >;

        // Get the bounds of the province as an array of [min_x, min_y, max_x, max_y]
        [[nodiscard]] auto bounds() const -> const std::array<uint_fast32_t, 4> &;

        // Get the center of the province as an array of [x, y]
        [[nodiscard]] auto center() const -> const std::array<uint_fast32_t, 2> &;

        // List of pops that are in this province
        [[nodiscard]] auto pops() const -> const pop_container &;

        // List of pops that are in this province
        auto pops() -> pop_container &;

        // Get the neighbors of this province as a map of neighbor province pointers to distances
        [[nodiscard]] auto neighbors() const -> const std::map<std::reference_wrapper<province>, std::pair<double_t,
            uint_fast8_t> > &;

        // Get the neighbor provinces that are divided by rivers, with the river size
        [[nodiscard]] auto river_neighbors() const -> const std::map<std::reference_wrapper<province>, uint_fast8_t> &;

        // Get the neighbors of this province that are impassable
        [[nodiscard]] auto impassable_neighbors() const -> const std::set<std::reference_wrapper<province> > &;

        // Add an impassable neighbor to this province
        auto add_impassable_neighbor(province &neighbor) -> void;

        [[nodiscard]] auto size() const -> uint_fast32_t;

        // Get some arbitrary value that is associated with the province, such as the size for a river province
        [[nodiscard]] auto value() const -> uint_fast8_t;

        auto set_value(uint_fast8_t value) -> void;

        // Tick function to update the province state
        auto tick() -> void;
    };

    template<typename T>
    auto province::path_to(province &destination,
                           std::function<bool(
                               std::pair<std::reference_wrapper<const province>, std::reference_wrapper<const
                                   province> > c,
                               const T &param)> accessible,
                           std::function<double_t (
                               std::pair<std::reference_wrapper<const province>, std::reference_wrapper<const
                                   province> > c,
                               const T &param)> cost_modifier,
                           const T &param) -> std::list<std::reference_wrapper<province> > {
        std::map<std::reference_wrapper<const province>, double_t> distances;
        std::map<std::reference_wrapper<province>, std::reference_wrapper<province> > previous;
        std::priority_queue<std::pair<double_t, std::reference_wrapper<province> >, std::vector<std::pair<double_t
            , std::reference_wrapper<province> > >, std::greater<> > paths;

        distances.emplace(*this, 0.0);
        paths.emplace(0, *this);

        auto reached = false;

        while (!paths.empty()) {
            auto [current_distance, current_province] = paths.top();
            paths.pop();

            if (&current_province.get() == &destination) {
                reached = true;
                break;
            }

            for (const auto &[neighbor_province, neighbor_distance]: current_province.get().neighbors()) {
                if (!accessible({*this, neighbor_province.get()}, param) ||
                    cost_modifier({*this, neighbor_province.get()}, param) < 0) { continue; }
                const double_t new_distance = current_distance + cost_modifier({*this, neighbor_province.get()}, param)
                                              * neighbor_distance.first * (
                                                  current_province.get().impassable_neighbors_.contains(
                                                      neighbor_province)
                                                      ? 10.0
                                                      : 1.0);
                if (distances.contains(neighbor_province) && new_distance >= distances[neighbor_province]) { continue; }
                distances.insert_or_assign(neighbor_province, new_distance);
                previous.insert_or_assign(neighbor_province, current_province);
                paths.emplace(new_distance, neighbor_province);
            }
        }

        std::list<std::reference_wrapper<province> > path;
        if (!reached) { return path; }

        for (std::reference_wrapper current_pos = destination; previous.contains(current_pos);
             current_pos = previous.at(current_pos)) { path.insert(path.begin(), current_pos); }

        return path;
    }
}

template<>
struct std::hash<std::reference_wrapper<mechanics::province> > {
    auto operator()(const std::reference_wrapper<mechanics::province> &rt) const noexcept -> std::size_t;
};
