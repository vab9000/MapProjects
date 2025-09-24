#pragma once
#include <array>
#include <functional>
#include <list>
#include <map>
#include <queue>
#include <reference.hpp>
#include <set>
#include <vector>
#include "map_mode.hpp"
#include "pop.hpp"
#include "settlement.hpp"
#include "tickable.hpp"
#include "province/province_properties.hpp"

namespace mechanics {
    class tag;
    class unit;
    class province;

    template<typename T, typename U>
    using province_connection_func = std::function<T(
        std::pair<utils::ref<const province>, utils::ref<const province>>, const U &)>;

    class province final : public tickable {
        size_t id_;
        tag *owner_{nullptr};
        std::array<unsigned int, 4UZ> bounds_{0U, 0U, 0U, 0U};
        std::array<unsigned int, 2UZ> center_{0U, 0U};
        unsigned int size_{0U};
        unsigned int base_color_;
        unsigned char value_{0U};
        koppen_t koppen_;
        elevation_t elevation_;
        vegetation_t vegetation_;
        soil_t soil_;
        sea_t sea_;
        std::map<utils::ref<province>, std::pair<double, unsigned char>> neighbors_;
        std::set<utils::ref<province>> impassable_neighbors_;
        std::map<utils::ref<province>, unsigned char> river_neighbors_;
        std::vector<std::unique_ptr<pop>> pops_;
        std::optional<settlement> settlement_;

    public:
        province();

        ~province() override = default;

        province(size_t id, koppen_t koppen, elevation_t elevation, vegetation_t vegetation,
            soil_t soil, sea_t sea, unsigned int color);

        province(const province &) = delete;

        province(province &&) noexcept = default;

        auto operator=(const province &) -> province & = delete;

        auto operator=(province &&) noexcept -> province & = default;

        [[nodiscard]] auto id() const -> size_t;

        // Do final calculations after all pixels have been added
        auto finalize(const std::vector<std::array<unsigned int, 2UZ>> &pixels) -> void;

        // Calculate the distances to all neighbor provinces
        auto process_distances() -> void;

        // Change the owner of the province
        auto set_owner(tag &new_owner) -> void;

        // Remove the owner of the province
        auto remove_owner() -> void;

        // Get the owner of the province
        [[nodiscard]] auto owner() const -> tag *;

        // Add a pop to the province
        auto add_pop(pop &&new_pop) -> void;

        // Remove a pop from the province
        auto remove_pop(pop &p) -> void;

        // Add a river boundary to the province with a specific size, linking it to a neighboring province
        auto add_river_neighbor(province &neighbor, unsigned char size) -> void;

        // Add a neighboring province to this province
        auto add_neighbor(province &neighbor) -> void;

        // Expand the bounds of the province to include a new pixel
        auto expand_bounds(std::array<unsigned int, 2UZ> coords) -> void;

        // Get the base color of the province
        [[nodiscard]] auto base_color() const -> unsigned int;

        // Get the color of the province based on the current map mode
        [[nodiscard]] auto color() const -> unsigned int;

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
        [[nodiscard]] auto distance(province &other) const -> double;

        // Find the shortest path to another province using Dijkstra's algorithm
        template<typename T>
        [[nodiscard]] auto path_to(province &destination,
            const province_connection_func<bool, T> &accessible,
            const province_connection_func<double, T> &cost_modifier,
            const T &param) -> std::list<utils::ref<province>>;

        // Get the bounds of the province as an array of [min_x, min_y, max_x, max_y]
        [[nodiscard]] auto bounds() const -> const std::array<unsigned int, 4UZ> &;

        // Get the center of the province as an array of [x, y]
        [[nodiscard]] auto center() const -> const std::array<unsigned int, 2UZ> &;

        // List of pops that are in this province
        [[nodiscard]] auto pops() const -> const std::vector<std::unique_ptr<pop>> &;

        // List of pops that are in this province
        auto pops() -> std::vector<std::unique_ptr<pop>> &;

        // Get the neighbors of this province as a map of neighbor province pointers to distances
        [[nodiscard]] auto neighbors() const -> const std::map<utils::ref<province>, std::pair<double,
            unsigned char>> &;

        // Get the neighbor provinces that are divided by rivers, with the river size
        [[nodiscard]] auto river_neighbors() const -> const std::map<utils::ref<province>, unsigned char> &;

        // Get the neighbors of this province that are impassable
        [[nodiscard]] auto impassable_neighbors() const -> const std::set<utils::ref<province>> &;

        // Add an impassable neighbor to this province
        auto add_impassable_neighbor(province &neighbor) -> void;

        [[nodiscard]] auto size() const -> unsigned int;

        [[nodiscard]] auto settlement() const -> const std::optional<settlement> &;

        // Get some arbitrary value associated with the province, such as the size for a river province
        [[nodiscard]] auto value() const -> unsigned char;

        auto set_value(unsigned char value) -> void;

        auto tick(tick_t tick_type) -> void override;
    };

    template<typename T>
    auto province::path_to(province &destination,
        const province_connection_func<bool, T> &accessible,
        const province_connection_func<double, T> &cost_modifier,
        const T &param) -> std::list<utils::ref<province>> {
        std::map<utils::ref<const province>, double> distances;
        std::map<utils::ref<province>, utils::ref<province>> previous;
        std::priority_queue<std::pair<double, utils::ref<province>>, std::vector<std::pair<double,
            utils::ref<province>>>, std::greater<>> paths;

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

            for (const auto &[neighbor_province, neighbor_distance] : current_province.get().neighbors()) {
                auto is_accessible = accessible({*this, neighbor_province.get()}, param);
                auto c_modifier = cost_modifier({*this, neighbor_province.get()}, param);
                if (!is_accessible || c_modifier < 0) { continue; }
                const double new_distance = current_distance + c_modifier
                                            * neighbor_distance.first * (
                                                current_province.get().impassable_neighbors_.contains(
                                                    neighbor_province)
                                                ? 10.0
                                                : 1.0);
                if (distances.contains(neighbor_province) && new_distance >= distances[neighbor_province]) {
                    continue;
                }
                distances.insert_or_assign(neighbor_province, new_distance);
                previous.insert_or_assign(neighbor_province, current_province);
                paths.emplace(new_distance, neighbor_province);
            }
        }

        std::list<utils::ref<province>> path;
        if (!reached) { return path; }

        utils::ref current_pos = destination;
        while (previous.contains(current_pos)) {
            path.insert(path.begin(), current_pos);
            current_pos = previous.at(current_pos);
        }

        return path;
    }
}

template<>
struct std::hash<utils::ref<mechanics::province>> {
    auto operator()(const utils::ref<mechanics::province> &rt) const noexcept -> std::size_t;
};
