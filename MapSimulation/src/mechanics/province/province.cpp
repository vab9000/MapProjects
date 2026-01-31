#include "province.hpp"
#include <algorithm>
#include <execution>
#include <ranges>
#include "../society/tag.hpp"

namespace {
    auto points_distance(
        const std::pair<std::pair<int, int>, std::pair<int, int>> points) -> float {
        return sqrt((points.first.first - points.second.first) * (points.first.first - points.second.first) +
                    (points.first.second - points.second.second) * (points.first.second - points.second.second));
    }
}

namespace mechanics {
    province::province() : base_color_(0U), properties_{}, type_{} {}

    province::province(const province_properties_t properties, const unsigned int color) : base_color_{color},
        properties_{.province_properties = properties}, type_{location_type_t::province} {}

    province::province(const sea_properties_t properties, const unsigned int color) : base_color_{color},
        properties_{.sea_properties = properties}, type_{location_type_t::sea} {}

    province::province(const river_properties_t properties, const unsigned int color) : base_color_{color},
        properties_{.river_properties = properties}, type_{location_type_t::river} {}

    auto province::finalize(const std::vector<std::array<unsigned int, 2UZ>> &pixels) -> void {
        std::erase_if(neighbors_, [&](const std::pair<const utils::ref<province>, const province_connection> &pair) {
            return pair.second.border_size == 0;
        });

        if (type_ != location_type_t::province && (
                type_ == location_type_t::river || properties_.sea_properties.sea_type ==
                sea_t::coast || properties_.sea_properties.sea_type == sea_t::lake)) {
            std::unordered_map<koppen_t, int> koppen_types;
            for (const auto &neighbor : neighbors_ | std::views::keys) {
                if (neighbor.get().type_ != location_type_t::province) { continue; }
                if (const auto koppen_type = neighbor.get().properties_.province_properties.
                    koppen; koppen_types.contains(koppen_type)) { koppen_types.at(koppen_type) += 1; }
                else { koppen_types.emplace(koppen_type, 1); }
            }

            if (type_ == location_type_t::river) {
                if (!koppen_types.empty()) {
                    properties_.river_properties.koppen =
                        std::ranges::max_element(koppen_types.begin(), koppen_types.end(),
                            [](const auto &a, const auto &b) { return a.second < b.second; })->first;
                }
                else {
                    for (const auto &s_neighbor : neighbors_ | std::views::keys) {
                        for (const auto &neighbor : s_neighbor.get().neighbors() | std::views::keys) {
                            if (neighbor.get().type_ != location_type_t::province) { continue; }
                            if (const auto koppen_type = neighbor.get().properties_.province_properties.
                                koppen; koppen_types.contains(koppen_type)) { koppen_types.at(koppen_type) += 1; }
                            else { koppen_types.emplace(koppen_type, 1); }
                        }
                    }
                    if (!koppen_types.empty()) {
                        properties_.river_properties.koppen =
                            std::ranges::max_element(koppen_types.begin(), koppen_types.end(),
                                [](const auto &a, const auto &b) { return a.second < b.second; })->first;
                    }
                    else {
                        throw std::runtime_error(
                            "River province has no neighboring land provinces to determine koppen type");
                    }
                }
            }
            else {
                if (!koppen_types.empty()) {
                    properties_.sea_properties.koppen =
                        std::ranges::max_element(koppen_types.begin(), koppen_types.end(),
                            [](const auto &a, const auto &b) { return a.second < b.second; })->first;
                }
                else {
                    throw std::runtime_error("Sea province has no neighboring land provinces to determine koppen type");
                }
            }
        }

        size_ = static_cast<unsigned int>(pixels.size());

        auto x = std::vector<unsigned int>();
        x.reserve(size_);
        auto y = std::vector<unsigned int>();
        y.reserve(size_);

        for (const auto &pixel : pixels) {
            x.push_back(pixel[0UZ]);
            y.push_back(pixel[1UZ]);
        }

        std::ranges::sort(x);
        std::ranges::sort(y);

        std::array test_center = {0U, 0U};

        test_center[0UZ] = x[size_ / 2UZ];
        test_center[1UZ] = y[size_ / 2UZ];

        for (auto i = 0U; i < size_; ++i) {
            if (pixels[i][0UZ] == test_center[0UZ] && pixels[i][1UZ] == test_center[1UZ]) {
                center_[0UZ] = test_center[0UZ];
                center_[1UZ] = test_center[1UZ];
                break;
            }
        }

        if (center_[0UZ] == 0U && center_[1UZ] == 0U) {
            auto min_distance = (std::numeric_limits<float>::max)();
            for (auto i = 0U; i < size_; ++i) {
                if (const auto dist = points_distance({
                    {pixels[i][0UZ], pixels[i][1UZ]}, {test_center[0UZ], test_center[1UZ]}
                }); dist < min_distance) {
                    min_distance = dist;
                    center_[0UZ] = pixels[i][0UZ];
                    center_[1UZ] = pixels[i][1UZ];
                }
            }
        }
    }

    auto province::process_distances() -> void {
        if (size_ == 0U) { return; }

        std::erase_if(neighbors_, [&](const auto &pair) { return pair.first.get().size_ == 0U; });

        for (const auto neighbor : neighbors_ | std::views::keys) {
            neighbors_.at(neighbor).distance = points_distance({
                {center_[0UZ], center_[1UZ]},
                {neighbor.get().center_[0UZ], neighbor.get().center_[1UZ]}
            });
        }

        std::erase_if(river_neighbors_, [&](const auto &pair) { return pair.first.get().size_ == 0U; });
    }

    auto province::set_owner(tag &new_owner) -> void {
        if (owner_ != nullptr && owner_->has_province(*this)) { owner_->remove_province(*this); }
        new_owner.add_province(*this);
        owner_ = &new_owner;
    }

    auto province::remove_owner() -> void {
        if (owner_ != nullptr) { owner_->remove_province(*this); }
        owner_ = nullptr;
    }

    auto province::owner() const -> tag * { return owner_; }

    auto province::add_pop(pop &&new_pop) -> void {
        pops_.emplace_back(std::move(std::make_unique<pop>(std::move(new_pop))));
    }

    auto province::remove_pop(pop &p) -> void {
        std::erase_if(pops_, [&p](const std::unique_ptr<pop> &elem) { return elem.get() == &p; });
    }

    auto province::add_river_neighbor(province &neighbor, const unsigned char size) -> void {
        river_neighbors_[neighbor] = size;
    }

    auto province::add_neighbor(province &neighbor) -> void {
        if (neighbors_.contains(neighbor)) {
            neighbors_.at(neighbor).border_size += 1;
            return;
        }

        neighbors_.emplace(neighbor, province_connection{.distance = 0.0, .border_size = 1, .impassable = false});
    }

    auto province::expand_bounds(const std::array<unsigned int, 2UZ> coords) -> void {
        if (size_ == 0U) {
            bounds_[0UZ] = coords[0UZ];
            bounds_[1UZ] = coords[1UZ];
            bounds_[2UZ] = coords[0UZ];
            bounds_[3UZ] = coords[1UZ];
            size_ += 1U;
            return;
        }
        if (coords[0UZ] < bounds_[0UZ]) { bounds_[0UZ] = coords[0UZ]; }
        else if (coords[0UZ] > bounds_[2UZ]) { bounds_[2UZ] = coords[0UZ]; }
        if (coords[1UZ] < bounds_[1UZ]) { bounds_[1UZ] = coords[1UZ]; }
        else if (coords[1UZ] > bounds_[3UZ]) { bounds_[3UZ] = coords[1UZ]; }
    }

    auto province::units() const -> const std::set<utils::ref<unit>> & { return units_; }

    auto province::add_unit(unit &u) -> void { units_.emplace(u); }

    auto province::remove_unit(unit &u) -> void { units_.erase(u); }

    auto province::base_color() const -> unsigned int { return base_color_; }

    auto province::type() const -> location_type_t { return type_; }

    auto province::distance(const province &other) const -> float {
        if (neighbors_.contains(const_cast<province &>(other))) {
            return neighbors_.at(const_cast<province &>(other)).distance;
        }
        return points_distance({{center_[0UZ], center_[1UZ]}, {other.center_[0UZ], other.center_[1UZ]}});
    }

    auto province::bounds() const -> const std::array<unsigned int, 4UZ> & { return bounds_; }

    auto province::center() const -> const std::array<unsigned int, 2UZ> & { return center_; }

    auto province::pops() const -> const std::vector<std::unique_ptr<pop>> & { return pops_; }

    auto province::pops() -> std::vector<std::unique_ptr<pop>> & { return pops_; }

    auto province::neighbors() const -> const std::map<utils::ref<province>, province_connection> & {
        return neighbors_;
    }

    auto province::river_neighbors() const -> const std::map<utils::ref<province>, unsigned char> & {
        return river_neighbors_;
    }

    auto province::add_impassable_neighbor(province &neighbor) -> void { neighbors_[neighbor].impassable = true; }

    auto province::size() const -> unsigned int { return size_; }

    auto province::tick(tick_t tick_type) -> void {
        std::for_each(std::execution::seq, pops_.begin(), pops_.end(),
            [tick_type](const std::unique_ptr<pop> &elem) { elem->tick(tick_type); });
    }
}
