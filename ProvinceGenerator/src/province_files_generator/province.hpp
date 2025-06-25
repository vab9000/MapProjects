#pragma once

#include <array>
#include <vector>
#include <fstream>



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
};
