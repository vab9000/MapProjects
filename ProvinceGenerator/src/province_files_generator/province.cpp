#include "province.hpp"
#include <map>
#include <numeric>
#include <iostream>
#include <set>
#include <random>

constexpr unsigned int to_integer_color(const unsigned char r, const unsigned char g, const unsigned char b) {
    return (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(b);
}

enum class province::koppen : unsigned int {
    none = 0, af = to_integer_color(0, 0, 255), am = to_integer_color(0, 120, 255),
    aw = to_integer_color(70, 170, 250), bwh = to_integer_color(255, 0, 0), bwk = to_integer_color(255, 150, 150),
    bsh = to_integer_color(245, 165, 0), bsk = to_integer_color(255, 220, 100), csa = to_integer_color(255, 255, 0),
    csb = to_integer_color(200, 200, 0), csc = to_integer_color(150, 150, 0), cwa = to_integer_color(150, 255, 150),
    cwb = to_integer_color(100, 200, 100), cwc = to_integer_color(50, 150, 50), cfa = to_integer_color(200, 255, 80),
    cfb = to_integer_color(100, 255, 80), cfc = to_integer_color(50, 200, 0), dsa = to_integer_color(255, 0, 255),
    dsb = to_integer_color(200, 0, 200), dsc = to_integer_color(150, 50, 150), dsd = to_integer_color(150, 100, 150),
    dwa = to_integer_color(170, 175, 255), dwb = to_integer_color(90, 120, 220), dwc = to_integer_color(78, 80, 180),
    dwd = to_integer_color(50, 0, 135), dfa = to_integer_color(0, 255, 255), dfb = to_integer_color(55, 200, 255),
    dfc = to_integer_color(0, 125, 125), dfd = to_integer_color(0, 70, 95), et = to_integer_color(178, 178, 178),
    ef = to_integer_color(102, 102, 102),
};

enum class province::elevation : unsigned int {
    none = 0, flatland = to_integer_color(0, 255, 0), hills = to_integer_color(30, 200, 200),
    plateau = to_integer_color(200, 200, 10), highlands = to_integer_color(200, 10, 10),
    mountains = to_integer_color(50, 20, 20),
};

enum class province::vegetation : unsigned int {
    none = 0, tropical_evergreen_broadleaf_forest = to_integer_color(28, 85, 16),
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
    tropical_savanna = to_integer_color(186, 255, 53), xerophytic_woods_scrub = to_integer_color(255, 186, 154),
    steppe = to_integer_color(255, 186, 53), desert = to_integer_color(247, 255, 202),
    graminoid_and_forb_tundra = to_integer_color(231, 231, 24),
    erect_dwarf_shrub_tundra = to_integer_color(121, 134, 73),
    low_and_high_shrub_tundra = to_integer_color(101, 255, 154),
    prostrate_dwarf_shrub_tundra = to_integer_color(210, 158, 150),
};

enum class province::soil : unsigned int {
    none = 0, acrisols = to_integer_color(247, 153, 29), albeluvisols = to_integer_color(155, 157, 87),
    alisols = to_integer_color(250, 247, 192), andosols = to_integer_color(237, 58, 51),
    arenosols = to_integer_color(247, 216, 172), calcisols = to_integer_color(255, 238, 0),
    cambisols = to_integer_color(254, 205, 103), chernozems = to_integer_color(226, 200, 55),
    cryosols = to_integer_color(117, 106, 146), durisols = to_integer_color(239, 230, 191),
    ferrasols = to_integer_color(246, 135, 45), fluvisols = to_integer_color(1, 176, 239),
    gleysols = to_integer_color(146, 145, 185), gypsisols = to_integer_color(251, 246, 165),
    histosols = to_integer_color(139, 137, 138), kastanozems = to_integer_color(201, 149, 128),
    leptosols = to_integer_color(213, 214, 216), lixisols = to_integer_color(249, 189, 191),
    luvisols = to_integer_color(244, 131, 133), nitisols = to_integer_color(247, 160, 130),
    phaeozems = to_integer_color(186, 104, 80), planosols = to_integer_color(245, 147, 84),
    plinthosols = to_integer_color(111, 14, 65), podzols = to_integer_color(13, 175, 99),
    regosols = to_integer_color(255, 226, 174), solonchaks = to_integer_color(237, 57, 148),
    solonetz = to_integer_color(244, 205, 226), stagnosols = to_integer_color(64, 193, 235),
    umbrisols = to_integer_color(97, 143, 130), vertisols = to_integer_color(158, 86, 124),
};

province::province() : color_(0), koppen_(koppen::none), elevation_(elevation::none), vegetation_(vegetation::none),
    soil_(soil::none), water_(false) {}

province::province(const unsigned int color, const bool water) : color_(color), koppen_(koppen::none),
    elevation_(elevation::none),
    vegetation_(vegetation::none), soil_(soil::none),
    water_(water) {}

unsigned int province::color() const { return color_; }

void province::set_koppen(const std::vector<unsigned int> &koppen_colors) {
    std::map<koppen, int> koppen_count;
    for (const auto &color : koppen_colors) {
        if (const auto koppen_value = static_cast<koppen>(color); koppen_value != koppen::none) {
            koppen_count[koppen_value]++;
        }
    }
    if (koppen_count.empty()) { koppen_ = koppen::none; }
    else {
        const auto max_koppen = std::max_element(koppen_count.begin(), koppen_count.end(),
            [](const auto &a, const auto &b) { return a.second < b.second; });
        koppen_ = max_koppen->first;
    }
}

unsigned int province::koppen_color() const { return static_cast<unsigned int>(koppen_); }

inline double mean(const std::vector<int> &vec) {
    if (vec.empty()) return 0.0;
    const double sum = std::accumulate(vec.begin(), vec.end(), 0.0);
    return sum / static_cast<double>(vec.size());
}

inline int roughness_func(const std::vector<int> &elevations) {
    std::multiset sorted_colors(elevations.begin(), elevations.end());
    if (sorted_colors.size() < 4) return 0.0;
    auto it = sorted_colors.begin();
    std::advance(it, static_cast<size_t>(static_cast<double>(sorted_colors.size()) * 0.25));
    const auto first_quarter = *it;
    std::advance(it, static_cast<size_t>(static_cast<double>(sorted_colors.size()) * 0.5));
    const auto third_quarter = *it;
    return third_quarter - first_quarter;
}

void province::set_elevation(const std::vector<unsigned int> &elevation_colors) {
    constexpr auto to_elevation = [](unsigned int color) -> int {
        const unsigned char *ptr = reinterpret_cast<unsigned char *>(&color);
        return ptr[1] + ptr[2] + ptr[3];
    };

    if (elevation_colors.empty()) {
        elevation_ = elevation::none;
        return;
    }

    std::vector<int> elevation;
    elevation.reserve(elevation_colors.size());
    for (const auto &color : elevation_colors) { elevation.push_back(to_elevation(color)); }

    const int roughness = roughness_func(elevation);
    roughness_ = roughness;
    const int average_elevation = static_cast<int>(mean(elevation));
    average_elevation_ = average_elevation;

    if (average_elevation > 400) { elevation_ = elevation::highlands; }
    if (roughness > 25) {
        if (average_elevation < 150) { elevation_ = elevation::hills; }
        else { elevation_ = elevation::mountains; }
    }
    else if (roughness > 10) {
        if (average_elevation < 70) { elevation_ = elevation::flatland; }
        else if (average_elevation < 150) { elevation_ = elevation::hills; }
        else if (average_elevation < 300) { elevation_ = elevation::plateau; }
        else { elevation_ = elevation::highlands; }
    }
    else {
        if (average_elevation < 70) { elevation_ = elevation::flatland; }
        else if (average_elevation < 200) { elevation_ = elevation::plateau; }
        else { elevation_ = elevation::highlands; }
    }
}

unsigned int province::elevation_color() const { return static_cast<unsigned int>(elevation_); }

void province::set_vegetation(const std::vector<unsigned int> &vegetation_colors) {
    std::map<vegetation, int> vegetation_count;
    for (const auto &color : vegetation_colors) {
        const auto vegetation_value = static_cast<vegetation>(color);
        vegetation_count[vegetation_value]++;
    }
    if (vegetation_count.empty()) { vegetation_ = vegetation::none; }
    else {
        const auto max_vegetation = std::max_element(vegetation_count.begin(), vegetation_count.end(),
            [](const auto &a, const auto &b) { return a.second < b.second; });
        vegetation_ = max_vegetation->first;
    }
}

unsigned int province::vegetation_color() const { return static_cast<unsigned int>(vegetation_); }

void province::set_soil(const std::vector<unsigned int> &soil_colors) {
    std::map<soil, int> soil_count;
    for (const auto &color : soil_colors) {
        const auto soil_value = static_cast<soil>(color);
        soil_count[soil_value]++;
    }
    if (soil_count.empty()) { soil_ = soil::none; }
    else {
        const auto max_soil = std::max_element(soil_count.begin(), soil_count.end(),
            [](const auto &a, const auto &b) { return a.second < b.second; });
        soil_ = max_soil->first;
    }
}

unsigned int province::soil_color() const { return static_cast<unsigned int>(soil_); }

void province::write(std::ofstream &file) const {
    if (water_) return;
    file << "color: " << color_ << "\n";
    file << "koppen: " << static_cast<unsigned int>(koppen_) << "\n";
    file << "elevation: " << static_cast<unsigned int>(elevation_) << "\n";
    file << "vegetation: " << static_cast<unsigned int>(vegetation_) << "\n";
    file << "soil: " << static_cast<unsigned int>(soil_) << "\n";
    file << "\n";
}

void province::add_neighbor(const province *neighbor) { neighbors_.insert(neighbor); }

const std::unordered_set<const province *> &province::neighbors() const { return neighbors_; }

void province::add_outline_point(const province *neighbor, int x, int y) { outline_[neighbor].insert({x, y}); }

const std::unordered_map<const province *, std::unordered_set<std::pair<int, int>, hash_coords>> &
province::outline() const { return outline_; }

void province::add_river(const province *neighbor, const int size) { rivers_[neighbor] = size; }

const std::unordered_map<const province *, int> &province::rivers() const { return rivers_; }

unsigned int province::river_color(int x, int y) const {
    for (const auto &[neighbor, outline_pixels] : outline_) {
        if (outline_pixels.contains({x, y})) { if (rivers_.contains(neighbor)) { return rivers_.at(neighbor); } }
    }
    return 0xFFFFFF;
}

bool province::is_water() const { return water_; }

void province::set_water(bool water) { water_ = water; }

bool color_is_water(const image::image_color &color) {
    if (color.r() == 0 && color.g() == 0 && color.b() != 0) { return true; }
    if (color.r() == 255 && color.g() == 0 && color.b() == 255) { return true; }
    if (color.r() == 0 && color.g() == 255 && color.b() == 255) { return true; }
    return false;
}

std::vector<const province *> province::impassable_neighbors(const image &base_image) const {
    std::vector<const province *> impassable;
    if (is_water()) { return impassable; }
    for (auto neighbor : neighbors_) {
        if (neighbor->is_water()) { continue; }
        const auto &border = outline_.at(neighbor);
        size_t num_points = border.size();
        const double border_elevation = std::accumulate(border.begin(), border.end(), 0,
                                            [&](int sum, const std::pair<int, int> &point) {
                                                const auto color = base_image(point.first, point.second);
                                                for (int i = -1; i <= 1; i++) {
                                                    for (int j = -1; j <= 1; j++) {
                                                        if (i == 0 && j == 0) { continue; }
                                                        if (point.first + i < 0 || point.first + i >= base_image.width()
                                                            ||
                                                            point.second + j < 0 || point.second + j >= base_image.
                                                            height()) { continue; }
                                                        const auto neighbor_color = base_image(
                                                            point.first + i, point.second + j);
                                                        if (!color_is_water(neighbor_color)) {
                                                            sum += neighbor_color.r() + neighbor_color.g() +
                                                                neighbor_color.b();
                                                            num_points++;
                                                        }
                                                    }
                                                }
                                                if (color_is_water(color)) { return sum + average_elevation_; }
                                                return sum + color.r() + color.g() + color.b();
                                            }) / static_cast<double>(num_points);
        size_t neighbor_num_points = neighbor->outline_.at(this).size();
        const double neighbor_border_elevation = std::accumulate(neighbor->outline_.at(this).begin(),
                                                     neighbor->outline_.at(this).end(), 0,
                                                     [&](int sum, const std::pair<int, int> &point) {
                                                         const auto color = base_image(
                                                             point.first, point.second);
                                                         for (int i = -1; i <= 1; i++) {
                                                             for (int j = -1; j <= 1; j++) {
                                                                 if (i == 0 && j == 0) { continue; }
                                                                 if (point.first + i < 0 || point.first + i >=
                                                                     base_image.width() ||
                                                                     point.second + j < 0 || point.second + j >=
                                                                     base_image.height()) { continue; }
                                                                 const auto neighbor_color = base_image(
                                                                     point.first + i, point.second + j);
                                                                 if (!color_is_water(neighbor_color)) {
                                                                     sum += neighbor_color.r() + neighbor_color.g() +
                                                                         neighbor_color.b();
                                                                     neighbor_num_points++;
                                                                 }
                                                             }
                                                         }
                                                         if (color_is_water(color)) { return sum + average_elevation_; }
                                                         return sum + color.r() + color.g() + color.b();
                                                     }) / static_cast<double>(neighbor_num_points);
        if (abs(border_elevation - neighbor_border_elevation) > average_elevation_ / 15.0 + 4 || abs(
                border_elevation - neighbor_border_elevation) > neighbor->average_elevation_ / 15.0 + 4) {
            impassable.push_back(neighbor);
            continue;
        }
        if (border_elevation > average_elevation_ * 1.2 + 25 || neighbor_border_elevation > neighbor->average_elevation_
            * 1.2 + 25) { impassable.push_back(neighbor); }
    }
    return impassable;
}
