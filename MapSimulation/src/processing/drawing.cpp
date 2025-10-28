#include "drawing.hpp"
#include <array>
#include <format>
#include <imgui.h>
#include <ranges>
#include <zstring_view.hpp>
#include "image.hpp"
#include "simulation.hpp"
#include "../mechanics/map_mode.hpp"
#include "../mechanics/province/province.hpp"

namespace {
    auto koppen_to_string(const mechanics::koppen_t value) -> utils::zstring_view {
        switch (value) {
            case mechanics::koppen_t::none: return "None";
            case mechanics::koppen_t::af: return "Tropical rainforest";
            case mechanics::koppen_t::am: return "Tropical monsoon";
            case mechanics::koppen_t::aw: return "Tropical savannah";
            case mechanics::koppen_t::bwh: return "Hot desert";
            case mechanics::koppen_t::bwk: return "Cold desert";
            case mechanics::koppen_t::bsh: return "Hot semi-arid";
            case mechanics::koppen_t::bsk: return "Cold semi-arid";
            case mechanics::koppen_t::csa: return "Hot summer Mediterranean";
            case mechanics::koppen_t::csb: return "Warm summer Mediterranean";
            case mechanics::koppen_t::csc: return "Cold summer Mediterranean";
            case mechanics::koppen_t::cwa: return "Monsoon-influenced humid subtropical";
            case mechanics::koppen_t::cwb: return "Monsoon-influenced oceanic";
            case mechanics::koppen_t::cwc: return "Monsoon-influenced subpolar";
            case mechanics::koppen_t::cfa: return "Humid subtropical";
            case mechanics::koppen_t::cfb: return "Oceanic";
            case mechanics::koppen_t::cfc: return "Subpolar oceanic";
            case mechanics::koppen_t::dsa: return "Mediterranean-influenced hot summer humid continental";
            case mechanics::koppen_t::dsb: return "Mediterranean-influenced warm summer humid continental";
            case mechanics::koppen_t::dsc: return "Mediterranean-influenced subarctic";
            case mechanics::koppen_t::dsd: return "Mediterranean-influenced cold subarctic";
            case mechanics::koppen_t::dwa: return "Monsoon-influenced hot summer humid continental";
            case mechanics::koppen_t::dwb: return "Monsoon-influenced warm summer humid continental";
            case mechanics::koppen_t::dwc: return "Monsoon-influenced subarctic";
            case mechanics::koppen_t::dwd: return "Monsoon-influenced cold subarctic";
            case mechanics::koppen_t::dfa: return "Hot summer humid continental";
            case mechanics::koppen_t::dfb: return "Warm summer humid continental";
            case mechanics::koppen_t::dfc: return "Subarctic";
            case mechanics::koppen_t::dfd: return "Cold subarctic";
            case mechanics::koppen_t::et: return "Tundra";
            case mechanics::koppen_t::ef: return "Ice cap";
        }
        return "No koppen type";
    }

    auto vegetation_to_string(const mechanics::vegetation_t value) -> utils::zstring_view {
        switch (value) {
            case mechanics::vegetation_t::none: return "None";
            case mechanics::vegetation_t::tropical_evergreen_broadleaf_forest: return
                    "Tropical Evergreen Broadleaf Forest";
            case mechanics::vegetation_t::tropical_semi_evergreen_broadleaf_forest: return
                    "Tropical Semi-Evergreen Broadleaf Forest";
            case mechanics::vegetation_t::tropical_deciduous_broadleaf_forest_and_woodland: return
                    "Tropical Deciduous Broadleaf Forest and Woodland";
            case mechanics::vegetation_t::warm_temperate_evergreen_and_mixed_forest: return
                    "Warm Temperate Evergreen and Mixed Forest";
            case mechanics::vegetation_t::cool_temperate_rainforest: return "Cool Temperate Rainforest";
            case mechanics::vegetation_t::cool_evergreen_needleleaf_forest: return "Cool Evergreen Needleleaf Forest";
            case mechanics::vegetation_t::cool_mixed_forest: return "Cool Mixed Forest";
            case mechanics::vegetation_t::temperate_deciduous_broadleaf_forest: return
                    "Temperate Deciduous Broadleaf Forest";
            case mechanics::vegetation_t::cold_deciduous_forest: return "Cold Deciduous Forest";
            case mechanics::vegetation_t::cold_evergreen_needleleaf_forest: return "Cold Evergreen Needleleaf Forest";
            case mechanics::vegetation_t::temperate_sclerophyll_woodland_and_shrubland: return
                    "Temperate Sclerophyll Woodland and Shrubland";
            case mechanics::vegetation_t::temperate_evergreen_needleleaf_open_woodland: return
                    "Temperate Evergreen Needleleaf Open Woodland";
            case mechanics::vegetation_t::tropical_savanna: return "Tropical Savanna";
            case mechanics::vegetation_t::xerophytic_woods_scrub: return "Xerophytic Woods Scrub";
            case mechanics::vegetation_t::steppe: return "Steppe";
            case mechanics::vegetation_t::desert: return "Desert";
            case mechanics::vegetation_t::graminoid_and_forb_tundra: return "Graminoid and Forb Tundra";
            case mechanics::vegetation_t::erect_dwarf_shrub_tundra: return "Erect Dwarf Shrub Tundra";
            case mechanics::vegetation_t::low_and_high_shrub_tundra: return "Low and High Shrub Tundra";
            case mechanics::vegetation_t::prostrate_dwarf_shrub_tundra: return "Prostrate Dwarf Shrub Tundra";
        }
        return "No vegetation type";
    }

    inline auto soil_to_string(const mechanics::soil_t value) -> utils::zstring_view {
        switch (value) {
            case mechanics::soil_t::none: return "None";
            case mechanics::soil_t::acrisols: return "Acrisols";
            case mechanics::soil_t::albeluvisols: return "Albeluvisols";
            case mechanics::soil_t::alisols: return "Alisols";
            case mechanics::soil_t::andosols: return "Andosols";
            case mechanics::soil_t::arenosols: return "Arenosols";
            case mechanics::soil_t::calcisols: return "Calcisols";
            case mechanics::soil_t::cambisols: return "Cambisols";
            case mechanics::soil_t::chernozems: return "Chernozems";
            case mechanics::soil_t::cryosols: return "Cryosols";
            case mechanics::soil_t::durisols: return "Durisols";
            case mechanics::soil_t::ferrasols: return "Ferrasols";
            case mechanics::soil_t::fluvisols: return "Fluvisols";
            case mechanics::soil_t::gleysols: return "Gleysols";
            case mechanics::soil_t::gypsisols: return "Gypsisols";
            case mechanics::soil_t::histosols: return "Histosols";
            case mechanics::soil_t::kastanozems: return "Kastanozems";
            case mechanics::soil_t::leptosols: return "Leptosols";
            case mechanics::soil_t::lixisols: return "Lixisols";
            case mechanics::soil_t::luvisols: return "Luvisols";
            case mechanics::soil_t::nitisols: return "Nitisols";
            case mechanics::soil_t::phaeozems: return "Phaeozems";
            case mechanics::soil_t::planosols: return "Planosols";
            case mechanics::soil_t::plinthosols: return "Plinthosols";
            case mechanics::soil_t::podzols: return "Podzols";
            case mechanics::soil_t::regosols: return "Regosols";
            case mechanics::soil_t::solonchaks: return "Solonchaks";
            case mechanics::soil_t::solonetz: return "Solonetz";
            case mechanics::soil_t::stagnosols: return "Stagnosols";
            case mechanics::soil_t::umbrisols: return "Umbrisols";
            case mechanics::soil_t::vertisols: return "Vertisols";
        }
        return "No soil type";
    }

    auto elevation_to_string(const mechanics::elevation_level_t value) -> utils::zstring_view {
        switch (value) {
            case mechanics::elevation_level_t::lowland: return "Lowland";
            case mechanics::elevation_level_t::midland: return "Midland";
            case mechanics::elevation_level_t::highland: return "Highland";
        }
        return "No elevation level";
    }

    auto roughness_to_string(const mechanics::roughness_t value) -> utils::zstring_view {
        switch (value) {
            case mechanics::roughness_t::flat: return "Flat";
            case mechanics::roughness_t::hilly: return "Hilly";
            case mechanics::roughness_t::mountainous: return "Mountainous";
        }
        return "No roughness type";
    }

    auto sea_to_string(const mechanics::sea_t value) -> utils::zstring_view {
        switch (value) {
            case mechanics::sea_t::coast: return "Coast";
            case mechanics::sea_t::lake: return "Lake";
            case mechanics::sea_t::polar: return "Polar";
            case mechanics::sea_t::northeasterly: return "Northeasterly";
            case mechanics::sea_t::southeasterly: return "Southeasterly";
            case mechanics::sea_t::sea: return "Sea";
            case mechanics::sea_t::westerly: return "Westerly";
            case mechanics::sea_t::doldrums: return "Doldrums";
        }
        return "No sea type";
    }

    auto draw_date() -> void {
        static mechanics::date current_date;
        static std::string date_str = std::move(current_date.to_string());
        if (ImGui::BeginChild("Date", {200.0F, 50.0F}, ImGuiChildFlags_AutoResizeY)) {
            if (const auto &date = mechanics::data::instance().current_date(); date != current_date) {
                current_date = date;
                date_str = std::move(date.to_string());
            }
            ImGui::Text("Current Date: %s", date_str.c_str());
        }
        ImGui::EndChild();
    }

    constexpr auto map_mode_names_array() {
        using namespace utils;
        return std::array{
            "Provinces"_zsv, "Owner"_zsv, "Koppen"_zsv, "Elevation"_zsv, "Roughness"_zsv, "Vegetation"_zsv, "Soil"_zsv,
            "Sea"_zsv, "River Size"_zsv
        };
    }

    auto draw_map_mode_selection(processing::simulation &sim) -> void {
        constexpr static std::array map_mode_names = map_mode_names_array();
        static size_t current_item = static_cast<unsigned char>(mechanics::map_mode_t::provinces);
        current_item = static_cast<unsigned char>(mechanics::map_mode);
        if (ImGui::BeginChild("Map Mode", {250.0F, 50.0F})) {
            if (ImGui::BeginCombo("Map Mode", map_mode_names[current_item].str())) {
                for (auto i = 0UZ; i < map_mode_names.size(); ++i) {
                    const bool is_selected = current_item == i;
                    if (ImGui::Selectable(map_mode_names[i].str(), is_selected)) {
                        current_item = i;
                        mechanics::map_mode = static_cast<mechanics::map_mode_t>(i);
                        mechanics::update_all_province_colors();
                    }
                    if (is_selected) { ImGui::SetItemDefaultFocus(); }
                }
                ImGui::EndCombo();
            }
        }
        ImGui::EndChild();
    }

    auto province_string(const mechanics::province &p) -> std::string {
        const auto color = p.base_color();
        if (p.type() == mechanics::location_type_t::province) {
            constexpr auto format_string = "Land\n"
                "Color: {:X}\n"
                "Koppen: {}\n"
                "Elevation: {} {}\n"
                "Vegetation: {}\n"
                "Soil: {}";
            const auto [elevation, roughness, koppen, vegetation, soil] = p.properties<
                mechanics::location_type_t::province>();
            const auto koppen_string = koppen_to_string(koppen).str();
            const auto roughness_string = roughness_to_string(roughness).str();
            const auto elevation_string = elevation_to_string(elevation).str();
            const auto vegetation_string = vegetation_to_string(vegetation).str();
            const auto soil_string = soil_to_string(soil).str();
            return std::format(format_string, color,
                koppen_string,
                roughness_string, elevation_string,
                vegetation_string,
                soil_string);
        }
        if (p.type() == mechanics::location_type_t::river) {
            const auto [koppen, width] = p.properties<mechanics::location_type_t::river>();
            const auto river_size = static_cast<unsigned int>(width);
            constexpr auto format_string = "River\n"
                "Color: {:X}\n"
                "Koppen: {}\n"
                "River Size: {}";
            const auto koppen_string = koppen_to_string(koppen).str();
            return std::format(format_string, color,
                koppen_string,
                river_size);
        }
        const auto [sea_type, koppen] = p.properties<mechanics::location_type_t::sea>();
        const auto sea_string = sea_to_string(sea_type).str();
        if (sea_type == mechanics::sea_t::coast ||
            sea_type == mechanics::sea_t::lake) {
            constexpr auto format_string = "Sea\n"
                "Color: {:X}\n"
                "Koppen: {}\n"
                "Type: {}";
            const auto koppen_string = koppen_to_string(koppen).str();
            return std::format(format_string, color,
                koppen_string,
                sea_string);
        }
        constexpr auto format_string = "Sea\n"
            "Color: {:X}\n"
            "Wind Type: {}";
        return std::format(format_string, color, sea_string);
    }

    auto draw_hovered_province_info(const processing::simulation &sim) -> void {
        const auto hovered_province = sim.hovered_province();
        if (hovered_province == nullptr) { return; }
        const auto info = province_string(*hovered_province);
        auto size = ImGui::CalcTextSize(info.c_str(), nullptr, false, 200.0F);
        size = {size.x * 1.3F, size.y * 1.3F};
        ImGui::SetNextWindowSize(size);
        auto window_pos = ImGui::GetMousePos();
        const auto available_region = ImGui::GetIO().DisplaySize;
        if (window_pos.x + size.x > available_region.x) { window_pos.x = available_region.x - size.x; }
        if (window_pos.y + size.y > available_region.y) { window_pos.y = available_region.y - size.y; }
        if (window_pos.x < 0) { window_pos.x = 0; }
        if (window_pos.y < 0) { window_pos.y = 0; }
        ImGui::SetNextWindowPos(window_pos);
        if (ImGui::Begin("Selected Province", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoScrollbar)) {
            ImGui::TextWrapped(info.c_str());
        }
        ImGui::End();
    }
}

namespace processing {
    drawing::drawing(simulation &simulation, const std::string &loading_text) : simulation_(simulation),
        loading_text_(loading_text), flags_sprite_{mechanics::pixel_flags} {}

    auto drawing::init_sprites(const image &map_image, const std::vector<unsigned char> &bytes) -> bool {
        if (!map_shader_.loadFromFile("shaders/map.vert", "shaders/map.frag")) { return false; }
        map_shader_.setUniform("tex", sf::Shader::CurrentTexture);
        map_shader_.setUniform("size", sf::Vector2f(static_cast<float>(map_image.width()),
            static_cast<float>(map_image.height())));
        map_shader_.setUniform("selected_index", -1);
        map_shader_.setUniform("draw_outline", draw_outline_);
        map_shader_.setUniform("dim", static_cast<int>(mechanics::province_colors.getSize().x));
        map_shader_.setUniform("province_colors", mechanics::province_colors);

        if (!map_texture_.loadFromImage(
            sf::Image(sf::Vector2u(map_image.width(), map_image.height()), bytes.data()))) { return false; }
        map_sprite_.setTexture(map_texture_);
        map_sprite_.setTextureRect({
            {0, 0},
            {static_cast<int>(map_image.width()), static_cast<int>(map_image.height())}
        });

        flags_sprite_.setTexture(mechanics::pixel_flags);
        flags_sprite_.setTextureRect({
            {0, 0},
            {static_cast<int>(map_image.width()), static_cast<int>(map_image.height())}
        });

        if (!flags_shader_.loadFromFile("shaders/pixel_flags.vert", "shaders/pixel_flags.frag")) { return false; }
        flags_shader_.setUniform("border_mask", sf::Shader::CurrentTexture);
        flags_shader_.setUniform("draw_crossings", draw_crossings_);
        flags_shader_.setUniform("draw_bridges", draw_bridges_);
        flags_shader_.setUniform("tex_size", sf::Glsl::Vec2{
            static_cast<float>(map_image.width()), static_cast<float>(map_image.height())
        });
        return true;
    }

    auto drawing::recalculate_sprite_coords(const std::array<int, 2UZ> offset, const double zoom) -> void {
        const auto zoom_f = static_cast<float>(zoom);
        map_sprite_.setPosition(sf::Vector2f(static_cast<float>(offset[0UZ]), static_cast<float>(offset[1UZ])));
        map_sprite_.setScale(sf::Vector2f(zoom_f, zoom_f));
        flags_sprite_.setPosition(sf::Vector2f(static_cast<float>(offset[0UZ]),
            static_cast<float>(offset[1UZ])));
        flags_sprite_.setScale(sf::Vector2f(zoom_f, zoom_f));
    }

    auto drawing::draw_loading_message(sf::RenderWindow &) const -> void {
        ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F));
        ImGui::SetNextWindowSize(ImVec2(500.0F, 100.0F));
        if (ImGui::Begin("Loading")) { ImGui::TextWrapped("%s", loading_text_.c_str()); }
        ImGui::End();
    }

    auto drawing::draw_map(sf::RenderWindow &window) -> void {
        static int selected_index = -1;
        if (simulation_.selected_province() == nullptr) {
            if (constexpr auto new_index = -1; new_index != selected_index) {
                selected_index = new_index;
                map_shader_.setUniform("selected_index", new_index);
            }
        }
        else {
            if (const auto new_index = static_cast<int>(simulation_.selected_province()->id());
                new_index != selected_index) {
                selected_index = new_index;
                map_shader_.setUniform("selected_index", new_index);
            }
        }
        window.draw(map_sprite_, &map_shader_);
        window.draw(flags_sprite_, &flags_shader_);
        const auto offset = static_cast<float>(map_texture_.getSize().x) * map_sprite_.getScale().x;
        map_sprite_.move(sf::Vector2f(offset, 0.0F));
        flags_sprite_.move(sf::Vector2f(offset, 0.0F));
        window.draw(map_sprite_, &map_shader_);
        window.draw(flags_sprite_, &flags_shader_);
        map_sprite_.move(sf::Vector2f(-offset, 0.0F));
        flags_sprite_.move(sf::Vector2f(-offset, 0.0F));
    }

    auto drawing::draw_checkboxes() -> void {
        if (ImGui::BeginChild("Checkboxes", {200.0F, 50.0F},
            ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize)) {
            if (ImGui::Checkbox("Crossings", &draw_crossings_)) {
                flags_shader_.setUniform("draw_crossings", draw_crossings_);
            }
            if (ImGui::Checkbox("Outlines", &draw_outline_)) { map_shader_.setUniform("draw_outline", draw_outline_); }
            if (ImGui::Checkbox("Bridges", &draw_bridges_)) { flags_shader_.setUniform("draw_bridges", draw_bridges_); }
        }
        ImGui::EndChild();
    }

    auto drawing::draw_gui(const sf::RenderWindow &window) -> void {
        ImGui::SetNextWindowPos({0.0F, 0.0F});
        ImGui::SetNextWindowSize({250.0F, static_cast<float>(window.getSize().y)});
        if (ImGui::Begin("GUI", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoScrollbar)) {
            draw_date();
            ImGui::Spacing();
            draw_checkboxes();
            ImGui::Spacing();
            draw_map_mode_selection(simulation_);
            ImGui::Spacing();
        }
        ImGui::End();
        draw_hovered_province_info(simulation_);
    }
}
