#include "province.hpp"
#include <map>
#include <numeric>
#include <iostream>
#include <set>

province::province() : color_(0) {
}

province::province(const unsigned int color) : color_(color) {
}

unsigned int province::color() const {
    return color_;
}

void province::set_koppen(const std::vector<unsigned int> &koppen_colors) {
    std::map<koppen, int> koppen_count;
    for (const auto &color: koppen_colors) {
        if (const auto koppen_value = static_cast<koppen>(color); koppen_value != koppen::none) {
            koppen_count[koppen_value]++;
        }
    }
    if (koppen_count.empty()) {
        koppen_ = koppen::none;
    } else {
        const auto max_koppen = std::max_element(koppen_count.begin(), koppen_count.end(),
                                                 [](const auto &a, const auto &b) { return a.second < b.second; });
        koppen_ = max_koppen->first;
    }
}

unsigned int province::koppen_color() const {
    return static_cast<unsigned int>(koppen_);
}

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

    std::vector<int> elevation;
    elevation.reserve(elevation_colors.size());
    for (const auto &color: elevation_colors) {
        elevation.push_back(to_elevation(color));
    }

    const int roughness = roughness_func(elevation);
    const int average_elevation = static_cast<int>(mean(elevation));

    if (average_elevation > 400) {
        elevation_ = elevation::highlands;
    }
    if (roughness > 25) {
        if (average_elevation < 150) {
            elevation_ = elevation::hills;
        } else {
            elevation_ = elevation::mountains;
        }
    } else if (roughness > 10) {
        if (average_elevation < 70) {
            elevation_ = elevation::flatland;
        } else if (average_elevation < 150) {
            elevation_ = elevation::hills;
        } else if (average_elevation < 300) {
            elevation_ = elevation::plateau;
        } else {
            elevation_ = elevation::highlands;
        }
    } else {
        if (average_elevation < 70) {
            elevation_ = elevation::flatland;
        } else if (average_elevation < 200) {
            elevation_ = elevation::plateau;
        } else {
            elevation_ = elevation::highlands;
        }
    }
}

unsigned int province::elevation_color() const {
    return static_cast<unsigned int>(elevation_);
}

void province::set_vegetation(const std::vector<unsigned int> &vegetation_colors) {
    std::map<vegetation, int> vegetation_count;
    for (const auto &color: vegetation_colors) {
        const auto vegetation_value = static_cast<vegetation>(color);
        vegetation_count[vegetation_value]++;
    }
    if (vegetation_count.empty()) {
        vegetation_ = vegetation::none;
    } else {
        const auto max_vegetation = std::max_element(vegetation_count.begin(), vegetation_count.end(),
                                                     [](const auto &a, const auto &b) { return a.second < b.second; });
        vegetation_ = max_vegetation->first;
    }
}

unsigned int province::vegetation_color() const {
    return static_cast<unsigned int>(vegetation_);
}

void province::write(std::ofstream &file) const {
    file << "color: " << color_ << "\n";
    file << "koppen: " << static_cast<unsigned int>(koppen_) << "\n";
    file << "elevation: " << static_cast<unsigned int>(elevation_) << "\n";
    file << "vegetation: " << static_cast<unsigned int>(vegetation_) << "\n";
    file << "\n";
}
