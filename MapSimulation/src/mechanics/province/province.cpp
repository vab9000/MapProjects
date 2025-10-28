#include "province.hpp"
#include <algorithm>
#include <colors.hpp>
#include <execution>
#include <ranges>
#include "../map_mode.hpp"
#include "../society/tag.hpp"

namespace {
    auto points_distance(
        const std::pair<std::pair<int, int>, std::pair<int, int>> points) -> float {
        return sqrt(pow(points.first.first - points.second.first, 2) +
                    pow(points.first.second - points.second.second, 2));
    }

    auto koppen_color(const mechanics::koppen_t koppen) -> unsigned int {
        switch (koppen) {
            case mechanics::koppen_t::none: return 0xFFFFFFU;
            case mechanics::koppen_t::af: return utils::to_integer_color(0U, 0U, 255U);
            case mechanics::koppen_t::am: return utils::to_integer_color(0U, 120U, 255U);
            case mechanics::koppen_t::aw: return utils::to_integer_color(70U, 170U, 250U);
            case mechanics::koppen_t::bwh: return utils::to_integer_color(255U, 0U, 0U);
            case mechanics::koppen_t::bwk: return utils::to_integer_color(255U, 150U, 150U);
            case mechanics::koppen_t::bsh: return utils::to_integer_color(245U, 165U, 0U);
            case mechanics::koppen_t::bsk: return utils::to_integer_color(255U, 220U, 100U);
            case mechanics::koppen_t::csa: return utils::to_integer_color(255U, 255U, 0U);
            case mechanics::koppen_t::csb: return utils::to_integer_color(200U, 200U, 0U);
            case mechanics::koppen_t::csc: return utils::to_integer_color(150U, 150U, 0U);
            case mechanics::koppen_t::cwa: return utils::to_integer_color(150U, 255U, 150U);
            case mechanics::koppen_t::cwb: return utils::to_integer_color(100U, 200U, 100U);
            case mechanics::koppen_t::cwc: return utils::to_integer_color(50U, 150U, 50U);
            case mechanics::koppen_t::cfa: return utils::to_integer_color(200U, 255U, 80U);
            case mechanics::koppen_t::cfb: return utils::to_integer_color(100U, 255U, 80U);
            case mechanics::koppen_t::cfc: return utils::to_integer_color(50U, 200U, 0U);
            case mechanics::koppen_t::dsa: return utils::to_integer_color(255U, 0U, 255U);
            case mechanics::koppen_t::dsb: return utils::to_integer_color(200U, 0U, 200U);
            case mechanics::koppen_t::dsc: return utils::to_integer_color(150U, 50U, 150U);
            case mechanics::koppen_t::dsd: return utils::to_integer_color(150U, 100U, 150U);
            case mechanics::koppen_t::dwa: return utils::to_integer_color(170U, 175U, 255U);
            case mechanics::koppen_t::dwb: return utils::to_integer_color(90U, 120U, 220U);
            case mechanics::koppen_t::dwc: return utils::to_integer_color(78U, 80U, 180U);
            case mechanics::koppen_t::dwd: return utils::to_integer_color(50U, 0U, 135U);
            case mechanics::koppen_t::dfa: return utils::to_integer_color(0U, 255U, 255U);
            case mechanics::koppen_t::dfb: return utils::to_integer_color(55U, 200U, 255U);
            case mechanics::koppen_t::dfc: return utils::to_integer_color(0U, 125U, 125U);
            case mechanics::koppen_t::dfd: return utils::to_integer_color(0U, 70U, 95U);
            case mechanics::koppen_t::et: return utils::to_integer_color(178U, 178U, 178U);
            case mechanics::koppen_t::ef: return utils::to_integer_color(102U, 102U, 102U);
        }
        return 0xFFFFFFU;
    }

    auto vegetation_color(const mechanics::vegetation_t vegetation) -> unsigned int {
        switch (vegetation) {
            case mechanics::vegetation_t::none: return 0xFFFFFFU;
            case mechanics::vegetation_t::tropical_evergreen_broadleaf_forest: return utils::to_integer_color(28U, 85U,
                    16U);
            case mechanics::vegetation_t::tropical_semi_evergreen_broadleaf_forest: return utils::to_integer_color(101U,
                    146U, 8U);
            case mechanics::vegetation_t::tropical_deciduous_broadleaf_forest_and_woodland: return
                    utils::to_integer_color(174U,
                        125U, 32U);
            case mechanics::vegetation_t::warm_temperate_evergreen_and_mixed_forest: return utils::to_integer_color(0U,
                    0U, 101U);
            case mechanics::vegetation_t::cool_temperate_rainforest: return utils::to_integer_color(187U, 203U, 53U);
            case mechanics::vegetation_t::cool_evergreen_needleleaf_forest: return utils::to_integer_color(0U, 154U,
                    24U);
            case mechanics::vegetation_t::cool_mixed_forest: return utils::to_integer_color(202U, 255U, 202U);
            case mechanics::vegetation_t::temperate_deciduous_broadleaf_forest: return utils::to_integer_color(85U,
                    235U, 73U);
            case mechanics::vegetation_t::cold_deciduous_forest: return utils::to_integer_color(101U, 178U, 255U);
            case mechanics::vegetation_t::cold_evergreen_needleleaf_forest: return utils::to_integer_color(0U, 32U,
                    202U);
            case mechanics::vegetation_t::temperate_sclerophyll_woodland_and_shrubland: return utils::to_integer_color(
                    142U, 162U,
                    40U);
            case mechanics::vegetation_t::temperate_evergreen_needleleaf_open_woodland: return utils::to_integer_color(
                    255U, 154U,
                    223U);
            case mechanics::vegetation_t::tropical_savanna: return utils::to_integer_color(186U, 255U, 53U);
            case mechanics::vegetation_t::xerophytic_woods_scrub: return utils::to_integer_color(255U, 186U, 154U);
            case mechanics::vegetation_t::steppe: return utils::to_integer_color(255U, 186U, 53U);
            case mechanics::vegetation_t::desert: return utils::to_integer_color(247U, 255U, 202U);
            case mechanics::vegetation_t::graminoid_and_forb_tundra: return utils::to_integer_color(231U, 231U, 24U);
            case mechanics::vegetation_t::erect_dwarf_shrub_tundra: return utils::to_integer_color(121U, 134U, 73U);
            case mechanics::vegetation_t::low_and_high_shrub_tundra: return utils::to_integer_color(101U, 255U, 154U);
            case mechanics::vegetation_t::prostrate_dwarf_shrub_tundra: return
                    utils::to_integer_color(210U, 158U, 150U);
        }
        return 0xFFFFFFU;
    }

    auto soil_color(const mechanics::soil_t soil) -> unsigned int {
        switch (soil) {
            case mechanics::soil_t::none: return 0xFFFFFFU;
            case mechanics::soil_t::acrisols: return utils::to_integer_color(247U, 153U, 29U);
            case mechanics::soil_t::albeluvisols: return utils::to_integer_color(155U, 157U, 87U);
            case mechanics::soil_t::alisols: return utils::to_integer_color(250U, 247U, 192U);
            case mechanics::soil_t::andosols: return utils::to_integer_color(237U, 58U, 51U);
            case mechanics::soil_t::arenosols: return utils::to_integer_color(247U, 216U, 172U);
            case mechanics::soil_t::calcisols: return utils::to_integer_color(255U, 238U, 0U);
            case mechanics::soil_t::cambisols: return utils::to_integer_color(254U, 205U, 103U);
            case mechanics::soil_t::chernozems: return utils::to_integer_color(226U, 200U, 55U);
            case mechanics::soil_t::cryosols: return utils::to_integer_color(117U, 106U, 146U);
            case mechanics::soil_t::durisols: return utils::to_integer_color(239U, 230U, 191U);
            case mechanics::soil_t::ferrasols: return utils::to_integer_color(246U, 135U, 45U);
            case mechanics::soil_t::fluvisols: return utils::to_integer_color(1U, 176U, 239U);
            case mechanics::soil_t::gleysols: return utils::to_integer_color(146U, 145U, 185U);
            case mechanics::soil_t::gypsisols: return utils::to_integer_color(251U, 246U, 165U);
            case mechanics::soil_t::histosols: return utils::to_integer_color(139U, 137U, 138U);
            case mechanics::soil_t::kastanozems: return utils::to_integer_color(201U, 149U, 128U);
            case mechanics::soil_t::leptosols: return utils::to_integer_color(213U, 214U, 216U);
            case mechanics::soil_t::lixisols: return utils::to_integer_color(249U, 189U, 191U);
            case mechanics::soil_t::luvisols: return utils::to_integer_color(244U, 131U, 133U);
            case mechanics::soil_t::nitisols: return utils::to_integer_color(247U, 160U, 130U);
            case mechanics::soil_t::phaeozems: return utils::to_integer_color(186U, 104U, 80U);
            case mechanics::soil_t::planosols: return utils::to_integer_color(245U, 147U, 84U);
            case mechanics::soil_t::plinthosols: return utils::to_integer_color(111U, 14U, 65U);
            case mechanics::soil_t::podzols: return utils::to_integer_color(13U, 175U, 99U);
            case mechanics::soil_t::regosols: return utils::to_integer_color(255U, 226U, 174U);
            case mechanics::soil_t::solonchaks: return utils::to_integer_color(237U, 57U, 148U);
            case mechanics::soil_t::solonetz: return utils::to_integer_color(244U, 205U, 226U);
            case mechanics::soil_t::stagnosols: return utils::to_integer_color(64U, 193U, 235U);
            case mechanics::soil_t::umbrisols: return utils::to_integer_color(97U, 143U, 130U);
            case mechanics::soil_t::vertisols: return utils::to_integer_color(158U, 86U, 124U);
        }
        return 0xFFFFFFU;
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

        for (const auto neighbor : river_neighbors_ | std::views::keys) {
            unsigned int x0 = center_[0UZ];
            unsigned int y0 = center_[1UZ];
            const unsigned int x1 = neighbor.get().center_[0UZ];
            const unsigned int y1 = neighbor.get().center_[1UZ];
            const auto dx = static_cast<int>(x1 > x0 ? x1 - x0 : x0 - x1);
            const int sx = x0 < x1 ? 1 : -1;
            const int dy = -static_cast<int>(y1 > y0 ? y1 - y0 : y0 - y1);
            const int sy = y0 < y1 ? 1 : -1;
            int err = dx + dy;
            int e2{};
            while (true) {
                set_pixel_flags({x0, y0}, pixel_flag_t::bridge, false);
                if (x0 == x1 && y0 == y1) { break; }
                e2 = 2 * err;
                if (e2 >= dy) {
                    err += dy;
                    x0 = static_cast<unsigned int>(static_cast<int>(x0) + sx);
                }
                if (e2 <= dx) {
                    err += dx;
                    y0 = static_cast<unsigned int>(static_cast<int>(y0) + sy);
                }
            }
        }
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

    auto province::color() const -> unsigned int {
        switch (map_mode) {
            case map_mode_t::provinces: return base_color_;
            case map_mode_t::owner: return owner_ != nullptr ? owner_->color() : 0xFFFFFFU;
            case map_mode_t::koppen: {
                switch (type_) {
                    case location_type_t::province: { return koppen_color(properties_.province_properties.koppen); }
                    case location_type_t::sea: {
                        if (properties_.sea_properties.sea_type == sea_t::coast ||
                            properties_.sea_properties.sea_type == sea_t::lake) {
                            return koppen_color(properties_.sea_properties.koppen);
                        }
                        return 0xFFFFFFU;
                    }
                    case location_type_t::river: { return koppen_color(properties_.river_properties.koppen); }
                }
            }
            case map_mode_t::elevation: {
                if (type_ != location_type_t::province) { return 0xFFFFFFU; }
                switch (properties_.province_properties.elevation) {
                    case elevation_level_t::lowland: return 0x00FF00U;
                    case elevation_level_t::midland: return 0x7F7F00U;
                    case elevation_level_t::highland: return 0x7F3F00U;
                    default: return 0xFFFFFFU;
                }
            }
            case map_mode_t::roughness: {
                if (type_ != location_type_t::province) { return 0xFFFFFFU; }
                switch (properties_.province_properties.roughness) {
                    case roughness_t::flat: return 0x00FF00U;
                    case roughness_t::hilly: return 0xFFFF00U;
                    case roughness_t::mountainous: return 0x7F3F00U;
                    default: return 0xFFFFFFU;
                }
            }
            case map_mode_t::vegetation: {
                if (type_ != location_type_t::province) { return 0xFFFFFFU; }
                return vegetation_color(properties_.province_properties.vegetation);
            }
            case map_mode_t::soil: {
                if (type_ != location_type_t::province) { return 0xFFFFFFU; }
                return soil_color(properties_.province_properties.soil);
            }
            case map_mode_t::sea: {
                if (type_ == location_type_t::river) { return 0x00FF00U; }
                if (type_ != location_type_t::sea) { return 0xFFFFFFU; }
                switch (properties_.sea_properties.sea_type) {
                    case sea_t::coast: return 0x00FFFFU;
                    case sea_t::lake: return 0xFF00FFU;
                    case sea_t::polar: return 0xAAAAFFU;
                    case sea_t::northeasterly: return 0xFFFF00U;
                    case sea_t::southeasterly: return 0xFFAA00U;
                    case sea_t::sea: return 0x0000AAU;
                    case sea_t::westerly: return 0xAAFF00U;
                    case sea_t::doldrums: return 0x00AAAAU;
                    default: return 0U;
                }
            }
            case map_mode_t::river_size: {
                if (type_ != location_type_t::river) { return 0xFFFFFFU; }
                return utils::to_integer_color(25U, 25U, (2U * 255U + properties_.river_properties.width) / 3U);
            }
        }
        return 0U;
    }

    auto province::type() const -> location_type_t { return type_; }

    auto province::distance(province &other) const -> float {
        if (neighbors_.contains(other)) { return neighbors_.at(other).distance; }
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
