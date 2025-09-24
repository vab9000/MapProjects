#include "drawing.hpp"
#include <array>
#include <format>
#include <imgui.h>
#include <ranges>
#include <thread>
#include <zstring_view.hpp>
#include "image.hpp"
#include "simulation.hpp"
#include "../mechanics/map_mode.hpp"
#include "../mechanics/province.hpp"

namespace {
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

    auto draw_map_mode_selection(processing::simulation &sim) -> void {
        constexpr static std::array<utils::zstring_view, 8UZ> map_mode_names = {
            "Provinces", "Owner", "Koppen", "Elevation", "Vegetation", "Soil", "Sea", "River"
        };
        static size_t current_item = static_cast<unsigned char>(mechanics::map_mode_t::provinces);
        current_item = static_cast<unsigned char>(mechanics::map_mode.load());
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

    auto province_string(const mechanics::province &p) {
        const auto color = p.base_color();
        const auto koppen_string = koppen_to_string(p.koppen()).str();
        if (p.sea() == mechanics::sea_t::none) {
            constexpr auto format_string = "Land\n"
                "Color: {:X}\n"
                "Climate: {}\n"
                "Elevation: {}\n"
                "Vegetation: {}\n"
                "Soil: {}\n";
            const auto elevation_string = elevation_to_string(p.elevation()).str();
            const auto vegetation_string = vegetation_to_string(p.vegetation()).str();
            const auto soil_string = soil_to_string(p.soil()).str();
            return std::vformat(format_string, std::make_format_args(color,
                koppen_string,
                elevation_string,
                vegetation_string,
                soil_string));
        }
        if (p.sea() == mechanics::sea_t::lake) {
            constexpr auto format_string = "Lake\n"
                "Color: {:X}\n"
                "Climate: {}\n";
            return std::vformat(format_string, std::make_format_args(color, koppen_string));
        }
        if (p.sea() == mechanics::sea_t::river) {
            const auto river_size = static_cast<unsigned int>(p.value());
            constexpr auto format_string = "River\n"
                "Color: {:X}\n"
                "River Size: {}\n"
                "Climate: {}\n";
            return std::vformat(format_string, std::make_format_args(color, river_size, koppen_string));
        }
        constexpr auto format_string = "Sea\n"
            "Color: {:X}\n"
            "Wind Type: {}\n"
            "Climate: {}\n";
        const auto sea_string = sea_to_string(p.sea()).str();
        return std::vformat(format_string, std::make_format_args(color, sea_string, koppen_string));
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
        map_shader_.setUniform("dim", static_cast<float>(mechanics::province_colors.getSize().x));
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
        flags_shader_.setUniform("tex_size", sf::Glsl::Vec2{static_cast<float>(map_image.width()), static_cast<float>(map_image.height())});
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
