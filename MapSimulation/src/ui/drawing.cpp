#include "drawing.hpp"
#include <imgui.h>
#include <ranges>
#include <array>
#include "../logic/image.hpp"
#include "../features/province.hpp"
#include "../logic/simulation.hpp"

drawing::drawing(simulation &simulation, const std::string &loading_text) : simulation_(simulation),
                                                                            loading_text_(loading_text) {
}

bool drawing::init_sprites(const image &map_image, const std::vector<uint8_t> &bytes) {
    if (!map_shader_.loadFromFile("shaders/map.frag", sf::Shader::Type::Fragment)) {
        return false;
    }
    map_shader_.setUniform("texture", texture_);
    map_shader_.setUniform("size", sf::Vector2f(static_cast<float_t>(map_image.width()),
                                                static_cast<float_t>(map_image.height())));
    if (!texture_.loadFromImage(
        sf::Image(sf::Vector2u(map_image.width(), map_image.height()), bytes.data()))) {
        return false;
    }
    map_sprite_.setTexture(texture_);
    map_sprite_.setTextureRect(sf::IntRect(sf::Vector2(0, 0),
                                           sf::Vector2(map_image.width(), map_image.height())));
    return true;
}

void drawing::recalculate_sprite_coords(const std::array<int_fast32_t, 2> offset, const double_t zoom,
                                        const uint_fast32_t map_width) {
    const auto zoom_f = static_cast<float_t>(zoom);
    map_sprite_.setPosition(sf::Vector2f(static_cast<float_t>(offset[0]), static_cast<float_t>(offset[1])));
    map_sprite_.setScale(sf::Vector2f(zoom_f, zoom_f));
}

void drawing::draw_loading_message(sf::RenderWindow &window) const {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(500, 100));
    if (ImGui::Begin("Loading")) {
        ImGui::TextWrapped("%s", loading_text_.c_str());
        ImGui::End();
    }
}

void drawing::draw_map(sf::RenderWindow &window) {
    window.draw(map_sprite_, &map_shader_);
    const auto offset = static_cast<float>(texture_.getSize().x) * map_sprite_.getScale().x;
    map_sprite_.move(sf::Vector2f(offset, 0));
    window.draw(map_sprite_, &map_shader_);
    map_sprite_.move(sf::Vector2f(-offset, 0));
}

void drawing::update_map_texture(const uint8_t *bytes) {
    texture_.update(bytes);
}

void drawing::update_map_texture(const uint8_t *bytes, const sf::Vector2u &dimensions,
                                 const sf::Vector2u &position) {
    texture_.update(bytes, dimensions, position);
}

inline void draw_map_mode_selection(simulation &sim) {
    constexpr static std::array<std::string, 7> map_mode_names = {
        "Provinces", "Owner", "Koppen", "Elevation", "Vegetation", "Soil", "Sea"
    };
    static auto current_item = static_cast<uint_fast8_t>(map_mode_t::provinces);
    current_item = static_cast<uint_fast8_t>(sim.map_mode());
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({200, 50});
    if (ImGui::BeginCombo("Map Mode", map_mode_names[current_item].c_str())) {
        for (size_t i = 0; i < map_mode_names.size(); ++i) {
            const bool is_selected = (current_item == i);
            if (ImGui::Selectable(map_mode_names[i].c_str(), is_selected)) {
                current_item = i;
                sim.change_map_mode(static_cast<map_mode_t>(i));
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

inline void draw_selected_province_info(const simulation &sim) {
    const auto selected_province = sim.selected_province();
    if (selected_province == nullptr) return;
    ImGui::SetNextWindowPos({0, 50});
    ImGui::SetNextWindowSize({200, 100});
    if (ImGui::Begin("Selected Province Info")) {
        ImGui::TextWrapped("Province Color: %06X\n"
                           "Climate: %s\n"
                           "Elevation: %s\n"
                           "Vegetation: %s\n"
                           "Soil: %s\n"
                           "Sea: %s\n",
                           selected_province->color(),
                           koppen_to_string(selected_province->koppen()).data(),
                           elevation_to_string(selected_province->elevation()).data(),
                           vegetation_to_string(selected_province->vegetation()).data(),
                           soil_to_string(selected_province->soil()).data(),
                           sea_to_string(selected_province->sea()).data());
        ImGui::End();
    }
}

void drawing::draw_gui(sf::RenderWindow &window) const {
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({200, static_cast<float_t>(window.getSize().y)});
    if (ImGui::Begin("GUI")) {
        if (ImGui::BeginChild("GUI")) {
            draw_map_mode_selection(simulation_);
            draw_selected_province_info(simulation_);
            ImGui::EndChild();
        }
        ImGui::End();
    }
}
