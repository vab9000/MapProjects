#include "drawing.hpp"
#include <array>
#include <imgui.h>
#include <ranges>
#include <thread>
#include "image.hpp"
#include "simulation.hpp"
#include "../mechanics/province.hpp"

namespace {
    auto draw_date() -> void {
        static mechanics::date current_date;
        static std::string date_str = std::move(current_date.to_string());
        if (ImGui::BeginChild("Date", {200, 50})) {
            if (const auto &date = mechanics::data::instance().current_date(); date != current_date) {
                current_date = date;
                date_str = std::move(date.to_string());
            }
            ImGui::Text("Current Date: %s", date_str.c_str());
        }
        ImGui::EndChild();
    }

    auto draw_map_mode_selection(processing::simulation &sim) -> void {
        constexpr static std::array<std::string_view, 7> map_mode_names = {
            "Provinces", "Owner", "Koppen", "Elevation", "Vegetation", "Soil", "Sea"
        };
        static auto current_item = static_cast<uint_fast8_t>(mechanics::map_mode_t::provinces);
        current_item = static_cast<uint_fast8_t>(sim.map_mode());
        if (ImGui::BeginChild("Map Mode", {200, 50})) {
            if (ImGui::BeginCombo("Map Mode", map_mode_names[current_item].data())) {
                for (size_t i = 0; i < map_mode_names.size(); ++i) {
                    const bool is_selected = current_item == i;
                    if (ImGui::Selectable(map_mode_names[i].data(), is_selected)) {
                        current_item = i;
                        std::thread update_map(&processing::simulation::change_map_mode, &sim,
                                               static_cast<mechanics::map_mode_t>(i));
                        update_map.detach();
                    }
                    if (is_selected) { ImGui::SetItemDefaultFocus(); }
                }
                ImGui::EndCombo();
            }
        }
        ImGui::EndChild();
    }

    auto draw_selected_province_info(const processing::simulation &sim) -> void {
        const auto selected_province = sim.selected_province();
        if (selected_province == nullptr) { return; }
        if (ImGui::BeginChild("Selected Province Info", {200, 100})) {
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
        }
        ImGui::EndChild();
    }
}

namespace processing {
    drawing::drawing(simulation &simulation, const std::string &loading_text) : simulation_(simulation),
        loading_text_(loading_text) {}

    auto drawing::init_sprites(const image &map_image, const std::vector<uint8_t> &bytes,
                               const std::vector<uint8_t> &crossing_bytes) -> bool {
        if (!base_map_.loadFromImage({sf::Vector2u(map_image.width(), map_image.height()), bytes.data()})) {
            return false;
        }
        if (!map_shader_.loadFromFile("shaders/map.frag", sf::Shader::Type::Fragment)) { return false; }
        map_shader_.setUniform("texture", sf::Shader::CurrentTexture);
        map_shader_.setUniform("size", sf::Vector2f(static_cast<float_t>(map_image.width()),
                                                    static_cast<float_t>(map_image.height())));
        map_shader_.setUniform("baseMap", base_map_);
        map_shader_.setUniform("selectedColor", sf::Glsl::Vec4(sf::Color::White));
        if (!texture_.loadFromImage(
            sf::Image(sf::Vector2u(map_image.width(), map_image.height()), bytes.data()))) { return false; }
        map_sprite_.setTexture(texture_);
        map_sprite_.setTextureRect({
            {0, 0},
            {static_cast<int>(map_image.width()), static_cast<int>(map_image.height())}
        });
        if (!crossing_texture_.loadFromImage({
            sf::Vector2u(map_image.width(), map_image.height()), crossing_bytes.data()
        })) { return false; }
        crossing_sprite_.setTexture(crossing_texture_);
        crossing_sprite_.setTextureRect({
            {0, 0},
            {static_cast<int>(map_image.width()), static_cast<int>(map_image.height())}
        });
        if (!crossing_shader_.loadFromFile("shaders/crossings.frag", sf::Shader::Type::Fragment)) { return false; }
        crossing_shader_.setUniform("borderMask", sf::Shader::CurrentTexture);
        return true;
    }

    auto drawing::recalculate_sprite_coords(const std::array<int_fast32_t, 2> offset, const double_t zoom) -> void {
        const auto zoom_f = static_cast<float_t>(zoom);
        map_sprite_.setPosition(sf::Vector2f(static_cast<float_t>(offset[0]), static_cast<float_t>(offset[1])));
        map_sprite_.setScale(sf::Vector2f(zoom_f, zoom_f));
        crossing_sprite_.setPosition(sf::Vector2f(static_cast<float_t>(offset[0]), static_cast<float_t>(offset[1])));
        crossing_sprite_.setScale(sf::Vector2f(zoom_f, zoom_f));
    }

    auto drawing::draw_loading_message(sf::RenderWindow &) const -> void {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(500, 100));
        if (ImGui::Begin("Loading")) { ImGui::TextWrapped("%s", loading_text_.c_str()); }
        ImGui::End();
    }

    auto drawing::draw_map(sf::RenderWindow &window) -> void {
        static sf::Color selected_color = sf::Color::White;
        if (simulation_.selected_province() == nullptr) {
            if (constexpr auto new_color = sf::Color::White; new_color != selected_color) {
                selected_color = new_color;
                map_shader_.setUniform("selectedColor", sf::Glsl::Vec4(selected_color));
            }
        } else {
            if (const auto new_color = sf::Color{utils::flip_rb(simulation_.selected_province()->base_color()) << 8};
                new_color != selected_color) {
                selected_color = new_color;
                map_shader_.setUniform("selectedColor", sf::Glsl::Vec4(selected_color));
            }
        }
        window.draw(map_sprite_, &map_shader_);
        if (draw_crossings_) { window.draw(crossing_sprite_, &crossing_shader_); }
        const auto offset = static_cast<float>(texture_.getSize().x) * map_sprite_.getScale().x;
        map_sprite_.move(sf::Vector2f(offset, 0));
        crossing_sprite_.move(sf::Vector2f(offset, 0));
        window.draw(map_sprite_, &map_shader_);
        if (draw_crossings_) { window.draw(crossing_sprite_, &crossing_shader_); }
        map_sprite_.move(sf::Vector2f(-offset, 0));
        crossing_sprite_.move(sf::Vector2f(-offset, 0));
    }

    auto drawing::update_map_texture(const uint8_t *bytes) -> void { texture_.update(bytes); }

    auto drawing::update_map_texture(const uint8_t *bytes, const sf::Vector2u &dimensions,
                                     const sf::Vector2u &position) -> void {
        texture_.update(bytes, dimensions, position);
    }

    auto drawing::draw_checkboxes() -> void { ImGui::Checkbox("Crossings", &draw_crossings_); }

    auto drawing::draw_gui(const sf::RenderWindow &window) -> void {
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize({200, static_cast<float_t>(window.getSize().y)});
        if (ImGui::Begin("GUI")) {
            draw_date();
            ImGui::Spacing();
            draw_checkboxes();
            ImGui::Spacing();
            draw_map_mode_selection(simulation_);
            ImGui::Spacing();
            draw_selected_province_info(simulation_);
        }
        ImGui::End();
    }
}
