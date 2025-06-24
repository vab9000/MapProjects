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
        sf::Image(sf::Vector2u(map_image.get_width(), map_image.get_height()), bytes.data()))) {
        return false;
    }
    map_sprite_one_.setTexture(texture_);
    map_sprite_one_.setTextureRect(sf::IntRect(sf::Vector2(0, 0),
                                               sf::Vector2(map_image.get_width(), map_image.get_height())));
    map_sprite_two_.setTexture(texture_);
    map_sprite_two_.setTextureRect(sf::IntRect(sf::Vector2(0, 0),
                                               sf::Vector2(map_image.get_width(), map_image.get_height())));
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

void drawing::draw_selected_province_gui(sf::RenderWindow &window) const {
    if (data_.selected_province == nullptr) return;

    auto bounds = data_.selected_province->get_bounds();
    simulation_.transform_to_screen_coordinates(bounds);
    ImGui::SetNextWindowPos(
        ImVec2(static_cast<float>(bounds[0] + bounds[2]) / 2.0f,
               static_cast<float>(bounds[1] + bounds[3]) / 2.0f));
    if (ImGui::Begin(data_.selected_province->name().data())) {
        ImGui::Text("Province Color: %06X", data_.selected_province->color);
        ImGui::End();
    }
}

void drawing::draw_map_mode_selector(sf::RenderWindow &window) const {
    constexpr std::array<std::string, 5> map_mode_names = {
        "Provinces", "Owner", "Koppen", "Elevation", "Vegetation"
    };
    static int current_item = static_cast<int>(map_modes::provinces);
    current_item = static_cast<int>(data_.map_mode);
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    if (ImGui::BeginCombo("Map Mode", map_mode_names[current_item].c_str())) {
        for (int i = 0; i < map_mode_names.size(); ++i) {
            const bool is_selected = (current_item == i);
            if (ImGui::Selectable(map_mode_names[i].c_str(), is_selected)) {
                current_item = i;
                simulation_.change_map_mode(static_cast<map_modes>(i));
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}
