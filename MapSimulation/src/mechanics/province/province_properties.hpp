#ifndef MECHANICS_PROVINCE_PROPERTIES
#define MECHANICS_PROVINCE_PROPERTIES
#include "enums.hpp"

namespace mechanics {
    enum class elevation_level_t : unsigned char {
        lowland, midland, highland,
    };

    enum class roughness_t : unsigned char {
        flat, hilly, mountainous,
    };

    enum class sea_t : unsigned char {
        coast, lake, polar,
        northeasterly, southeasterly, sea,
        westerly, doldrums,
    };

    enum class koppen_t : unsigned char {
        none, af, am, aw,
        bwh, bwk, bsh,
        bsk, csa, csb,
        csc, cwa, cwb,
        cwc, cfa, cfb,
        cfc, dsa, dsb,
        dsc, dsd, dwa,
        dwb, dwc, dwd,
        dfa, dfb, dfc,
        dfd, et, ef,
    };

    enum class vegetation_t : unsigned char {
        none, tropical_evergreen_broadleaf_forest, tropical_semi_evergreen_broadleaf_forest,
        tropical_deciduous_broadleaf_forest_and_woodland, warm_temperate_evergreen_and_mixed_forest,
        cool_temperate_rainforest, cool_evergreen_needleleaf_forest, cool_mixed_forest,
        temperate_deciduous_broadleaf_forest, cold_deciduous_forest, cold_evergreen_needleleaf_forest,
        temperate_sclerophyll_woodland_and_shrubland, temperate_evergreen_needleleaf_open_woodland, tropical_savanna,
        xerophytic_woods_scrub, steppe, desert,
        graminoid_and_forb_tundra, erect_dwarf_shrub_tundra, low_and_high_shrub_tundra,
        prostrate_dwarf_shrub_tundra,
    };

    enum class soil_t : unsigned char {
        none, acrisols, albeluvisols,
        alisols, andosols, arenosols,
        calcisols, cambisols, chernozems,
        cryosols, durisols, ferrasols,
        fluvisols, gleysols, gypsisols,
        histosols, kastanozems, leptosols,
        lixisols, luvisols, nitisols,
        phaeozems, planosols, plinthosols,
        podzols, regosols, solonchaks,
        solonetz, stagnosols, umbrisols,
        vertisols,
    };

    struct province_properties_t {
        elevation_level_t elevation : utils::enum_bits_required_v<elevation_level_t>;
        roughness_t roughness : utils::enum_bits_required_v<roughness_t>;
        koppen_t koppen : utils::enum_bits_required_v<koppen_t>;
        vegetation_t vegetation : utils::enum_bits_required_v<vegetation_t>;
        soil_t soil : utils::enum_bits_required_v<soil_t>;
    };

    struct sea_properties_t {
        sea_t sea_type : utils::enum_bits_required_v<sea_t>;
        koppen_t koppen : utils::enum_bits_required_v<koppen_t>;
    };

    struct river_properties_t {
        koppen_t koppen : utils::enum_bits_required_v<koppen_t>;
        unsigned char width;
    };

    enum class location_type_t : unsigned char {
        province, sea, river,
    };

    union location_properties_t {
        province_properties_t province_properties;
        sea_properties_t sea_properties;
        river_properties_t river_properties;

        template<location_type_t T>
        using type = std::conditional_t<T == location_type_t::province, province_properties_t,
            std::conditional_t<T == location_type_t::sea, sea_properties_t,
                river_properties_t>>;
    };
}

#endif