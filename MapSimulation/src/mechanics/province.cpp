#include "province.hpp"
#include <algorithm>
#include <execution>
#include <ranges>
#include "tag.hpp"

namespace {
    auto points_distance(
        const std::pair<std::pair<int, int>, std::pair<int, int>> points) -> double {
        return sqrt(pow(points.first.first - points.second.first, 2) +
                    pow(points.first.second - points.second.second, 2));
    }
}

namespace mechanics {
    province::province() : koppen_(koppen_t::none), elevation_(elevation_t::none), vegetation_(vegetation_t::none),
        soil_(soil_t::none),
        sea_(sea_t::none), base_color_(0U), id_(0U) {}

    province::province(const size_t id, const koppen_t koppen,
        const elevation_t elevation, const vegetation_t vegetation, const soil_t soil, const sea_t sea,
        const unsigned int color) : koppen_(koppen), elevation_(elevation), vegetation_(vegetation),
        soil_(soil), sea_(sea), base_color_(color), id_(id) {}

    auto province::id() const -> size_t { return id_; }

    auto province::finalize(const std::vector<std::array<unsigned int, 2UZ>> &pixels) -> void {
        std::erase_if(impassable_neighbors_, [&](const utils::ref<province> &p) {
            return !this->neighbors_.contains(p);
        });

        if (sea_ != sea_t::none) {
            std::unordered_map<koppen_t, int> koppen_types;
            for (const auto &neighbor : neighbors_ | std::views::keys) {
                if (neighbor.get().sea_ == sea_t::none) {
                    if (koppen_types.contains(neighbor.get().koppen_)) { koppen_types.at(neighbor.get().koppen_) += 1; }
                    else { koppen_types.emplace(neighbor.get().koppen_, 1); }
                }
            }
            if (!koppen_types.empty()) {
                koppen_ = std::ranges::max_element(koppen_types.begin(), koppen_types.end(),
                    [](const auto &a, const auto &b) { return a.second < b.second; })->first;
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
            auto min_distance = (std::numeric_limits<double>::max)();
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
            neighbors_.at(neighbor).first = points_distance({
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
            neighbors_.at(neighbor).second += 1;
            return;
        }

        neighbors_.emplace(neighbor, std::pair{0.0, 1});
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

    auto province::base_color() const -> unsigned int { return base_color_; }

    auto province::color() const -> unsigned int {
        switch (map_mode.load()) {
            case map_mode_t::provinces: return base_color_;
            case map_mode_t::owner: return owner_ != nullptr ? owner_->color() : 0xFFFFFFFF;
            case map_mode_t::koppen: {
                if (koppen_ == koppen_t::none) { return 0xFFFFFFFF; }
                return static_cast<unsigned int>(koppen_);
            }
            case map_mode_t::elevation: {
                if (elevation_ == elevation_t::none) { return 0xFFFFFFFF; }
                return static_cast<unsigned int>(elevation_);
            }
            case map_mode_t::vegetation: {
                if (vegetation_ == vegetation_t::none) { return 0xFFFFFFFF; }
                return static_cast<unsigned int>(vegetation_);
            }
            case map_mode_t::soil: {
                if (soil_ == soil_t::none) { return 0xFFFFFFFF; }
                return static_cast<unsigned int>(soil_);
            }
            case map_mode_t::sea: {
                if (sea_ == sea_t::none) { return 0xFFFFFFFF; }
                return static_cast<unsigned int>(sea_);
            }
            case map_mode_t::river_size: return (value_ == 0U) ? 0xFFFFFFFF
                    : 0x000000FFU | static_cast<unsigned int>(value_) << 8;
        }
        return 0U;
    }

    auto province::koppen() const -> koppen_t { return koppen_; }

    auto province::elevation() const -> elevation_t { return elevation_; }

    auto province::vegetation() const -> vegetation_t { return vegetation_; }

    auto province::soil() const -> soil_t { return soil_; }

    auto province::sea() const -> sea_t { return sea_; }

    auto province::distance(province &other) const -> double {
        if (neighbors_.contains(other)) { return neighbors_.at(other).first; }
        return points_distance({{center_[0UZ], center_[1UZ]}, {other.center_[0UZ], other.center_[1UZ]}});
    }

    auto province::tick(tick_t tick_type) -> void {
        if (sea_ != sea_t::none) { return; }
        std::for_each(std::execution::seq, pops_.begin(), pops_.end(), [tick_type](const std::unique_ptr<pop> &elem) {
            elem->tick(tick_type);
        });
        if (settlement_.has_value()) { settlement_->tick(tick_type); }
    }

    auto province::bounds() const -> const std::array<unsigned int, 4UZ> & { return bounds_; }

    auto province::center() const -> const std::array<unsigned int, 2UZ> & { return center_; }

    auto province::pops() const -> const std::vector<std::unique_ptr<pop>> & { return pops_; }

    auto province::pops() -> std::vector<std::unique_ptr<pop>> & { return pops_; }

    auto province::neighbors() const -> const std::map<utils::ref<province>, std::pair<double,
        unsigned char>> & { return neighbors_; }

    auto province::river_neighbors() const -> const std::map<utils::ref<province>, unsigned char> & {
        return river_neighbors_;
    }

    auto province::impassable_neighbors() const -> const std::set<utils::ref<province>> & {
        return impassable_neighbors_;
    }

    auto province::add_impassable_neighbor(province &neighbor) -> void {
        impassable_neighbors_.insert(neighbor);
        neighbor.impassable_neighbors_.insert(*this);
    }

    auto province::size() const -> unsigned int { return size_; }

    auto province::settlement() const -> const std::optional<mechanics::settlement> & {
        return settlement_;
    }

    auto province::value() const -> unsigned char { return value_; }

    auto province::set_value(const unsigned char value) -> void { value_ = value; }
}
