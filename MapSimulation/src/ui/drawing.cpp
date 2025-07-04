#include "drawing.hpp"
#include <imgui.h>
#include <ranges>
#include <array>

#include "../logic/image.hpp"
#include "../logic/data.hpp"
#include "../features/base/province.hpp"
#include "../logic/simulation.hpp"

drawing::drawing(data &data, const double &progress, simulation *const simulation) : data_(data),
    progress_(progress),
    simulation_(*simulation) {
}

bool drawing::init_sprites(const image &map_image, const std::vector<unsigned char> &bytes) {
    if (!texture_.loadFromImage(
        sf::Image(sf::Vector2u(map_image.width(), map_image.height()), bytes.data()))) {
        return false;
    }
    map_sprite_one_.setTexture(texture_);
    map_sprite_one_.setTextureRect(sf::IntRect(sf::Vector2(0, 0),
                                               sf::Vector2(map_image.width(), map_image.height())));
    map_sprite_two_.setTexture(texture_);
    map_sprite_two_.setTextureRect(sf::IntRect(sf::Vector2(0, 0),
                                               sf::Vector2(map_image.width(), map_image.height())));
    return true;
}

void drawing::recalculate_sprite_coords(const std::array<int, 2> offset, const double zoom,
                                        const unsigned int map_width) {
    const auto zoom_f = static_cast<float>(zoom);
    map_sprite_one_.setPosition(sf::Vector2f(static_cast<float>(offset[0]), static_cast<float>(offset[1])));
    map_sprite_one_.setScale(sf::Vector2f(zoom_f, zoom_f));
    map_sprite_two_.setPosition(sf::Vector2f(static_cast<float>(offset[0] + static_cast<int>(map_width * zoom)),
                                             static_cast<float>(offset[1])));
    map_sprite_two_.setScale(sf::Vector2f(zoom_f, zoom_f));
}

void drawing::draw_loading_screen(sf::RenderWindow &window) const {
    sf::RectangleShape progress_bar(sf::Vector2f(800, 50));

    progress_bar.setFillColor(sf::Color::Green);
    progress_bar.setPosition(sf::Vector2f(0, 275));
    progress_bar.setSize(sf::Vector2f(800 * static_cast<float>(progress_), 50));

    window.draw(progress_bar);
}

void drawing::draw_map(sf::RenderWindow &window) const {
    window.draw(map_sprite_one_);
    window.draw(map_sprite_two_);
}

void drawing::update_map_texture(const unsigned char *bytes) {
    texture_.update(bytes);
}

void drawing::update_map_texture(const unsigned char *bytes, const sf::Vector2u &dimensions,
                                 const sf::Vector2u &position) {
    texture_.update(bytes, dimensions, position);
}

inline void draw_map_mode_selection(simulation &sim, const data &d) {
    constexpr static std::array<std::string, 7> map_mode_names = {
        "Provinces", "Owner", "Koppen", "Elevation", "Vegetation", "Soil", "Sea"
    };
    static int current_item = static_cast<int>(map_modes::provinces);
    current_item = static_cast<int>(d.map_mode);
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({200, 50});
    if (ImGui::BeginCombo("Map Mode", map_mode_names[current_item].c_str())) {
        for (int i = 0; i < map_mode_names.size(); ++i) {
            const bool is_selected = (current_item == i);
            if (ImGui::Selectable(map_mode_names[i].c_str(), is_selected)) {
                current_item = i;
                sim.change_map_mode(static_cast<map_modes>(i));
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

inline void draw_selected_province_info(const simulation &sim, const data &d) {
    if (d.selected_province == nullptr) return;
    ImGui::SetNextWindowPos({0, 50});
    ImGui::SetNextWindowSize({200, 100});
    if (ImGui::Begin(d.selected_province->name().data())) {
        ImGui::TextWrapped("Province Color: %06X\n"
                           "Climate: %s\n"
                           "Elevation: %s\n"
                           "Vegetation: %s\n"
                           "Soil: %s\n"
                           "Sea: %s\n",
                           d.selected_province->color(),
                           koppen_to_string(d.selected_province->koppen()).data(),
                           elevation_to_string(d.selected_province->elevation()).data(),
                           vegetation_to_string(d.selected_province->vegetation()).data(),
                           soil_to_string(d.selected_province->soil()).data(),
                           sea_to_string(d.selected_province->sea()).data());
        ImGui::End();
    }
}

void drawing::draw_gui(sf::RenderWindow &window) const {
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({200, static_cast<float>(window.getSize().y)});
    if (ImGui::Begin("GUI")) {
        if (ImGui::BeginChild("GUI")) {
            draw_map_mode_selection(simulation_, data_);
            draw_selected_province_info(simulation_, data_);
            ImGui::EndChild();
        }
        ImGui::End();
    }
}
