#include "load_image.hpp"
#include <algorithm>
#include <colors.hpp>
#include <execution>
#include <fstream>
#include <iostream>
#include <ranges>
#include "image.hpp"
#include "../mechanics/data.hpp"
#include "../ui/map_view.hpp"

namespace {
    enum class koppen_value_t : unsigned int {
        none = 0U, af = utils::to_integer_color(0U, 0U, 255U), am = utils::to_integer_color(0U, 120U, 255U),
        aw = utils::to_integer_color(70U, 170U, 250U), bwh = utils::to_integer_color(255U, 0U, 0U),
        bwk = utils::to_integer_color(255U, 150U, 150U), bsh = utils::to_integer_color(245U, 165U, 0U),
        bsk = utils::to_integer_color(255U, 220U, 100U), csa = utils::to_integer_color(255U, 255U, 0U),
        csb = utils::to_integer_color(200U, 200U, 0U), csc = utils::to_integer_color(150U, 150U, 0U),
        cwa = utils::to_integer_color(150U, 255U, 150U), cwb = utils::to_integer_color(100U, 200U, 100U),
        cwc = utils::to_integer_color(50U, 150U, 50U), cfa = utils::to_integer_color(200U, 255U, 80U),
        cfb = utils::to_integer_color(100U, 255U, 80U), cfc = utils::to_integer_color(50U, 200U, 0U),
        dsa = utils::to_integer_color(255U, 0U, 255U), dsb = utils::to_integer_color(200U, 0U, 200U),
        dsc = utils::to_integer_color(150U, 50U, 150U), dsd = utils::to_integer_color(150U, 100U, 150U),
        dwa = utils::to_integer_color(170U, 175U, 255U), dwb = utils::to_integer_color(90U, 120U, 220U),
        dwc = utils::to_integer_color(78U, 80U, 180U), dwd = utils::to_integer_color(50U, 0U, 135U),
        dfa = utils::to_integer_color(0U, 255U, 255U), dfb = utils::to_integer_color(55U, 200U, 255U),
        dfc = utils::to_integer_color(0U, 125U, 125U), dfd = utils::to_integer_color(0U, 70U, 95U),
        et = utils::to_integer_color(178U, 178U, 178U), ef = utils::to_integer_color(102U, 102U, 102U),
    };

    enum class vegetation_value_t : unsigned int {
        none = 0U, tropical_evergreen_broadleaf_forest = utils::to_integer_color(28U, 85U, 16U),
        tropical_semi_evergreen_broadleaf_forest = utils::to_integer_color(101U, 146U, 8U),
        tropical_deciduous_broadleaf_forest_and_woodland = utils::to_integer_color(174U, 125U, 32U),
        warm_temperate_evergreen_and_mixed_forest = utils::to_integer_color(0U, 0U, 101U),
        cool_temperate_rainforest = utils::to_integer_color(187U, 203U, 53U),
        cool_evergreen_needleleaf_forest = utils::to_integer_color(0U, 154U, 24U),
        cool_mixed_forest = utils::to_integer_color(202U, 255U, 202U),
        temperate_deciduous_broadleaf_forest = utils::to_integer_color(85U, 235U, 73U),
        cold_deciduous_forest = utils::to_integer_color(101U, 178U, 255U),
        cold_evergreen_needleleaf_forest = utils::to_integer_color(0U, 32U, 202U),
        temperate_sclerophyll_woodland_and_shrubland = utils::to_integer_color(142U, 162U, 40U),
        temperate_evergreen_needleleaf_open_woodland = utils::to_integer_color(255U, 154U, 223U),
        tropical_savanna = utils::to_integer_color(186U, 255U, 53U),
        xerophytic_woods_scrub = utils::to_integer_color(255U, 186U, 154U),
        steppe = utils::to_integer_color(255U, 186U, 53U), desert = utils::to_integer_color(247U, 255U, 202U),
        graminoid_and_forb_tundra = utils::to_integer_color(231U, 231U, 24U),
        erect_dwarf_shrub_tundra = utils::to_integer_color(121U, 134U, 73U),
        low_and_high_shrub_tundra = utils::to_integer_color(101U, 255U, 154U),
        prostrate_dwarf_shrub_tundra = utils::to_integer_color(210U, 158U, 150U),
    };

    enum class soil_value_t : unsigned int {
        none = 0U, acrisols = utils::to_integer_color(247U, 153U, 29U),
        albeluvisols = utils::to_integer_color(155U, 157U, 87U), alisols = utils::to_integer_color(250U, 247U, 192U),
        andosols = utils::to_integer_color(237U, 58U, 51U), arenosols = utils::to_integer_color(247U, 216U, 172U),
        calcisols = utils::to_integer_color(255U, 238U, 0U), cambisols = utils::to_integer_color(254U, 205U, 103U),
        chernozems = utils::to_integer_color(226U, 200U, 55U), cryosols = utils::to_integer_color(117U, 106U, 146U),
        durisols = utils::to_integer_color(239U, 230U, 191U), ferrasols = utils::to_integer_color(246U, 135U, 45U),
        fluvisols = utils::to_integer_color(1U, 176U, 239U), gleysols = utils::to_integer_color(146U, 145U, 185U),
        gypsisols = utils::to_integer_color(251U, 246U, 165U), histosols = utils::to_integer_color(139U, 137U, 138U),
        kastanozems = utils::to_integer_color(201U, 149U, 128U), leptosols = utils::to_integer_color(213U, 214U, 216U),
        lixisols = utils::to_integer_color(249U, 189U, 191U), luvisols = utils::to_integer_color(244U, 131U, 133U),
        nitisols = utils::to_integer_color(247U, 160U, 130U), phaeozems = utils::to_integer_color(186U, 104U, 80U),
        planosols = utils::to_integer_color(245U, 147U, 84U), plinthosols = utils::to_integer_color(111U, 14U, 65U),
        podzols = utils::to_integer_color(13U, 175U, 99U), regosols = utils::to_integer_color(255U, 226U, 174U),
        solonchaks = utils::to_integer_color(237U, 57U, 148U), solonetz = utils::to_integer_color(244U, 205U, 226U),
        stagnosols = utils::to_integer_color(64U, 193U, 235U), umbrisols = utils::to_integer_color(97U, 143U, 130U),
        vertisols = utils::to_integer_color(158U, 86U, 124U),
    };

    enum class elevation_t : unsigned int {
        none = 0U, flat_lowland = utils::to_integer_color(255U, 255U, 50U),
        hill_lowland = utils::to_integer_color(125U, 255U, 50U),
        mountain_lowland = utils::to_integer_color(0U, 255U, 50U),
        flat_midland = utils::to_integer_color(255U, 125U, 50U),
        hill_midland = utils::to_integer_color(125U, 125U, 50U),
        mountain_midland = utils::to_integer_color(0U, 125U, 50U),
        flat_highland = utils::to_integer_color(255U, 0U, 50U), hill_highland = utils::to_integer_color(125U, 0U, 50U),
        mountain_highland = utils::to_integer_color(0U, 0U, 50U),
    };

    enum class sea_value_t :unsigned int {
        none = 0U, coast = utils::to_integer_color(25U, 255U, 255U), sea = utils::to_integer_color(100U, 200U, 255U),
        southeasterly = utils::to_integer_color(255U, 100U, 100U),
        northeasterly = utils::to_integer_color(100U, 100U, 255U), westerly = utils::to_integer_color(255U, 255U, 100U),
        polar = utils::to_integer_color(200U, 255U, 255U), lake = utils::to_integer_color(255U, 150U, 255U),
        river = utils::to_integer_color(0U, 255U, 0U), doldrums = utils::to_integer_color(100U, 255U, 150U)
    };

    auto koppen_value_to_koppen(const koppen_value_t value) -> mechanics::koppen_t {
        switch (value) {
            case koppen_value_t::none: return mechanics::koppen_t::none;
            case koppen_value_t::af: return mechanics::koppen_t::af;
            case koppen_value_t::am: return mechanics::koppen_t::am;
            case koppen_value_t::aw: return mechanics::koppen_t::aw;
            case koppen_value_t::bwh: return mechanics::koppen_t::bwh;
            case koppen_value_t::bwk: return mechanics::koppen_t::bwk;
            case koppen_value_t::bsh: return mechanics::koppen_t::bsh;
            case koppen_value_t::bsk: return mechanics::koppen_t::bsk;
            case koppen_value_t::csa: return mechanics::koppen_t::csa;
            case koppen_value_t::csb: return mechanics::koppen_t::csb;
            case koppen_value_t::csc: return mechanics::koppen_t::csc;
            case koppen_value_t::cwa: return mechanics::koppen_t::cwa;
            case koppen_value_t::cwb: return mechanics::koppen_t::cwb;
            case koppen_value_t::cwc: return mechanics::koppen_t::cwc;
            case koppen_value_t::cfa: return mechanics::koppen_t::cfa;
            case koppen_value_t::cfb: return mechanics::koppen_t::cfb;
            case koppen_value_t::cfc: return mechanics::koppen_t::cfc;
            case koppen_value_t::dsa: return mechanics::koppen_t::dsa;
            case koppen_value_t::dsb: return mechanics::koppen_t::dsb;
            case koppen_value_t::dsc: return mechanics::koppen_t::dsc;
            case koppen_value_t::dsd: return mechanics::koppen_t::dsd;
            case koppen_value_t::dwa: return mechanics::koppen_t::dwa;
            case koppen_value_t::dwb: return mechanics::koppen_t::dwb;
            case koppen_value_t::dwc: return mechanics::koppen_t::dwc;
            case koppen_value_t::dwd: return mechanics::koppen_t::dwd;
            case koppen_value_t::dfa: return mechanics::koppen_t::dfa;
            case koppen_value_t::dfb: return mechanics::koppen_t::dfb;
            case koppen_value_t::dfc: return mechanics::koppen_t::dfc;
            case koppen_value_t::dfd: return mechanics::koppen_t::dfd;
            case koppen_value_t::et: return mechanics::koppen_t::et;
            case koppen_value_t::ef: return mechanics::koppen_t::ef;
        }
        throw std::runtime_error("Invalid koppen_t value");
    }

    auto vegetation_value_to_vegetation(const vegetation_value_t value) -> mechanics::vegetation_t {
        switch (value) {
            case vegetation_value_t::none: return mechanics::vegetation_t::none;
            case vegetation_value_t::tropical_evergreen_broadleaf_forest: return
                    mechanics::vegetation_t::tropical_evergreen_broadleaf_forest;
            case vegetation_value_t::tropical_semi_evergreen_broadleaf_forest: return
                    mechanics::vegetation_t::tropical_semi_evergreen_broadleaf_forest;
            case vegetation_value_t::tropical_deciduous_broadleaf_forest_and_woodland: return
                    mechanics::vegetation_t::tropical_deciduous_broadleaf_forest_and_woodland;
            case vegetation_value_t::warm_temperate_evergreen_and_mixed_forest: return
                    mechanics::vegetation_t::warm_temperate_evergreen_and_mixed_forest;
            case vegetation_value_t::cool_temperate_rainforest: return
                    mechanics::vegetation_t::cool_temperate_rainforest;
            case vegetation_value_t::cool_evergreen_needleleaf_forest: return
                    mechanics::vegetation_t::cool_evergreen_needleleaf_forest;
            case vegetation_value_t::cool_mixed_forest: return mechanics::vegetation_t::cool_mixed_forest;
            case vegetation_value_t::temperate_deciduous_broadleaf_forest: return
                    mechanics::vegetation_t::temperate_deciduous_broadleaf_forest;
            case vegetation_value_t::cold_deciduous_forest: return mechanics::vegetation_t::cold_deciduous_forest;
            case vegetation_value_t::cold_evergreen_needleleaf_forest: return
                    mechanics::vegetation_t::cold_evergreen_needleleaf_forest;
            case vegetation_value_t::temperate_sclerophyll_woodland_and_shrubland: return
                    mechanics::vegetation_t::temperate_sclerophyll_woodland_and_shrubland;
            case vegetation_value_t::temperate_evergreen_needleleaf_open_woodland: return
                    mechanics::vegetation_t::temperate_evergreen_needleleaf_open_woodland;
            case vegetation_value_t::tropical_savanna: return mechanics::vegetation_t::tropical_savanna;
            case vegetation_value_t::xerophytic_woods_scrub: return mechanics::vegetation_t::xerophytic_woods_scrub;
            case vegetation_value_t::steppe: return mechanics::vegetation_t::steppe;
            case vegetation_value_t::desert: return mechanics::vegetation_t::desert;
            case vegetation_value_t::graminoid_and_forb_tundra: return
                    mechanics::vegetation_t::graminoid_and_forb_tundra;
            case vegetation_value_t::erect_dwarf_shrub_tundra: return mechanics::vegetation_t::erect_dwarf_shrub_tundra;
            case vegetation_value_t::low_and_high_shrub_tundra: return
                    mechanics::vegetation_t::low_and_high_shrub_tundra;
            case vegetation_value_t::prostrate_dwarf_shrub_tundra: return
                    mechanics::vegetation_t::prostrate_dwarf_shrub_tundra;
        }
        throw std::runtime_error("Invalid vegetation_t value");
    }

    auto soil_value_to_soil(const soil_value_t value) -> mechanics::soil_t {
        switch (value) {
            case soil_value_t::none: return mechanics::soil_t::none;
            case soil_value_t::acrisols: return mechanics::soil_t::acrisols;
            case soil_value_t::albeluvisols: return mechanics::soil_t::albeluvisols;
            case soil_value_t::alisols: return mechanics::soil_t::alisols;
            case soil_value_t::andosols: return mechanics::soil_t::andosols;
            case soil_value_t::arenosols: return mechanics::soil_t::arenosols;
            case soil_value_t::calcisols: return mechanics::soil_t::calcisols;
            case soil_value_t::cambisols: return mechanics::soil_t::cambisols;
            case soil_value_t::chernozems: return mechanics::soil_t::chernozems;
            case soil_value_t::cryosols: return mechanics::soil_t::cryosols;
            case soil_value_t::durisols: return mechanics::soil_t::durisols;
            case soil_value_t::ferrasols: return mechanics::soil_t::ferrasols;
            case soil_value_t::fluvisols: return mechanics::soil_t::fluvisols;
            case soil_value_t::gleysols: return mechanics::soil_t::gleysols;
            case soil_value_t::gypsisols: return mechanics::soil_t::gypsisols;
            case soil_value_t::histosols: return mechanics::soil_t::histosols;
            case soil_value_t::kastanozems: return mechanics::soil_t::kastanozems;
            case soil_value_t::leptosols: return mechanics::soil_t::leptosols;
            case soil_value_t::lixisols: return mechanics::soil_t::lixisols;
            case soil_value_t::luvisols: return mechanics::soil_t::luvisols;
            case soil_value_t::nitisols: return mechanics::soil_t::nitisols;
            case soil_value_t::phaeozems: return mechanics::soil_t::phaeozems;
            case soil_value_t::planosols: return mechanics::soil_t::planosols;
            case soil_value_t::plinthosols: return mechanics::soil_t::plinthosols;
            case soil_value_t::podzols: return mechanics::soil_t::podzols;
            case soil_value_t::regosols: return mechanics::soil_t::regosols;
            case soil_value_t::solonchaks: return mechanics::soil_t::solonchaks;
            case soil_value_t::solonetz: return mechanics::soil_t::solonetz;
            case soil_value_t::stagnosols: return mechanics::soil_t::stagnosols;
            case soil_value_t::umbrisols: return mechanics::soil_t::umbrisols;
            case soil_value_t::vertisols: return mechanics::soil_t::vertisols;
        }
        throw std::runtime_error("Invalid soil_t value");
    }

    auto properties_from_values(const koppen_value_t koppen, const elevation_t elevation,
        const vegetation_value_t vegetation,
        const soil_value_t soil) -> mechanics::province_properties_t {
        mechanics::province_properties_t props{
            .koppen = koppen_value_to_koppen(koppen), .vegetation = vegetation_value_to_vegetation(vegetation),
            .soil = soil_value_to_soil(soil)
        };
        switch (elevation) {
            case elevation_t::none: { throw std::runtime_error("elevation_t is none"); }
            case elevation_t::flat_lowland: {
                props.elevation = mechanics::elevation_level_t::lowland;
                props.roughness = mechanics::roughness_t::flat;
            }
            break;
            case elevation_t::flat_midland: {
                props.elevation = mechanics::elevation_level_t::midland;
                props.roughness = mechanics::roughness_t::flat;
            }
            break;
            case elevation_t::flat_highland: {
                props.elevation = mechanics::elevation_level_t::highland;
                props.roughness = mechanics::roughness_t::flat;
            }
            break;
            case elevation_t::hill_lowland: {
                props.elevation = mechanics::elevation_level_t::lowland;
                props.roughness = mechanics::roughness_t::hilly;
            }
            break;
            case elevation_t::mountain_lowland: {
                props.elevation = mechanics::elevation_level_t::lowland;
                props.roughness = mechanics::roughness_t::mountainous;
            }
            break;
            case elevation_t::hill_midland: {
                props.elevation = mechanics::elevation_level_t::midland;
                props.roughness = mechanics::roughness_t::hilly;
            }
            break;
            case elevation_t::mountain_midland: {
                props.elevation = mechanics::elevation_level_t::midland;
                props.roughness = mechanics::roughness_t::mountainous;
            }
            break;
            case elevation_t::hill_highland: {
                props.elevation = mechanics::elevation_level_t::highland;
                props.roughness = mechanics::roughness_t::hilly;
            }
            break;
            case elevation_t::mountain_highland: {
                props.elevation = mechanics::elevation_level_t::highland;
                props.roughness = mechanics::roughness_t::mountainous;
            }
            break;
        }
        return props;
    }

    auto load_provinces() -> void {
        std::ifstream province_file("assets/provinces.txt");
        if (!province_file.is_open()) { throw std::runtime_error("Failed to open province file: " "provinces.txt"); }
        while (!province_file.eof()) {
            std::string ignore;
            std::getline(province_file, ignore, ':');
            if (ignore == "\n") { break; }
            unsigned int color{};
            province_file >> color;
            std::getline(province_file, ignore, ':');
            unsigned int koppen_value{};
            province_file >> koppen_value;
            std::getline(province_file, ignore, ':');
            unsigned int elevation_value{};
            province_file >> elevation_value;
            std::getline(province_file, ignore, ':');
            unsigned int vegetation_value{};
            province_file >> vegetation_value;
            std::getline(province_file, ignore, ':');
            unsigned int soil_value{};
            province_file >> soil_value;
            std::getline(province_file, ignore);
            mechanics::data::emplace_province(properties_from_values(static_cast<koppen_value_t>(koppen_value),
                static_cast<elevation_t>(elevation_value),
                static_cast<vegetation_value_t>(vegetation_value),
                static_cast<soil_value_t>(soil_value)), color);
        }
        province_file.close();
    }

    auto convert_sea_types(const sea_value_t sea) -> mechanics::sea_t {
        switch (sea) {
            case sea_value_t::none: { throw std::runtime_error("sea_t is none"); }
            case sea_value_t::river: { throw std::runtime_error("sea_t is river"); }
            case sea_value_t::coast: return mechanics::sea_t::coast;
            case sea_value_t::lake: return mechanics::sea_t::lake;
            case sea_value_t::polar: return mechanics::sea_t::polar;
            case sea_value_t::northeasterly: return mechanics::sea_t::northeasterly;
            case sea_value_t::southeasterly: return mechanics::sea_t::southeasterly;
            case sea_value_t::sea: return mechanics::sea_t::sea;
            case sea_value_t::westerly: return mechanics::sea_t::westerly;
            case sea_value_t::doldrums: return mechanics::sea_t::doldrums;
        }
        throw std::runtime_error("Invalid sea_t value");
    }

    auto load_seas() -> void {
        std::ifstream sea_file("assets/sea_tiles.txt");
        if (!sea_file.is_open()) { throw std::runtime_error("Failed to open sea tiles file: " "sea_tiles.txt"); }
        while (!sea_file.eof()) {
            std::string ignore;
            std::getline(sea_file, ignore, ':');
            if (ignore == "\n") { break; }
            unsigned int color{};
            sea_file >> color;
            std::getline(sea_file, ignore, ':');
            unsigned int sea_value{};
            sea_file >> sea_value;
            mechanics::data::emplace_province(mechanics::sea_properties_t{
                .sea_type = convert_sea_types(static_cast<sea_value_t>(sea_value))
            }, color);
        }
        sea_file.close();
    }

    auto load_river_tiles() -> void {
        std::ifstream river_tiles_file("assets/river_tiles.txt");
        if (!river_tiles_file.is_open()) {
            throw std::runtime_error("Failed to open river tiles file: " "river_tiles.txt");
        }
        while (!river_tiles_file.eof()) {
            std::string ignore;
            std::getline(river_tiles_file, ignore, ':');
            if (ignore == "\n") { break; }
            unsigned int color{};
            river_tiles_file >> color;
            std::getline(river_tiles_file, ignore, ':');
            unsigned int river_value{};
            river_tiles_file >> river_value;
            mechanics::data::emplace_province(mechanics::river_properties_t{.width = static_cast<unsigned char>(river_value)}, color);
        }
        river_tiles_file.close();
    }

    auto load_river_borders() -> void {
        std::ifstream rivers_file("assets/rivers.txt");
        if (!rivers_file.is_open()) { throw std::runtime_error("Failed to open rivers file: " "rivers.txt"); }
        while (!rivers_file.eof()) {
            std::string ignore;
            std::getline(rivers_file, ignore, ':');
            if (ignore == "\n") { break; }
            unsigned int color{};
            rivers_file >> color;
            std::getline(rivers_file, ignore, ':');
            unsigned int neighbor_color{};
            rivers_file >> neighbor_color;
            std::getline(rivers_file, ignore, ':');
            unsigned int river_value{};
            rivers_file >> river_value;
            mechanics::data::province_at(color).add_river_neighbor(mechanics::data::province_at(neighbor_color),
                static_cast<unsigned char>(river_value));
        }
        rivers_file.close();
    }

    auto load_impassable_neighbors() -> void {
        std::ifstream impassable_file("assets/impassable_crossings.txt");
        if (!impassable_file.is_open()) {
            throw std::runtime_error("Failed to open impassable neighbors file: " "impassable_crossings.txt");
        }
        while (!impassable_file.eof()) {
            std::string ignore;
            std::getline(impassable_file, ignore, ':');
            if (ignore == "\n") { break; }
            unsigned int color{};
            impassable_file >> color;
            std::getline(impassable_file, ignore, ':');
            unsigned int neighbor_color{};
            impassable_file >> neighbor_color;
            mechanics::data::province_at(color).add_impassable_neighbor(mechanics::data::province_at(neighbor_color));
        }
        impassable_file.close();
    }

    auto process_pixel(
        std::unordered_map<utils::ref<mechanics::province>, std::vector<std::array<unsigned int, 2UZ>>, utils::ref<
            mechanics::province>::hash> &pixels_by_province, const processing::image &map_image,
        const unsigned int color, const std::array<unsigned int, 2UZ> &position) -> void {
        auto &this_province = mechanics::data::province_at(color);
        this_province.expand_bounds(position);
        pixels_by_province[this_province].push_back(position);

        if (this_province.type() == mechanics::location_type_t::sea || this_province.type() ==
            mechanics::location_type_t::river) {
            ui::map_view::set_pixel_flags({position[0UZ], position[1UZ]}, ui::pixel_flag_t::water, false);
        }

        auto impassable_neighbor = false;

        std::ranges::for_each(std::views::iota(0U, 2U + 1U), [&](const unsigned int dx) {
            std::ranges::for_each(std::views::iota(0U, 2U + 1U), [&](const unsigned int dy) {
                if (dx == 1U && dy == 1U) { return; }
                if ((position[0UZ] == 0U && dx == 0U) || (position[1UZ] == 0U && dy == 0U)) { return; }
                const auto ni = position[0UZ] + dx - 1U;
                const auto nj = position[1UZ] + dy - 1U;
                if (ni >= map_image.width() ||
                    nj >= map_image.height()) { return; }
                const auto neighbor_color = map_image.color(ni, nj);
                if (neighbor_color == color) { return; }
                auto &neighbor = mechanics::data::province_at(neighbor_color);
                this_province.add_neighbor(neighbor);
                if (!impassable_neighbor && this_province.neighbors().at(neighbor).impassable) {
                    impassable_neighbor = true;
                }
            });
        });
        if (impassable_neighbor) {
            ui::map_view::set_pixel_flags({position[0UZ], position[1UZ]}, ui::pixel_flag_t::impassable, false);
        }
    }
}

namespace processing {
    auto load_image(image &map_image, std::string &loading_text) -> void {
        loading_text = "Loading provinces file...";
        load_provinces();

        loading_text = "Loading sea tiles file...";
        load_seas();

        loading_text = "Loading river tiles file...";
        load_river_tiles();

        mechanics::data::lock_provinces();

        loading_text = "Loading rivers file...";
        load_river_borders();

        loading_text = "Loading impassable neighbors file...";
        load_impassable_neighbors();

        map_image = image{"assets/provinces_generated.png"};

        std::unordered_map<utils::ref<mechanics::province>, std::vector<std::array<unsigned int, 2UZ>>, utils::ref<
            mechanics::province>::hash> pixels_by_province;

        loading_text = "Processing pixels...";
        ui::map_view::set_pixel_flags_size(map_image.width(), map_image.height());

        std::ranges::for_each(std::views::iota(0U, map_image.height()), [&](const unsigned int j) {
            std::ranges::for_each(std::views::iota(0U, map_image.width()), [&](const unsigned int i) {
                process_pixel(pixels_by_province, map_image, map_image.color(i, j), {i, j});
            });
            loading_text = "Processing pixels..." + std::to_string((j * 100U) / map_image.height()) + "%";
        });

        auto &province_values = mechanics::data::provinces();

        loading_text = "Finalizing provinces...";
        std::for_each(std::execution::unseq, province_values.begin(), province_values.end(),
            [&](mechanics::province &province) {
                if (province.size() == 0U) { return; }
                province.finalize(pixels_by_province.at(province));
            });
        std::for_each(std::execution::unseq, province_values.begin(), province_values.end(),
            [](mechanics::province &province) { province.process_distances(); });

        loading_text.clear();
    }
}
