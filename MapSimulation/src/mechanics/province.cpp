#include "province.hpp"
#include <algorithm>
#include <execution>
#include <ranges>
#include "tag.hpp"

namespace {
    auto points_distance(
        const std::pair<std::pair<uint_fast32_t, uint_fast32_t>, std::pair<uint_fast32_t, uint_fast32_t>> points) ->
        double_t {
        return sqrt(pow(points.first.first - points.second.first, 2) +
                    pow(points.first.second - points.second.second, 2));
    }
}

namespace mechanics {
    province::province() : koppen_(koppen_t::none), elevation_(elevation_t::none), vegetation_(vegetation_t::none),
        soil_(soil_t::none),
        sea_(sea_t::none), base_color_(0U), color_(0U), id_(0U) {}

    province::province(const size_t id, const koppen_t koppen,
        const elevation_t elevation, const vegetation_t vegetation, const soil_t soil, const sea_t sea,
        const uint_fast32_t color) : koppen_(koppen), elevation_(elevation), vegetation_(vegetation),
        soil_(soil), sea_(sea), base_color_(color), color_(color), id_(id) {}

    auto province::id() const -> size_t { return id_; }

    auto province::finalize(const std::vector<std::array<uint_fast32_t, 2UZ>> &pixels) -> void {
        size_ = static_cast<uint_fast32_t>(pixels.size());

        auto x = std::vector<uint_fast32_t>(size_);
        auto y = std::vector<uint_fast32_t>(size_);

        for (const auto &pixel : pixels) {
            x.push_back(pixel[0UZ]);
            y.push_back(pixel[1UZ]);
        }

        std::ranges::sort(x);
        std::ranges::sort(y);

        std::array test_center = {0U, 0U};

        if (size_ % 2U == 0U) {
            test_center[0UZ] = (x[size_ / 2UZ] + x[size_ / 2UZ - 1UZ]) / 2U;
            test_center[1UZ] = (y[size_ / 2UZ] + y[size_ / 2UZ - 1UZ]) / 2U;
        }
        else {
            test_center[0UZ] = x[size_ / 2UZ];
            test_center[1UZ] = y[size_ / 2UZ];
        }

        for (auto i = 0U; i < size_; ++i) {
            if (pixels[i][0UZ] == test_center[0UZ] && pixels[i][1UZ] == test_center[1UZ]) {
                center_[0UZ] = test_center[0UZ];
                center_[1UZ] = test_center[1UZ];
                break;
            }
        }

        const auto distance = [](const std::array<uint_fast32_t, 2UZ> &a, const std::array<uint_fast32_t, 2UZ> &b) {
            return sqrt(
                pow(static_cast<int_fast32_t>(a[0UZ]) - static_cast<int_fast32_t>(b[0UZ]), 2) + pow(
                    static_cast<int_fast32_t>(a[1UZ]) - static_cast<int_fast32_t>(b[1UZ]), 2));
        };

        if (center_[0UZ] == 0U && center_[1UZ] == 0U) {
            auto min_distance = (std::numeric_limits<double_t>::max)();
            for (auto i = 0U; i < size_; ++i) {
                if (const auto dist = distance(pixels[i], {test_center[0UZ], test_center[1UZ]}); dist < min_distance) {
                    min_distance = dist;
                    center_[0UZ] = pixels[i][0UZ];
                    center_[1UZ] = pixels[i][1UZ];
                }
            }
        }
    }

    auto province::process_distances() -> void {
        for (const auto neighbor : neighbors_ | std::views::keys) {
            neighbors_.at(neighbor).first = points_distance({
                {center_[0UZ], center_[1UZ]},
                {neighbor.get().center_[0UZ], neighbor.get().center_[1UZ]}
            });
        }
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

    auto province::owner() const -> std::optional<std::reference_wrapper<tag>> { return owner_; }

    auto province::add_pop(const pop &new_pop) -> void { pops_.emplace_back(std::make_unique<pop>(new_pop)); }

    auto province::remove_pop(pop &p) -> void {
        std::erase_if(pops_, [p](const std::unique_ptr<pop> &elem) { return elem.get() == &p; });
    }

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

    auto province::expand_bounds(const std::array<uint_fast32_t, 2UZ> coords) -> void {
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

    auto province::base_color() const -> uint_fast32_t { return base_color_; }

    auto province::color() const -> uint_fast32_t {
        switch (map_mode.load()) {
            case map_mode_t::provinces: return base_color_;
            case map_mode_t::owner: return owner_.has_value() ? owner_->get().color() : 0xFFFFFFFF;
            case map_mode_t::koppen: return static_cast<uint_fast32_t>(koppen_);
            case map_mode_t::elevation: return static_cast<uint_fast32_t>(elevation_);
            case map_mode_t::vegetation: return static_cast<uint_fast32_t>(vegetation_);
            case map_mode_t::soil: return static_cast<uint_fast32_t>(soil_);
            case map_mode_t::sea: return static_cast<uint_fast32_t>(sea_);
        }
        return 0U;
    }

    auto province::koppen() const -> koppen_t { return koppen_; }

    auto province::elevation() const -> elevation_t { return elevation_; }

    auto province::vegetation() const -> vegetation_t { return vegetation_; }

    auto province::soil() const -> soil_t { return soil_; }

    auto province::sea() const -> sea_t { return sea_; }

    auto province::distance(province &other) const -> double_t {
        if (neighbors_.contains(other)) { return neighbors_.at(other).first; }
        return points_distance({{center_[0UZ], center_[1UZ]}, {other.center_[0UZ], other.center_[1UZ]}});
    }

    auto province::tick(tick_t tick_type) -> void {
        if (sea_ != sea_t::none) { return; }
        std::for_each(std::execution::unseq, pops_.begin(), pops_.end(), [tick_type](const std::unique_ptr<pop> &elem) {
            elem->tick(tick_type);
        });
    }

    auto province::bounds() const -> const std::array<uint_fast32_t, 4UZ> & { return bounds_; }

    auto province::center() const -> const std::array<uint_fast32_t, 2UZ> & { return center_; }

    auto province::pops() const -> const std::vector<std::unique_ptr<pop>> & { return pops_; }

    auto province::pops() -> std::vector<std::unique_ptr<pop>> & { return pops_; }

    auto province::neighbors() const -> const std::map<std::reference_wrapper<province>, std::pair<double_t,
        uint_fast8_t>> & { return neighbors_; }

    auto province::river_neighbors() const -> const std::map<std::reference_wrapper<province>, uint_fast8_t> & {
        return river_neighbors_;
    }

    auto province::impassable_neighbors() const -> const std::set<std::reference_wrapper<province>> & {
        return impassable_neighbors_;
    }

    auto province::add_impassable_neighbor(province &neighbor) -> void { impassable_neighbors_.insert(neighbor); }

    auto province::size() const -> uint_fast32_t { return size_; }

    auto province::value() const -> uint_fast8_t { return value_; }

    auto province::set_value(const uint_fast8_t value) -> void { value_ = value; }

    auto operator==(const std::reference_wrapper<const province> &lhs,
        const std::reference_wrapper<const province> &rhs) -> bool { return &lhs.get() == &rhs.get(); }

    auto operator<=>(const std::reference_wrapper<const province> &lhs,
        const std::reference_wrapper<const province> &rhs) -> std::strong_ordering { return &lhs.get() <=> &rhs.get(); }
}

namespace std {
    auto hash<reference_wrapper<mechanics::province>>::operator()(
        const reference_wrapper<mechanics::province> &rt) const noexcept -> size_t {
        return hash<mechanics::province *>()(&rt.get());
    }
}
