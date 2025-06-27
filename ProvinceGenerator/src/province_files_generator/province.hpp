#pragma once

#include <array>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

struct hash_coords {
    std::size_t operator()(const std::pair<int, int> &p) const noexcept {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

class province {
    enum class koppen : unsigned int;
    enum class elevation : unsigned int;
    enum class vegetation : unsigned int;
    enum class soil : unsigned int;

    unsigned int color_;
    koppen koppen_;
    elevation elevation_;
    vegetation vegetation_;
    soil soil_;
    std::unordered_set<province *> neighbors_;
    std::unordered_map<province *, std::unordered_set<std::pair<int, int>, hash_coords>> outline_;
    std::unordered_map<province *, int> rivers_;

public:
    province();

    explicit province(unsigned int color);

    [[nodiscard]] std::array<int, 2> dimensions() const;

    [[nodiscard]] unsigned int color() const;

    void set_koppen(const std::vector<unsigned int> &koppen_colors);

    [[nodiscard]] unsigned int koppen_color() const;

    void set_elevation(const std::vector<unsigned int> &elevation_colors);

    [[nodiscard]] unsigned int elevation_color() const;

    void set_vegetation(const std::vector<unsigned int> &vegetation_colors);

    [[nodiscard]] unsigned int vegetation_color() const;

    void set_soil(const std::vector<unsigned int> &soil_colors);

    [[nodiscard]] unsigned int soil_color() const;

    void write(std::ofstream &file) const;

    void add_neighbor(province *neighbor);

    [[nodiscard]] const std::unordered_set<province *> &neighbors() const;

    void add_outline_point(province * neighbor, int x, int y);

    [[nodiscard]] const std::unordered_map<province *, std::unordered_set<std::pair<int, int>, hash_coords>> &outline() const;

    void add_river(province *neighbor, int size);

    [[nodiscard]] const std::unordered_map<province *, int> &rivers() const;

    [[nodiscard]] unsigned int river_color(int x, int y) const;
};
