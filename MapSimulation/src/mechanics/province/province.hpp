#ifndef MECHANICS_PROVINCE
#define MECHANICS_PROVINCE
#include <array>
#include <functional>
#include <map>
#include <numbered.hpp>
#include <queue>
#include <reference.hpp>
#include <set>
#include <vector>
#include "building.hpp"
#include "province_properties.hpp"
#include "../date.hpp"
#include "../life/pop.hpp"

namespace mechanics {
    class tag;
    class unit;
    class province;

    struct province_connection {
        float distance;
        unsigned char border_size;
        bool impassable;
    };

    template<typename T, typename U>
    using province_connection_func = std::function<T(
        std::pair<utils::ref<const province>, utils::ref<const province>>, const U &)>;

    class province final : public utils::numbered<province> {
        tag *owner_ = nullptr;
        std::array<unsigned int, 4UZ> bounds_{0U, 0U, 0U, 0U};
        std::array<unsigned int, 2UZ> center_{0U, 0U};
        unsigned int size_ = 0U;
        unsigned int base_color_;
        std::map<utils::ref<province>, province_connection> neighbors_;
        std::map<utils::ref<province>, unsigned char> river_neighbors_;
        std::vector<std::unique_ptr<pop>> pops_;
        std::vector<std::unique_ptr<building>> buildings_;
        std::set<utils::ref<unit>> units_;
        location_properties_t properties_;
        location_type_t type_;

    public:
        province();

        province(province_properties_t properties, unsigned int color);

        province(sea_properties_t properties, unsigned int color);

        province(river_properties_t properties, unsigned int color);

        province(const province &) = delete;

        province(province &&) noexcept = default;

        auto operator=(const province &) -> province & = delete;

        auto operator=(province &&) noexcept -> province & = default;

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

        [[nodiscard]] auto units() const -> const std::set<utils::ref<unit>> &;

        auto add_unit(unit &u) -> void;

        auto remove_unit(unit &u) -> void;

        // Get the base color of the province
        [[nodiscard]] auto base_color() const -> unsigned int;

        template<location_type_t T>
        [[nodiscard]] auto properties() const -> const location_properties_t::type<T> &;

        [[nodiscard]] auto type() const -> location_type_t;

        // Get the distance to another province
        [[nodiscard]] auto distance(const province &other) const -> float;

        // Find the shortest path to another province using Dijkstra's algorithm
        template<typename T>
        [[nodiscard]] auto path_to(province &destination,
            const province_connection_func<bool, T> &accessible,
            const province_connection_func<float, T> &cost_modifier,
            const T &param) -> std::vector<utils::ref<province>>;

        template<typename T>
        [[nodiscard]] auto path_to(const province &destination,
            const province_connection_func<bool, T> &accessible,
            const province_connection_func<float, T> &cost_modifier,
            const T &param) const -> std::vector<utils::ref<const province>>;

        // Get the bounds of the province as an array of [min_x, min_y, max_x, max_y]
        [[nodiscard]] auto bounds() const -> const std::array<unsigned int, 4UZ> &;

        // Get the center of the province as an array of [x, y]
        [[nodiscard]] auto center() const -> const std::array<unsigned int, 2UZ> &;

        // List of pops that are in this province
        [[nodiscard]] auto pops() const -> const std::vector<std::unique_ptr<pop>> &;

        // List of pops that are in this province
        auto pops() -> std::vector<std::unique_ptr<pop>> &;

        // Get the neighbors of this province as a map of neighbor province pointers to distances
        [[nodiscard]] auto neighbors() const -> const std::map<utils::ref<province>, province_connection> &;

        // Get the neighbor provinces that are divided by rivers, with the river size
        [[nodiscard]] auto river_neighbors() const -> const std::map<utils::ref<province>, unsigned char> &;

        // Add an impassable neighbor to this province
        auto add_impassable_neighbor(province &neighbor) -> void;

        [[nodiscard]] auto size() const -> unsigned int;

        auto tick(tick_t tick_type) -> void;
    };

    template<location_type_t T>
    auto province::properties() const -> const location_properties_t::type<T> & {
        if (T != type_) { throw std::runtime_error("Attempted to access province properties of incorrect type"); }
        if constexpr (T == location_type_t::province) { return properties_.province_properties; }
        else if constexpr (T == location_type_t::sea) { return properties_.sea_properties; }
        else { return properties_.river_properties; }
    }

    template<typename T>
    auto province::path_to(province &destination,
        const province_connection_func<bool, T> &accessible,
        const province_connection_func<float, T> &cost_modifier,
        const T &param) -> std::vector<utils::ref<province>> {
        std::map<utils::ref<const province>, float> distances;
        std::map<utils::ref<province>, utils::ref<province>> previous;
        std::priority_queue<std::pair<float, utils::ref<province>>, std::vector<std::pair<float,
            utils::ref<province>>>, std::greater<>> paths;

        distances.emplace(*this, 0.0F);
        paths.emplace(0.0F, *this);

        auto reached = false;

        while (!paths.empty()) {
            auto [current_distance, current_province] = paths.top();
            paths.pop();

            if (&current_province.get() == &destination) {
                reached = true;
                break;
            }

            for (const auto &[neighbor_province, n_info] : current_province.get().neighbors()) {
                auto is_accessible = accessible({*this, neighbor_province.get()}, param);
                auto c_modifier = cost_modifier({*this, neighbor_province.get()}, param);
                if (!is_accessible || c_modifier < 0) { continue; }
                const float new_distance = current_distance + c_modifier
                                           * n_info.distance * (n_info.impassable ? 10.0F : 1.0F);
                if (distances.contains(neighbor_province) && new_distance >= distances[neighbor_province]) { continue; }
                distances.insert_or_assign(neighbor_province, new_distance);
                previous.insert_or_assign(neighbor_province, current_province);
                paths.emplace(new_distance, neighbor_province);
            }
        }

        std::vector<utils::ref<province>> path;
        if (!reached) { return path; }

        utils::ref current_pos = destination;
        while (previous.contains(current_pos)) {
            path.push_back(current_pos);
            current_pos = previous.at(current_pos);
        }

        return path;
    }

    template<typename T>
    auto province::path_to(const province &destination, const province_connection_func<bool, T> &accessible,
        const province_connection_func<float, T> &cost_modifier,
        const T &param) const -> std::vector<utils::ref<const province>> {
        std::map<utils::ref<const province>, float> distances;
        std::map<utils::ref<const province>, utils::ref<const province>> previous;
        std::priority_queue<std::pair<float, utils::ref<const province>>, std::vector<std::pair<float,
            utils::ref<const province>>>, std::greater<>> paths;

        distances.emplace(*this, 0.0F);
        paths.emplace(0.0F, *this);

        auto reached = false;

        while (!paths.empty()) {
            auto [current_distance, current_province] = paths.top();
            paths.pop();

            if (&current_province.get() == &destination) {
                reached = true;
                break;
            }

            for (const auto &[neighbor_province, n_info] : current_province.get().neighbors()) {
                auto is_accessible = accessible({*this, neighbor_province.get()}, param);
                auto c_modifier = cost_modifier({*this, neighbor_province.get()}, param);
                if (!is_accessible || c_modifier < 0) { continue; }
                const float new_distance = current_distance + c_modifier
                                           * n_info.distance * (n_info.impassable ? 10.0F : 1.0F);
                if (distances.contains(neighbor_province) && new_distance >= distances[neighbor_province]) { continue; }
                distances.insert_or_assign(neighbor_province, new_distance);
                previous.insert_or_assign(neighbor_province, current_province);
                paths.emplace(new_distance, neighbor_province);
            }
        }

        std::vector<utils::ref<const province>> path;
        if (!reached) { return path; }

        utils::ref current_pos = destination;
        while (previous.contains(current_pos)) {
            path.push_back(current_pos);
            current_pos = previous.at(current_pos);
        }

        return path;
    }
}
#endif
