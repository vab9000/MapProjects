#pragma once
#include "../../utils/colors.hpp"
#include "../../utils/zstring_view.hpp"

namespace mechanics {
    enum class koppen_t : uint_fast32_t {
        none = 0U,
        af = utils::to_integer_color(0U, 0U, 255U),
        am = utils::to_integer_color(0U, 120U, 255U),
        aw = utils::to_integer_color(70U, 170U, 250U),
        bwh = utils::to_integer_color(255U, 0U, 0U),
        bwk = utils::to_integer_color(255U, 150U, 150U),
        bsh = utils::to_integer_color(245U, 165U, 0U),
        bsk = utils::to_integer_color(255U, 220U, 100U),
        csa = utils::to_integer_color(255U, 255U, 0U),
        csb = utils::to_integer_color(200U, 200U, 0U),
        csc = utils::to_integer_color(150U, 150U, 0U),
        cwa = utils::to_integer_color(150U, 255U, 150U),
        cwb = utils::to_integer_color(100U, 200U, 100U),
        cwc = utils::to_integer_color(50U, 150U, 50U),
        cfa = utils::to_integer_color(200U, 255U, 80U),
        cfb = utils::to_integer_color(100U, 255U, 80U),
        cfc = utils::to_integer_color(50U, 200U, 0U),
        dsa = utils::to_integer_color(255U, 0U, 255U),
        dsb = utils::to_integer_color(200U, 0U, 200U),
        dsc = utils::to_integer_color(150U, 50U, 150U),
        dsd = utils::to_integer_color(150U, 100U, 150U),
        dwa = utils::to_integer_color(170U, 175U, 255U),
        dwb = utils::to_integer_color(90U, 120U, 220U),
        dwc = utils::to_integer_color(78U, 80U, 180U),
        dwd = utils::to_integer_color(50U, 0U, 135U),
        dfa = utils::to_integer_color(0U, 255U, 255U),
        dfb = utils::to_integer_color(55U, 200U, 255U),
        dfc = utils::to_integer_color(0U, 125U, 125U),
        dfd = utils::to_integer_color(0U, 70U, 95U),
        et = utils::to_integer_color(178U, 178U, 178U),
        ef = utils::to_integer_color(102U, 102U, 102U),
    };

    enum class elevation_t : uint_fast32_t {
        none = 0U,
        flatland = utils::to_integer_color(0U, 255U, 0U),
        hills = utils::to_integer_color(30U, 200U, 200U),
        plateau = utils::to_integer_color(200U, 200U, 10U),
        highlands = utils::to_integer_color(200U, 10U, 10U),
        mountains = utils::to_integer_color(50U, 20U, 20U),
    };

    enum class vegetation_t : uint_fast32_t {
        none = 0U,
        tropical_evergreen_broadleaf_forest = utils::to_integer_color(28U, 85U, 16U),
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
        steppe = utils::to_integer_color(255U, 186U, 53U),
        desert = utils::to_integer_color(247U, 255U, 202U),
        graminoid_and_forb_tundra = utils::to_integer_color(231U, 231U, 24U),
        erect_dwarf_shrub_tundra = utils::to_integer_color(121U, 134U, 73U),
        low_and_high_shrub_tundra = utils::to_integer_color(101U, 255U, 154U),
        prostrate_dwarf_shrub_tundra = utils::to_integer_color(210U, 158U, 150U),
    };

    enum class soil_t : uint_fast32_t {
        none = 0U,
        acrisols = utils::to_integer_color(247U, 153U, 29U),
        albeluvisols = utils::to_integer_color(155U, 157U, 87U),
        alisols = utils::to_integer_color(250U, 247U, 192U),
        andosols = utils::to_integer_color(237U, 58U, 51U),
        arenosols = utils::to_integer_color(247U, 216U, 172U),
        calcisols = utils::to_integer_color(255U, 238U, 0U),
        cambisols = utils::to_integer_color(254U, 205U, 103U),
        chernozems = utils::to_integer_color(226U, 200U, 55U),
        cryosols = utils::to_integer_color(117U, 106U, 146U),
        durisols = utils::to_integer_color(239U, 230U, 191U),
        ferrasols = utils::to_integer_color(246U, 135U, 45U),
        fluvisols = utils::to_integer_color(1U, 176U, 239U),
        gleysols = utils::to_integer_color(146U, 145U, 185U),
        gypsisols = utils::to_integer_color(251U, 246U, 165U),
        histosols = utils::to_integer_color(139U, 137U, 138U),
        kastanozems = utils::to_integer_color(201U, 149U, 128U),
        leptosols = utils::to_integer_color(213U, 214U, 216U),
        lixisols = utils::to_integer_color(249U, 189U, 191U),
        luvisols = utils::to_integer_color(244U, 131U, 133U),
        nitisols = utils::to_integer_color(247U, 160U, 130U),
        phaeozems = utils::to_integer_color(186U, 104U, 80U),
        planosols = utils::to_integer_color(245U, 147U, 84U),
        plinthosols = utils::to_integer_color(111U, 14U, 65U),
        podzols = utils::to_integer_color(13U, 175U, 99U),
        regosols = utils::to_integer_color(255U, 226U, 174U),
        solonchaks = utils::to_integer_color(237U, 57U, 148U),
        solonetz = utils::to_integer_color(244U, 205U, 226U),
        stagnosols = utils::to_integer_color(64U, 193U, 235U),
        umbrisols = utils::to_integer_color(97U, 143U, 130U),
        vertisols = utils::to_integer_color(158U, 86U, 124U),
    };

    enum class sea_t :uint_fast32_t {
        none = 0U,
        coast = utils::to_integer_color(25U, 255U, 255U),
        sea = utils::to_integer_color(100U, 200U, 255U),
        southeasterly = utils::to_integer_color(255U, 100U, 100U),
        northeasterly = utils::to_integer_color(100U, 100U, 255U),
        westerly = utils::to_integer_color(255U, 255U, 100U),
        polar = utils::to_integer_color(200U, 255U, 255U),
        lake = utils::to_integer_color(255U, 150U, 255U),
        river = utils::to_integer_color(0U, 255U, 0U),
    };

    inline auto koppen_to_string(const koppen_t value) -> utils::zstring_view {
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

    inline auto elevation_to_string(const elevation_t value) -> utils::zstring_view {
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

    inline auto vegetation_to_string(const vegetation_t value) -> utils::zstring_view {
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

    inline auto soil_to_string(const soil_t value) -> utils::zstring_view {
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

    inline auto sea_to_string(const sea_t value) -> utils::zstring_view {
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
