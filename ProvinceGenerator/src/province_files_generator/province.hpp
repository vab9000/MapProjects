#pragma once

#include <array>
#include <vector>
#include <fstream>

constexpr unsigned int to_integer_color(const unsigned char r, const unsigned char g, const unsigned char b) {
    return (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(b);
}

class province {
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

    unsigned int color_;
    koppen koppen_ = koppen::none;
    elevation elevation_ = elevation::none;
    vegetation vegetation_ = vegetation::none;

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

    void write(std::ofstream &file) const;
};
