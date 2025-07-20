#include "province.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <ranges>
#include <vector>
#include "tag.hpp"

namespace mechanics {
    province::province() : koppen_(koppen_t::none), elevation_(elevation_t::none), vegetation_(vegetation_t::none),
                           soil_(soil_t::none), sea_(sea_t::none), base_color_(0), color_(0) {}

    province::province(const uint_fast32_t color, const koppen_t koppen,
                       const elevation_t elevation,
                       const vegetation_t vegetation, const soil_t soil, const sea_t sea) : koppen_(koppen),
        elevation_(elevation),
        vegetation_(vegetation),
        soil_(soil), sea_(sea), base_color_(color), color_(color) {}

    auto province::finalize(const std::vector<std::array<uint_fast32_t, 2> > &pixels) -> void {
        size_ = pixels.size();

        auto x = std::vector<uint_fast32_t>(size_);
        auto y = std::vector<uint_fast32_t>(size_);

        for (const auto &pixel: pixels) {
            x.push_back(pixel[0]);
            y.push_back(pixel[1]);
        }

        std::ranges::sort(x);
        std::ranges::sort(y);

        std::array<uint_fast32_t, 2> test_center = {0, 0};

        if (size_ % 2 == 0) {
            test_center[0] = (x[size_ / 2] + x[size_ / 2 - 1]) / 2;
            test_center[1] = (y[size_ / 2] + y[size_ / 2 - 1]) / 2;
        } else {
            test_center[0] = x[size_ / 2];
            test_center[1] = y[size_ / 2];
        }

        for (uint_fast32_t i = 0; i < size_; ++i) {
            if (pixels[i][0] == test_center[0] && pixels[i][1] == test_center[1]) {
                center_[0] = test_center[0];
                center_[1] = test_center[1];
                break;
            }
        }

        const auto distance = [](const std::array<uint_fast32_t, 2> &a, const std::array<uint_fast32_t, 2> &b) {
            return sqrt(
                pow(static_cast<int_fast32_t>(a[0]) - static_cast<int_fast32_t>(b[0]), 2) + pow(
                    static_cast<int_fast32_t>(a[1]) - static_cast<int_fast32_t>(b[1]), 2));
        };

        if (center_[0] == 0 && center_[1] == 0) {
            auto min_distance = (std::numeric_limits<double_t>::max)();
            for (uint_fast32_t i = 0; i < size_; ++i) {
                if (const auto dist = distance(pixels[i], {test_center[0], test_center[1]}); dist < min_distance) {
                    min_distance = dist;
                    center_[0] = pixels[i][0];
                    center_[1] = pixels[i][1];
                }
            }
        }
    }

    auto province::process_distances() -> void {
        for (const auto neighbor: neighbors_ | std::views::keys) { neighbors_.at(neighbor).first = distance(neighbor); }
        distances_processed_ = true;
    }

    auto province::set_owner(tag &new_owner) -> void {
        if (owner_.has_value() && owner_->get().has_province(*this)) { owner_->get().remove_province(*this); }
        new_owner.add_province(*this);
        owner_ = new_owner;
    }

    auto province::remove_owner() -> void {
        if (owner_.has_value()) { owner_->get().remove_province(*this); }
        owner_ = std::nullopt;
    }

    auto province::owner() const -> std::optional<std::reference_wrapper<tag> > { return owner_; }

    auto province::add_pop(pop new_pop) -> void { pops_.emplace_back(new_pop); }

    auto province::remove_pop(pop &p) -> void { std::erase_if(pops_, [p](const pop &elem) { return &elem == &p; }); }

    auto province::add_river_neighbor(province &neighbor, const uint_fast8_t size) -> void {
        river_neighbors_[neighbor] = size;
    }

    auto province::add_neighbor(province &neighbor) -> void {
        if (neighbors_.contains(neighbor)) {
            neighbors_.at(neighbor).second += 1;
            return;
        }

        neighbors_.emplace(neighbor, std::pair{0.0, 1});
    }

    auto province::expand_bounds(const std::array<uint_fast32_t, 2> coords) -> void {
        if (size_ == 0) {
            bounds_[0] = coords[0];
            bounds_[1] = coords[1];
            bounds_[2] = coords[0];
            bounds_[3] = coords[1];
            size_ += 1;
            return;
        }
        if (coords[0] < bounds_[0]) { bounds_[0] = coords[0]; } else if (coords[0] > bounds_[2]) {
            bounds_[2] = coords[0];
        }
        if (coords[1] < bounds_[1]) { bounds_[1] = coords[1]; } else if (coords[1] > bounds_[3]) {
            bounds_[3] = coords[1];
        }
    }

    auto province::recolor(const map_mode_t mode) -> void {
        switch (mode) {
            case map_mode_t::provinces:
                color_ = base_color_;
                break;
            case map_mode_t::owner:
                if (owner_.has_value()) { color_ = owner_->get().color(); } else { color_ = 0xFFFFFFFF; }
                break;
            case map_mode_t::koppen:
                color_ = static_cast<uint_fast32_t>(koppen_);
                break;
            case map_mode_t::elevation:
                color_ = static_cast<uint_fast32_t>(elevation_);
                break;
            case map_mode_t::vegetation:
                color_ = static_cast<uint_fast32_t>(vegetation_);
                break;
            case map_mode_t::soil:
                color_ = static_cast<uint_fast32_t>(soil_);
                break;
            case map_mode_t::sea:
                color_ = static_cast<uint_fast32_t>(sea_);
                break;
        }
    }

    auto province::base_color() const -> uint_fast32_t { return base_color_; }

    auto province::color() const -> uint_fast32_t { return color_; }

    auto province::koppen() const -> koppen_t { return koppen_; }

    auto province::elevation() const -> elevation_t { return elevation_; }

    auto province::vegetation() const -> vegetation_t { return vegetation_; }

    auto province::soil() const -> soil_t { return soil_; }

    auto province::sea() const -> sea_t { return sea_; }

    auto province::distance(province &other) const -> double_t {
        if (distances_processed_ && neighbors_.contains(other)) { return neighbors_.at(other).first; }
        return sqrt(pow(static_cast<int_fast32_t>(center_[0]) - static_cast<int_fast32_t>(other.center_[0]), 2) +
                    pow(static_cast<int_fast32_t>(center_[1]) - static_cast<int_fast32_t>(other.center_[1]), 2));
    }

    auto province::tick() -> void { if (owner_.has_value()) { owner_->get().add_gold(1); } }

    auto province::bounds() const -> const std::array<uint_fast32_t, 4> & { return bounds_; }

    auto province::center() const -> const std::array<uint_fast32_t, 2> & { return center_; }

    auto province::pops() const -> const pop_container & { return pops_; }

    auto province::pops() -> pop_container & { return pops_; }

    auto province::neighbors() const -> const std::map<std::reference_wrapper<province>, std::pair<double_t,
        uint_fast8_t> > & { return neighbors_; }

    auto province::river_neighbors() const -> const std::map<std::reference_wrapper<province>, uint_fast8_t> & {
        return river_neighbors_;
    }

    auto province::impassable_neighbors() const -> const std::set<std::reference_wrapper<province> > & {
        return impassable_neighbors_;
    }

    auto province::add_impassable_neighbor(province &neighbor) -> void { impassable_neighbors_.insert(neighbor); }

    auto province::size() const -> uint_fast32_t { return size_; }

    auto province::value() const -> uint_fast8_t { return value_; }

    auto province::set_value(const uint_fast8_t value) -> void { value_ = value; }

    auto operator==(const std::reference_wrapper<province> &lhs, const std::reference_wrapper<province> &rhs) -> bool {
        return &lhs.get() == &rhs.get();
    }

    auto operator==(const std::reference_wrapper<province> &lhs,
                    const std::reference_wrapper<const province> &rhs) -> bool { return &lhs.get() == &rhs.get(); }

    auto operator<=>(const std::reference_wrapper<const province> &lhs,
                     const std::reference_wrapper<const province> &rhs) -> std::strong_ordering {
        return &lhs.get() <=> &rhs.get();
    }
}

auto std::hash<std::reference_wrapper<mechanics::province> >::operator()(
    const std::reference_wrapper<mechanics::province> &rt) const noexcept -> std::size_t {
    return std::hash<mechanics::province *>()(&rt.get());
}
