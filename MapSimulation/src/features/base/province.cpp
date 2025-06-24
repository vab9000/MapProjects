#include "province.hpp"
#include <algorithm>
#include <array>
#include <queue>
#include <ranges>
#include <string>
#include <unordered_map>
#include <cmath>
#include <vector>
#include "../tags/tag.hpp"
#include "utils.hpp"

constexpr unsigned int flip_rb(const unsigned int color) {
    return (color & 0x00FF00) | ((color & 0xFF0000) >> 16) | ((color & 0x0000FF) << 16);
}

constexpr unsigned int to_integer_color(const unsigned char r, const unsigned char g, const unsigned char b) {
    return flip_rb((static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(b));
}

enum class koppen : unsigned int {
    none = 0,
    af = to_integer_color(0, 0, 255),
    am = to_integer_color(0, 120, 255),
    aw = to_integer_color(70, 170, 250),
    bwh = to_integer_color(255, 0, 0),
    bwk = to_integer_color(255, 150, 150),
    bsh = to_integer_color(245, 165, 0),
    bsk = to_integer_color(255, 220, 100),
    csa = to_integer_color(255, 255, 0),
    csb = to_integer_color(200, 200, 0),
    csc = to_integer_color(150, 150, 0),
    cwa = to_integer_color(150, 255, 150),
    cwb = to_integer_color(100, 200, 100),
    cwc = to_integer_color(50, 150, 50),
    cfa = to_integer_color(200, 255, 80),
    cfb = to_integer_color(100, 255, 80),
    cfc = to_integer_color(50, 200, 0),
    dsa = to_integer_color(255, 0, 255),
    dsb = to_integer_color(200, 0, 200),
    dsc = to_integer_color(150, 50, 150),
    dsd = to_integer_color(150, 100, 150),
    dwa = to_integer_color(170, 175, 255),
    dwb = to_integer_color(90, 120, 220),
    dwc = to_integer_color(78, 80, 180),
    dwd = to_integer_color(50, 0, 135),
    dfa = to_integer_color(0, 255, 255),
    dfb = to_integer_color(55, 200, 255),
    dfc = to_integer_color(0, 125, 125),
    dfd = to_integer_color(0, 70, 95),
    et = to_integer_color(178, 178, 178),
    ef = to_integer_color(102, 102, 102),
};

enum class elevation : unsigned int {
    none = 0,
    flatland = to_integer_color(0, 255, 0),
    hills = to_integer_color(30, 200, 200),
    plateau = to_integer_color(200, 200, 10),
    highlands = to_integer_color(200, 10, 10),
    mountains = to_integer_color(50, 20, 20),
};

enum class vegetation : unsigned int {
    none = 0,
    tropical_evergreen_broadleaf_forest = to_integer_color(28, 85, 16),
    tropical_semi_evergreen_broadleaf_forest = to_integer_color(101, 146, 8),
    tropical_deciduous_broadleaf_forest_and_woodland = to_integer_color(174, 125, 32),
    warm_temperate_evergreen_and_mixed_forest = to_integer_color(0, 0, 101),
    cool_temperate_rainforest = to_integer_color(187, 203, 53),
    cool_evergreen_needleleaf_forest = to_integer_color(0, 154, 24),
    cool_mixed_forest = to_integer_color(202, 255, 202),
    temperate_deciduous_broadleaf_forest = to_integer_color(85, 235, 73),
    cold_deciduous_forest = to_integer_color(101, 178, 255),
    cold_evergreen_needleleaf_forest = to_integer_color(0, 32, 202),
    temperate_sclerophyll_woodland_and_shrubland = to_integer_color(142, 162, 40),
    temperate_evergreen_needleleaf_open_woodland = to_integer_color(255, 154, 223),
    tropical_savanna = to_integer_color(186, 255, 53),
    xerophytic_woods_scrub = to_integer_color(255, 186, 154),
    steppe = to_integer_color(255, 186, 53),
    desert = to_integer_color(247, 255, 202),
    graminoid_and_forb_tundra = to_integer_color(231, 231, 24),
    erect_dwarf_shrub_tundra = to_integer_color(121, 134, 73),
    low_and_high_shrub_tundra = to_integer_color(101, 255, 154),
    prostrate_dwarf_shrub_tundra = to_integer_color(210, 158, 150),
};

province::province() : owner_(nullptr), num_pixels_(0), num_outline_(0), bounds_({-1, -1, 0, 0}), center_({-1, -1}),
                       base_color_(0), koppen_(koppen::none), elevation_(elevation::none),
                       vegetation_(vegetation::none), color(0) {
}

province::province(std::string name, const unsigned int color, const koppen koppen, const elevation elevation,
                   const vegetation vegetation) : owner_(nullptr), num_pixels_(0), num_outline_(0),
                                                  bounds_({-1, -1, 0, 0}), center_{-1, -1}, base_color_(color),
                                                  name_(std::move(name)), koppen_(koppen), elevation_(elevation),
                                                  vegetation_(vegetation), color(color) {
}

void province::finalize() {
    const auto total_pixels = num_pixels_ + num_outline_;

    auto x = std::vector<int>(total_pixels);
    auto y = std::vector<int>(total_pixels);

    std::ranges::sort(x);
    std::ranges::sort(y);

    int test_center[2] = {0, 0};

    if (total_pixels % 2 == 0) {
        test_center[0] = (x[total_pixels / 2] + x[total_pixels / 2 - 1]) / 2;
        test_center[1] = (y[total_pixels / 2] + y[total_pixels / 2 - 1]) / 2;
    } else {
        test_center[0] = x[total_pixels / 2];
        test_center[1] = y[total_pixels / 2];
    }

    for (int i = 0; i < num_pixels_; ++i) {
        if (pixels_[i][0] == test_center[0] && pixels_[i][1] == test_center[1]) {
            center_[0] = test_center[0];
            center_[1] = test_center[1];
            break;
        }
    }

    for (int i = 0; i < num_outline_; ++i) {
        if (outline_[i].second[0] == test_center[0] && outline_[i].second[1] == test_center[1]) {
            center_[0] = test_center[0];
            center_[1] = test_center[1];
            break;
        }
    }

    auto distance = [](const std::array<int, 2> &a, const std::array<int, 2> &b) {
        return sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]));
    };

    if (center_[0] == -1) {
        double min_distance = (std::numeric_limits<double>::max)();
        for (int i = 0; i < num_pixels_; ++i) {
            if (const auto dist = distance(pixels_[i], {test_center[0], test_center[1]}); dist < min_distance) {
                min_distance = dist;
                center_[0] = pixels_[i][0];
                center_[1] = pixels_[i][1];
            }
        }
        for (int i = 0; i < num_outline_; ++i) {
            if (const auto dist = distance(outline_[i].second, {test_center[0], test_center[1]}); dist < min_distance) {
                min_distance = dist;
                center_[0] = outline_[i].second[0];
                center_[1] = outline_[i].second[1];
            }
        }
    }
}

void province::process_distances() {
    for (const auto neighbor: neighbors_ | std::views::keys) {
        neighbors_[neighbor] = this->distance(*neighbor);
    }
    distances_processed_ = true;
}

std::string_view province::name() {
    return name_;
}

void province::set_owner(tag &new_owner) {
    if (has_owner() && owner_->has_province(*this)) {
        owner_->remove_province(*this);
    }
    owner_ = &new_owner;
}

void province::remove_owner() {
    if (has_owner()) {
        owner_->remove_province(*this);
    }
    owner_ = nullptr;
}

[[nodiscard]] tag &province::get_owner() const {
    return *owner_;
}

bool province::has_owner() const {
    return owner_;
}

void province::add_pixel(const int x, const int y) {
    pixels_.emplace_back(std::array{x, y});
    num_pixels_ += 1;
}

void province::add_outline(const int x, const int y, province &other) {
    outline_.emplace_back(&other, std::array{x, y});
    num_outline_ += 1;
    neighbors_.emplace(&other, 0.0);
}

[[nodiscard]] const std::vector<std::array<int, 2> > &province::get_pixels() const {
    return pixels_;
}

[[nodiscard]] const std::vector<std::pair<province *, std::array<int, 2> > > &
province::get_outline() const {
    return outline_;
}

void province::expand_bounds(const int x, const int y) {
    if (bounds_[0] == -1) {
        bounds_[0] = x;
        bounds_[1] = y;
        bounds_[2] = x;
        bounds_[3] = y;
    }
    if (x < bounds_[0]) {
        bounds_[0] = x;
    } else if (x > bounds_[2]) {
        bounds_[2] = x;
    }
    if (y < bounds_[1]) {
        bounds_[1] = y;
    } else if (y > bounds_[3]) {
        bounds_[3] = y;
    }
}

void province::recolor(const map_modes mode) {
    if (mode == map_modes::provinces) {
        color = base_color_;
    } else if (mode == map_modes::owner) {
        if (has_owner()) {
            color = owner_->get_color();
        } else {
            color = 0xFFFFFFFF;
        }
    }
    switch (mode) {
        case map_modes::provinces:
            color = base_color_;
            break;
        case map_modes::owner:
            if (has_owner()) {
                color = owner_->get_color();
            } else {
                color = 0xFFFFFFFF;
            }
            break;
        case map_modes::koppen:
            color = static_cast<unsigned int>(koppen_);
            break;
        case map_modes::elevation:
            color = static_cast<unsigned int>(elevation_);
            break;
        case map_modes::vegetation:
            color = static_cast<unsigned int>(vegetation_);
            break;
        default:
            color = 0xFFFFFFFF; // Fallback color
            break;
    }
}

[[nodiscard]] unsigned int province::get_base_color() const {
    return base_color_;
}

[[nodiscard]] double province::distance(const province &other) const {
    if (distances_processed_ && neighbors_.contains(&const_cast<province &>(other))) {
        return neighbors_.at(&const_cast<province &>(other));
    }
    return sqrt((center_[0] - other.center_[0]) * (center_[0] - other.center_[0]) +
                (center_[1] - other.center_[1]) * (center_[1] - other.center_[1]));
}

[[nodiscard]] std::vector<province *> province::get_path_to(const province &destination,
                                                            bool (*accessible)(const province &, void *),
                                                            double (*cost_modifier)(const province &, void *),
                                                            void *param) {
    std::unordered_map<province *, double> distances;
    std::unordered_map<province *, province *> previous;
    std::priority_queue<std::pair<double, province *>, std::vector<std::pair<double,
                province *> >, std::greater<> >
            queue;

    distances[this] = 0;
    queue.emplace(0, this);

    auto reached = false;

    while (!queue.empty()) {
        auto [currentDistance, currentProvince] = queue.top();
        queue.pop();

        if (currentProvince == &const_cast<province &>(destination)) {
            reached = true;
            break;
        }

        for (const auto &[neighborProvince, neighborDistance]: currentProvince->get_neighbors()) {
            if (!accessible(*neighborProvince, param) ||
                cost_modifier(*neighborProvince, param) < 0) {
                continue;
            }

            if (double new_distance = currentDistance + cost_modifier(*neighborProvince, param) * neighborDistance;
                !distances.contains(neighborProvince) || new_distance < distances[neighborProvince]) {
                distances[neighborProvince] = new_distance;
                previous[neighborProvince] = currentProvince;
                queue.emplace(new_distance, neighborProvince);
            }
        }
    }

    if (!reached) {
        return {};
    }

    std::vector<province *> path;
    for (province *at = &const_cast<province &>(destination); at != nullptr; at = previous[at]) {
        path.insert(path.begin(), at);
    }

    return path;
}

void province::tick() {
    if (has_owner()) {
        owner_->add_gold(1);
    }
}

[[nodiscard]] unsigned int province::get_num_pixels() const {
    return num_pixels_;
}

[[nodiscard]] unsigned int province::get_num_outline() const {
    return
            num_outline_;
}

[[nodiscard]] const std::array<int, 4> &province::get_bounds() const {
    return bounds_;
}

[[nodiscard]] const std::array<int, 2> &province::get_center() const {
    return center_;
}

[[nodiscard]] const std::vector<std::unique_ptr<pop> > &province::get_pops() const {
    return pops_;
}

[[nodiscard]] const std::unordered_map<province *, double> &province::get_neighbors() const {
    return neighbors_;
}
