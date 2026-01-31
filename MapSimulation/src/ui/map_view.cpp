#include "map_view.hpp"
#include <colors.hpp>
#include "../mechanics/data.hpp"
#include "../mechanics/province/province.hpp"

namespace {
    auto koppen_color(const mechanics::koppen_t koppen) -> unsigned int {
        switch (koppen) {
            case mechanics::koppen_t::none: return 0xFFFFFFU;
            case mechanics::koppen_t::af: return utils::to_integer_color(0U, 0U, 255U);
            case mechanics::koppen_t::am: return utils::to_integer_color(0U, 120U, 255U);
            case mechanics::koppen_t::aw: return utils::to_integer_color(70U, 170U, 250U);
            case mechanics::koppen_t::bwh: return utils::to_integer_color(255U, 0U, 0U);
            case mechanics::koppen_t::bwk: return utils::to_integer_color(255U, 150U, 150U);
            case mechanics::koppen_t::bsh: return utils::to_integer_color(245U, 165U, 0U);
            case mechanics::koppen_t::bsk: return utils::to_integer_color(255U, 220U, 100U);
            case mechanics::koppen_t::csa: return utils::to_integer_color(255U, 255U, 0U);
            case mechanics::koppen_t::csb: return utils::to_integer_color(200U, 200U, 0U);
            case mechanics::koppen_t::csc: return utils::to_integer_color(150U, 150U, 0U);
            case mechanics::koppen_t::cwa: return utils::to_integer_color(150U, 255U, 150U);
            case mechanics::koppen_t::cwb: return utils::to_integer_color(100U, 200U, 100U);
            case mechanics::koppen_t::cwc: return utils::to_integer_color(50U, 150U, 50U);
            case mechanics::koppen_t::cfa: return utils::to_integer_color(200U, 255U, 80U);
            case mechanics::koppen_t::cfb: return utils::to_integer_color(100U, 255U, 80U);
            case mechanics::koppen_t::cfc: return utils::to_integer_color(50U, 200U, 0U);
            case mechanics::koppen_t::dsa: return utils::to_integer_color(255U, 0U, 255U);
            case mechanics::koppen_t::dsb: return utils::to_integer_color(200U, 0U, 200U);
            case mechanics::koppen_t::dsc: return utils::to_integer_color(150U, 50U, 150U);
            case mechanics::koppen_t::dsd: return utils::to_integer_color(150U, 100U, 150U);
            case mechanics::koppen_t::dwa: return utils::to_integer_color(170U, 175U, 255U);
            case mechanics::koppen_t::dwb: return utils::to_integer_color(90U, 120U, 220U);
            case mechanics::koppen_t::dwc: return utils::to_integer_color(78U, 80U, 180U);
            case mechanics::koppen_t::dwd: return utils::to_integer_color(50U, 0U, 135U);
            case mechanics::koppen_t::dfa: return utils::to_integer_color(0U, 255U, 255U);
            case mechanics::koppen_t::dfb: return utils::to_integer_color(55U, 200U, 255U);
            case mechanics::koppen_t::dfc: return utils::to_integer_color(0U, 125U, 125U);
            case mechanics::koppen_t::dfd: return utils::to_integer_color(0U, 70U, 95U);
            case mechanics::koppen_t::et: return utils::to_integer_color(178U, 178U, 178U);
            case mechanics::koppen_t::ef: return utils::to_integer_color(102U, 102U, 102U);
        }
        return 0xFFFFFFU;
    }

    auto vegetation_color(const mechanics::vegetation_t vegetation) -> unsigned int {
        switch (vegetation) {
            case mechanics::vegetation_t::none: return 0xFFFFFFU;
            case mechanics::vegetation_t::tropical_evergreen_broadleaf_forest: return utils::to_integer_color(28U, 85U,
                    16U);
            case mechanics::vegetation_t::tropical_semi_evergreen_broadleaf_forest: return utils::to_integer_color(101U,
                    146U, 8U);
            case mechanics::vegetation_t::tropical_deciduous_broadleaf_forest_and_woodland: return
                    utils::to_integer_color(174U,
                        125U, 32U);
            case mechanics::vegetation_t::warm_temperate_evergreen_and_mixed_forest: return utils::to_integer_color(0U,
                    0U, 101U);
            case mechanics::vegetation_t::cool_temperate_rainforest: return utils::to_integer_color(187U, 203U, 53U);
            case mechanics::vegetation_t::cool_evergreen_needleleaf_forest: return utils::to_integer_color(0U, 154U,
                    24U);
            case mechanics::vegetation_t::cool_mixed_forest: return utils::to_integer_color(202U, 255U, 202U);
            case mechanics::vegetation_t::temperate_deciduous_broadleaf_forest: return utils::to_integer_color(85U,
                    235U, 73U);
            case mechanics::vegetation_t::cold_deciduous_forest: return utils::to_integer_color(101U, 178U, 255U);
            case mechanics::vegetation_t::cold_evergreen_needleleaf_forest: return utils::to_integer_color(0U, 32U,
                    202U);
            case mechanics::vegetation_t::temperate_sclerophyll_woodland_and_shrubland: return utils::to_integer_color(
                    142U, 162U,
                    40U);
            case mechanics::vegetation_t::temperate_evergreen_needleleaf_open_woodland: return utils::to_integer_color(
                    255U, 154U,
                    223U);
            case mechanics::vegetation_t::tropical_savanna: return utils::to_integer_color(186U, 255U, 53U);
            case mechanics::vegetation_t::xerophytic_woods_scrub: return utils::to_integer_color(255U, 186U, 154U);
            case mechanics::vegetation_t::steppe: return utils::to_integer_color(255U, 186U, 53U);
            case mechanics::vegetation_t::desert: return utils::to_integer_color(247U, 255U, 202U);
            case mechanics::vegetation_t::graminoid_and_forb_tundra: return utils::to_integer_color(231U, 231U, 24U);
            case mechanics::vegetation_t::erect_dwarf_shrub_tundra: return utils::to_integer_color(121U, 134U, 73U);
            case mechanics::vegetation_t::low_and_high_shrub_tundra: return utils::to_integer_color(101U, 255U, 154U);
            case mechanics::vegetation_t::prostrate_dwarf_shrub_tundra: return
                    utils::to_integer_color(210U, 158U, 150U);
        }
        return 0xFFFFFFU;
    }

    auto soil_color(const mechanics::soil_t soil) -> unsigned int {
        switch (soil) {
            case mechanics::soil_t::none: return 0xFFFFFFU;
            case mechanics::soil_t::acrisols: return utils::to_integer_color(247U, 153U, 29U);
            case mechanics::soil_t::albeluvisols: return utils::to_integer_color(155U, 157U, 87U);
            case mechanics::soil_t::alisols: return utils::to_integer_color(250U, 247U, 192U);
            case mechanics::soil_t::andosols: return utils::to_integer_color(237U, 58U, 51U);
            case mechanics::soil_t::arenosols: return utils::to_integer_color(247U, 216U, 172U);
            case mechanics::soil_t::calcisols: return utils::to_integer_color(255U, 238U, 0U);
            case mechanics::soil_t::cambisols: return utils::to_integer_color(254U, 205U, 103U);
            case mechanics::soil_t::chernozems: return utils::to_integer_color(226U, 200U, 55U);
            case mechanics::soil_t::cryosols: return utils::to_integer_color(117U, 106U, 146U);
            case mechanics::soil_t::durisols: return utils::to_integer_color(239U, 230U, 191U);
            case mechanics::soil_t::ferrasols: return utils::to_integer_color(246U, 135U, 45U);
            case mechanics::soil_t::fluvisols: return utils::to_integer_color(1U, 176U, 239U);
            case mechanics::soil_t::gleysols: return utils::to_integer_color(146U, 145U, 185U);
            case mechanics::soil_t::gypsisols: return utils::to_integer_color(251U, 246U, 165U);
            case mechanics::soil_t::histosols: return utils::to_integer_color(139U, 137U, 138U);
            case mechanics::soil_t::kastanozems: return utils::to_integer_color(201U, 149U, 128U);
            case mechanics::soil_t::leptosols: return utils::to_integer_color(213U, 214U, 216U);
            case mechanics::soil_t::lixisols: return utils::to_integer_color(249U, 189U, 191U);
            case mechanics::soil_t::luvisols: return utils::to_integer_color(244U, 131U, 133U);
            case mechanics::soil_t::nitisols: return utils::to_integer_color(247U, 160U, 130U);
            case mechanics::soil_t::phaeozems: return utils::to_integer_color(186U, 104U, 80U);
            case mechanics::soil_t::planosols: return utils::to_integer_color(245U, 147U, 84U);
            case mechanics::soil_t::plinthosols: return utils::to_integer_color(111U, 14U, 65U);
            case mechanics::soil_t::podzols: return utils::to_integer_color(13U, 175U, 99U);
            case mechanics::soil_t::regosols: return utils::to_integer_color(255U, 226U, 174U);
            case mechanics::soil_t::solonchaks: return utils::to_integer_color(237U, 57U, 148U);
            case mechanics::soil_t::solonetz: return utils::to_integer_color(244U, 205U, 226U);
            case mechanics::soil_t::stagnosols: return utils::to_integer_color(64U, 193U, 235U);
            case mechanics::soil_t::umbrisols: return utils::to_integer_color(97U, 143U, 130U);
            case mechanics::soil_t::vertisols: return utils::to_integer_color(158U, 86U, 124U);
        }
        return 0xFFFFFFU;
    }

    auto province_color(const mechanics::province &p) -> unsigned int {
        switch (ui::map_view::map_mode) {
            case ui::map_mode_t::provinces: return p.base_color();
            case ui::map_mode_t::owner: return p.owner() != nullptr ? p.owner()->color() : 0xFFFFFFU;
            case ui::map_mode_t::koppen: {
                switch (p.type()) {
                    case mechanics::location_type_t::province: {
                        return koppen_color(p.properties<mechanics::location_type_t::province>().koppen);
                    }
                    case mechanics::location_type_t::sea: {
                        if (const auto &[sea_type, koppen] = p.properties<mechanics::location_type_t::sea>();
                            sea_type == mechanics::sea_t::coast ||
                            sea_type == mechanics::sea_t::lake) { return koppen_color(koppen); }
                        return 0xFFFFFFU;
                    }
                    case mechanics::location_type_t::river: {
                        return koppen_color(p.properties<mechanics::location_type_t::river>().koppen);
                    }
                }
            }
            case ui::map_mode_t::elevation: {
                if (p.type() != mechanics::location_type_t::province) { return 0xFFFFFFU; }
                switch (p.properties<mechanics::location_type_t::province>().elevation) {
                    case mechanics::elevation_level_t::lowland: return 0x00FF00U;
                    case mechanics::elevation_level_t::midland: return 0x7F7F00U;
                    case mechanics::elevation_level_t::highland: return 0x7F3F00U;
                    default: return 0xFFFFFFU;
                }
            }
            case ui::map_mode_t::roughness: {
                if (p.type() != mechanics::location_type_t::province) { return 0xFFFFFFU; }
                switch (p.properties<mechanics::location_type_t::province>().roughness) {
                    case mechanics::roughness_t::flat: return 0x00FF00U;
                    case mechanics::roughness_t::hilly: return 0xFFFF00U;
                    case mechanics::roughness_t::mountainous: return 0x7F3F00U;
                    default: return 0xFFFFFFU;
                }
            }
            case ui::map_mode_t::vegetation: {
                if (p.type() != mechanics::location_type_t::province) { return 0xFFFFFFU; }
                return vegetation_color(p.properties<mechanics::location_type_t::province>().vegetation);
            }
            case ui::map_mode_t::soil: {
                if (p.type() != mechanics::location_type_t::province) { return 0xFFFFFFU; }
                return soil_color(p.properties<mechanics::location_type_t::province>().soil);
            }
            case ui::map_mode_t::sea: {
                if (p.type() == mechanics::location_type_t::river) { return 0x00FF00U; }
                if (p.type() != mechanics::location_type_t::sea) { return 0xFFFFFFU; }
                switch (p.properties<mechanics::location_type_t::sea>().sea_type) {
                    case mechanics::sea_t::coast: return 0x00FFFFU;
                    case mechanics::sea_t::lake: return 0xFF00FFU;
                    case mechanics::sea_t::polar: return 0xAAAAFFU;
                    case mechanics::sea_t::northeasterly: return 0xFFFF00U;
                    case mechanics::sea_t::southeasterly: return 0xFFAA00U;
                    case mechanics::sea_t::sea: return 0x0000AAU;
                    case mechanics::sea_t::westerly: return 0xAAFF00U;
                    case mechanics::sea_t::doldrums: return 0x00AAAAU;
                    default: return 0U;
                }
            }
            case ui::map_mode_t::river_size: {
                if (p.type() != mechanics::location_type_t::river) { return 0xFFFFFFU; }
                return utils::to_integer_color(25U, 25U,
                    (2U * 255U + p.properties<mechanics::location_type_t::river>().width) / 3U);
            }
        }
        return 0U;
    }
}

namespace ui {
    auto map_mode{map_mode_t::provinces};

    sf::Texture province_colors;

    sf::Texture pixel_flags;

    mechanics::province *selected = nullptr;

    sf::Vector2u dimensions;

    auto map_view::select_province(mechanics::province &p) -> void { selected = &p; }

    auto map_view::unselect_province() -> void { selected = nullptr; }

    auto map_view::selected_province() -> mechanics::province * { return selected; }

    auto map_view::set_province_colors_size() -> void {
        const auto num_provinces = mechanics::data::provinces().size();
        if (const auto dim = static_cast<unsigned int>(sqrt(num_provinces)) + 1U; !province_colors.resize({dim, dim})) {
            throw std::runtime_error("Failed to allocate memory for provinces.");
        }
        dimensions = province_colors.getSize();
    }

    auto map_view::update_province_color(const mechanics::province &p) -> void {
        const auto color = province_color(p);
        const std::array<unsigned char, 4U> color_array{
            static_cast<unsigned char>(color >> 16), static_cast<unsigned char>(color >> 8),
            static_cast<unsigned char>(color), 255U
        };
        const auto index = p.id() * 4UZ;
        province_colors.update(color_array.data(), {1U, 1U}, {
            static_cast<unsigned int>(index % dimensions.x), static_cast<unsigned int>(index / dimensions.x)
        });
    }

    auto map_view::update_all_province_colors() -> void {
        const auto &provinces = mechanics::data::provinces();
        std::vector<unsigned char> colors(4UZ * dimensions.x * dimensions.y);
        for (const auto &p : provinces) {
            const auto color = province_color(p);
            const auto index = p.id() * 4UZ;
            colors[index + 3UZ] = 255U;
            colors[index + 2UZ] = static_cast<unsigned char>(color);
            colors[index + 1UZ] = static_cast<unsigned char>(color >> 8);
            colors[index] = static_cast<unsigned char>(color >> 16);
        }
        province_colors.update(colors.data(), {dimensions.x, dimensions.y}, {
            0U, 0U
        });
    }

    sf::Vector2u pixel_flags_dimensions;

    std::vector<unsigned char> pixel_flags_data;

    auto map_view::set_pixel_flags_size(const unsigned int width, const unsigned int height) -> void {
        pixel_flags_dimensions = {width, height};
        pixel_flags_data.resize(4UZ * width * height, 0U);
        if (!pixel_flags.resize({width, height})) {
            throw std::runtime_error("Failed to allocate memory for pixel flags.");
        }
    }

    auto map_view::set_pixel_flags(const std::pair<const unsigned int, const unsigned int> pixel, const pixel_flag_t flag,
        const bool clear) -> void {
        const auto index = 4UZ * (pixel.first + pixel.second * pixel_flags_dimensions.x);
        const auto flag_value = static_cast<unsigned int>(flag);
        if (clear) {
            pixel_flags_data[index] &= ~flag_value >> 24U;
            pixel_flags_data[index + 1UZ] &= ~flag_value >> 16U;
            pixel_flags_data[index + 2UZ] &= ~flag_value >> 8U;
            pixel_flags_data[index + 3UZ] &= ~flag_value;
        }
        else {
            pixel_flags_data[index] |= flag_value >> 24U;
            pixel_flags_data[index + 1UZ] |= flag_value >> 16U;
            pixel_flags_data[index + 2UZ] |= flag_value >> 8U;
            pixel_flags_data[index + 3UZ] |= flag_value;
        }
    }

    auto map_view::update_pixel_flags_texture() -> void {
        pixel_flags.update(pixel_flags_data.data(), {pixel_flags_dimensions.x, pixel_flags_dimensions.y}, {0U, 0U});
    }
}
