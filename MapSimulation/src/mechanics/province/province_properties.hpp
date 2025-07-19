#pragma once
#include <string_view>
#include "../../utils/colors.hpp"

namespace mechanics {
    enum class koppen_t : uint_fast32_t {
        none = 0,
        af = utils::to_integer_color(0, 0, 255),
        am = utils::to_integer_color(0, 120, 255),
        aw = utils::to_integer_color(70, 170, 250),
        bwh = utils::to_integer_color(255, 0, 0),
        bwk = utils::to_integer_color(255, 150, 150),
        bsh = utils::to_integer_color(245, 165, 0),
        bsk = utils::to_integer_color(255, 220, 100),
        csa = utils::to_integer_color(255, 255, 0),
        csb = utils::to_integer_color(200, 200, 0),
        csc = utils::to_integer_color(150, 150, 0),
        cwa = utils::to_integer_color(150, 255, 150),
        cwb = utils::to_integer_color(100, 200, 100),
        cwc = utils::to_integer_color(50, 150, 50),
        cfa = utils::to_integer_color(200, 255, 80),
        cfb = utils::to_integer_color(100, 255, 80),
        cfc = utils::to_integer_color(50, 200, 0),
        dsa = utils::to_integer_color(255, 0, 255),
        dsb = utils::to_integer_color(200, 0, 200),
        dsc = utils::to_integer_color(150, 50, 150),
        dsd = utils::to_integer_color(150, 100, 150),
        dwa = utils::to_integer_color(170, 175, 255),
        dwb = utils::to_integer_color(90, 120, 220),
        dwc = utils::to_integer_color(78, 80, 180),
        dwd = utils::to_integer_color(50, 0, 135),
        dfa = utils::to_integer_color(0, 255, 255),
        dfb = utils::to_integer_color(55, 200, 255),
        dfc = utils::to_integer_color(0, 125, 125),
        dfd = utils::to_integer_color(0, 70, 95),
        et = utils::to_integer_color(178, 178, 178),
        ef = utils::to_integer_color(102, 102, 102),
    };

    enum class elevation_t : uint_fast32_t {
        none = 0,
        flatland = utils::to_integer_color(0, 255, 0),
        hills = utils::to_integer_color(30, 200, 200),
        plateau = utils::to_integer_color(200, 200, 10),
        highlands = utils::to_integer_color(200, 10, 10),
        mountains = utils::to_integer_color(50, 20, 20),
    };

    enum class vegetation_t : uint_fast32_t {
        none = 0,
        tropical_evergreen_broadleaf_forest = utils::to_integer_color(28, 85, 16),
        tropical_semi_evergreen_broadleaf_forest = utils::to_integer_color(101, 146, 8),
        tropical_deciduous_broadleaf_forest_and_woodland = utils::to_integer_color(174, 125, 32),
        warm_temperate_evergreen_and_mixed_forest = utils::to_integer_color(0, 0, 101),
        cool_temperate_rainforest = utils::to_integer_color(187, 203, 53),
        cool_evergreen_needleleaf_forest = utils::to_integer_color(0, 154, 24),
        cool_mixed_forest = utils::to_integer_color(202, 255, 202),
        temperate_deciduous_broadleaf_forest = utils::to_integer_color(85, 235, 73),
        cold_deciduous_forest = utils::to_integer_color(101, 178, 255),
        cold_evergreen_needleleaf_forest = utils::to_integer_color(0, 32, 202),
        temperate_sclerophyll_woodland_and_shrubland = utils::to_integer_color(142, 162, 40),
        temperate_evergreen_needleleaf_open_woodland = utils::to_integer_color(255, 154, 223),
        tropical_savanna = utils::to_integer_color(186, 255, 53),
        xerophytic_woods_scrub = utils::to_integer_color(255, 186, 154),
        steppe = utils::to_integer_color(255, 186, 53),
        desert = utils::to_integer_color(247, 255, 202),
        graminoid_and_forb_tundra = utils::to_integer_color(231, 231, 24),
        erect_dwarf_shrub_tundra = utils::to_integer_color(121, 134, 73),
        low_and_high_shrub_tundra = utils::to_integer_color(101, 255, 154),
        prostrate_dwarf_shrub_tundra = utils::to_integer_color(210, 158, 150),
    };

    enum class soil_t : uint_fast32_t {
        none = 0,
        acrisols = utils::to_integer_color(247, 153, 29),
        albeluvisols = utils::to_integer_color(155, 157, 87),
        alisols = utils::to_integer_color(250, 247, 192),
        andosols = utils::to_integer_color(237, 58, 51),
        arenosols = utils::to_integer_color(247, 216, 172),
        calcisols = utils::to_integer_color(255, 238, 0),
        cambisols = utils::to_integer_color(254, 205, 103),
        chernozems = utils::to_integer_color(226, 200, 55),
        cryosols = utils::to_integer_color(117, 106, 146),
        durisols = utils::to_integer_color(239, 230, 191),
        ferrasols = utils::to_integer_color(246, 135, 45),
        fluvisols = utils::to_integer_color(1, 176, 239),
        gleysols = utils::to_integer_color(146, 145, 185),
        gypsisols = utils::to_integer_color(251, 246, 165),
        histosols = utils::to_integer_color(139, 137, 138),
        kastanozems = utils::to_integer_color(201, 149, 128),
        leptosols = utils::to_integer_color(213, 214, 216),
        lixisols = utils::to_integer_color(249, 189, 191),
        luvisols = utils::to_integer_color(244, 131, 133),
        nitisols = utils::to_integer_color(247, 160, 130),
        phaeozems = utils::to_integer_color(186, 104, 80),
        planosols = utils::to_integer_color(245, 147, 84),
        plinthosols = utils::to_integer_color(111, 14, 65),
        podzols = utils::to_integer_color(13, 175, 99),
        regosols = utils::to_integer_color(255, 226, 174),
        solonchaks = utils::to_integer_color(237, 57, 148),
        solonetz = utils::to_integer_color(244, 205, 226),
        stagnosols = utils::to_integer_color(64, 193, 235),
        umbrisols = utils::to_integer_color(97, 143, 130),
        vertisols = utils::to_integer_color(158, 86, 124),
    };

    enum class sea_t :uint_fast32_t {
        none = 0,
        coast = utils::to_integer_color(25, 255, 255),
        sea = utils::to_integer_color(100, 200, 255),
        southeasterly = utils::to_integer_color(255, 100, 100),
        northeasterly = utils::to_integer_color(100, 100, 255),
        westerly = utils::to_integer_color(255, 255, 100),
        polar = utils::to_integer_color(200, 255, 255),
        lake = utils::to_integer_color(255, 150, 255),
        river = utils::to_integer_color(0, 255, 0),
    };

    inline auto koppen_to_string(const koppen_t value) -> std::string_view {
        switch (value) {
            case koppen_t::none:
                return "None";
            case koppen_t::af:
                return "Af";
            case koppen_t::am:
                return "Am";
            case koppen_t::aw:
                return "Aw";
            case koppen_t::bwh:
                return "BWh";
            case koppen_t::bwk:
                return "BWk";
            case koppen_t::bsh:
                return "BSh";
            case koppen_t::bsk:
                return "BSk";
            case koppen_t::csa:
                return "Csa";
            case koppen_t::csb:
                return "Csb";
            case koppen_t::csc:
                return "Csc";
            case koppen_t::cwa:
                return "Cwa";
            case koppen_t::cwb:
                return "Cwb";
            case koppen_t::cwc:
                return "Cwc";
            case koppen_t::cfa:
                return "Cfa";
            case koppen_t::cfb:
                return "Cfb";
            case koppen_t::cfc:
                return "Cfc";
            case koppen_t::dsa:
                return "Dsa";
            case koppen_t::dsb:
                return "Dsb";
            case koppen_t::dsc:
                return "Dsc";
            case koppen_t::dsd:
                return "Dsd";
            case koppen_t::dwa:
                return "Dwa";
            case koppen_t::dwb:
                return "Dwb";
            case koppen_t::dwc:
                return "Dwc";
            case koppen_t::dwd:
                return "Dwd";
            case koppen_t::dfa:
                return "Dfa";
            case koppen_t::dfb:
                return "Dfb";
            case koppen_t::dfc:
                return "Dfc";
            case koppen_t::dfd:
                return "Dfd";
            case koppen_t::et:
                return "ET";
            case koppen_t::ef:
                return "EF";
            default:
                return "No climate type";
        }
    }

    inline auto elevation_to_string(const elevation_t value) -> std::string_view {
        switch (value) {
            case elevation_t::none:
                return "None";
            case elevation_t::flatland:
                return "Flatland";
            case elevation_t::hills:
                return "Hills";
            case elevation_t::plateau:
                return "Plateau";
            case elevation_t::highlands:
                return "Highlands";
            case elevation_t::mountains:
                return "Mountains";
            default:
                return "No elevation type";
        }
    }

    inline auto vegetation_to_string(const vegetation_t value) -> std::string_view {
        switch (value) {
            case vegetation_t::none:
                return "None";
            case vegetation_t::tropical_evergreen_broadleaf_forest:
                return "Tropical Evergreen Broadleaf Forest";
            case vegetation_t::tropical_semi_evergreen_broadleaf_forest:
                return "Tropical Semi-Evergreen Broadleaf Forest";
            case vegetation_t::tropical_deciduous_broadleaf_forest_and_woodland:
                return "Tropical Deciduous Broadleaf Forest and Woodland";
            case vegetation_t::warm_temperate_evergreen_and_mixed_forest:
                return "Warm Temperate Evergreen and Mixed Forest";
            case vegetation_t::cool_temperate_rainforest:
                return "Cool Temperate Rainforest";
            case vegetation_t::cool_evergreen_needleleaf_forest:
                return "Cool Evergreen Needleleaf Forest";
            case vegetation_t::cool_mixed_forest:
                return "Cool Mixed Forest";
            case vegetation_t::temperate_deciduous_broadleaf_forest:
                return "Temperate Deciduous Broadleaf Forest";
            case vegetation_t::cold_deciduous_forest:
                return "Cold Deciduous Forest";
            case vegetation_t::cold_evergreen_needleleaf_forest:
                return "Cold Evergreen Needleleaf Forest";
            case vegetation_t::temperate_sclerophyll_woodland_and_shrubland:
                return "Temperate Sclerophyll Woodland and Shrubland";
            case vegetation_t::temperate_evergreen_needleleaf_open_woodland:
                return "Temperate Evergreen Needleleaf Open Woodland";
            case vegetation_t::tropical_savanna:
                return "Tropical Savanna";
            case vegetation_t::xerophytic_woods_scrub:
                return "Xerophytic Woods Scrub";
            case vegetation_t::steppe:
                return "Steppe";
            case vegetation_t::desert:
                return "Desert";
            case vegetation_t::graminoid_and_forb_tundra:
                return "Graminoid and Forb Tundra";
            case vegetation_t::erect_dwarf_shrub_tundra:
                return "Erect Dwarf Shrub Tundra";
            case vegetation_t::low_and_high_shrub_tundra:
                return "Low and High Shrub Tundra";
            case vegetation_t::prostrate_dwarf_shrub_tundra:
                return "Prostrate Dwarf Shrub Tundra";
            default:
                return "No vegetation type";
        }
    }

    inline auto soil_to_string(const soil_t value) -> std::string_view {
        switch (value) {
            case soil_t::none:
                return "None";
            case soil_t::acrisols:
                return "Acrisols";
            case soil_t::albeluvisols:
                return "Albeluvisols";
            case soil_t::alisols:
                return "Alisols";
            case soil_t::andosols:
                return "Andosols";
            case soil_t::arenosols:
                return "Arenosols";
            case soil_t::calcisols:
                return "Calcisols";
            case soil_t::cambisols:
                return "Cambisols";
            case soil_t::chernozems:
                return "Chernozems";
            case soil_t::cryosols:
                return "Cryosols";
            case soil_t::durisols:
                return "Durisols";
            case soil_t::ferrasols:
                return "Ferrasols";
            case soil_t::fluvisols:
                return "Fluvisols";
            case soil_t::gleysols:
                return "Gleysols";
            case soil_t::gypsisols:
                return "Gypsisols";
            case soil_t::histosols:
                return "Histosols";
            case soil_t::kastanozems:
                return "Kastanozems";
            case soil_t::leptosols:
                return "Leptosols";
            case soil_t::lixisols:
                return "Lixisols";
            case soil_t::luvisols:
                return "Luvisols";
            case soil_t::nitisols:
                return "Nitisols";
            case soil_t::phaeozems:
                return "Phaeozems";
            case soil_t::planosols:
                return "Planosols";
            case soil_t::plinthosols:
                return "Plinthosols";
            case soil_t::podzols:
                return "Podzols";
            case soil_t::regosols:
                return "Regosols";
            case soil_t::solonchaks:
                return "Solonchaks";
            case soil_t::solonetz:
                return "Solonetz";
            case soil_t::stagnosols:
                return "Stagnosols";
            case soil_t::umbrisols:
                return "Umbrisols";
            case soil_t::vertisols:
                return "Vertisols";
            default:
                return "No soil type";
        }
    }

    inline auto sea_to_string(const sea_t value) -> std::string_view {
        switch (value) {
            case sea_t::none:
                return "None";
            case sea_t::coast:
                return "Coast";
            case sea_t::lake:
                return "Lake";
            case sea_t::polar:
                return "Polar";
            case sea_t::northeasterly:
                return "Northeasterly";
            case sea_t::southeasterly:
                return "Southeasterly";
            case sea_t::sea:
                return "Sea";
            case sea_t::westerly:
                return "Westerly";
            case sea_t::river:
                return "River";
            default:
                return "No sea type";
        }
    }
}
