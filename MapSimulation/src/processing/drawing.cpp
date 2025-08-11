#include "drawing.hpp"
#include <array>
#include <imgui.h>
#include <ranges>
#include <thread>
#include "image.hpp"
#include "simulation.hpp"
#include "../mechanics/map_mode.hpp"
#include "../mechanics/province.hpp"
#include "../utils/zstring_view.hpp"

namespace {
    auto draw_date() -> void {
        static mechanics::date current_date;
        static std::string date_str = std::move(current_date.to_string());
        if (ImGui::BeginChild("Date", {200.0F, 50.0F})) {
            if (const auto &date = mechanics::data::instance().current_date(); date != current_date) {
                current_date = date;
                date_str = std::move(date.to_string());
            }
            ImGui::Text("Current Date: %s", date_str.c_str());
        }
        ImGui::EndChild();
    }

    auto draw_map_mode_selection(processing::simulation &sim) -> void {
        constexpr static std::array<utils::zstring_view, 7UZ> map_mode_names = {
            "Provinces", "Owner", "Koppen", "Elevation", "Vegetation", "Soil", "Sea"
        };
        static size_t current_item = static_cast<uint_fast8_t>(mechanics::map_mode_t::provinces);
        current_item = static_cast<uint_fast8_t>(mechanics::map_mode.load());
        if (ImGui::BeginChild("Map Mode", {200.0F, 50.0F})) {
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

    auto draw_selected_province_info(const processing::simulation &sim) -> void {
        const auto selected_province = sim.selected_province();
        if (!selected_province.has_value()) { return; }
        if (ImGui::BeginChild("Selected Province Info", {200.0F, 100.0F})) {
            const auto koppen_string = koppen_to_string(selected_province->get().koppen());
            const auto elevation_string = elevation_to_string(selected_province->get().elevation());
            const auto vegetation_string = vegetation_to_string(selected_province->get().vegetation());
            const auto soil_string = soil_to_string(selected_province->get().soil());
            const auto sea_string = sea_to_string(selected_province->get().sea());
            ImGui::TextWrapped("Province Color: %06X\n"
                "Id: %d\n"
                "Climate: %s\n"
                "Elevation: %s\n"
                "Vegetation: %s\n"
                "Soil: %s\n"
                "Sea: %s\n",
                selected_province->get().base_color(),
                selected_province->get().id(),
                koppen_string.str(),
                elevation_string.str(),
                vegetation_string.str(),
                soil_string.str(),
                sea_string.str());
        }
        ImGui::EndChild();
    }
}

namespace processing {
    drawing::drawing(simulation &simulation, const std::string &loading_text) : simulation_(simulation),
        loading_text_(loading_text) {}

    auto drawing::init_sprites(const image &map_image, const std::vector<uint8_t> &bytes,
        const std::vector<uint8_t> &crossing_bytes) -> bool {
        if (!map_shader_.loadFromFile("shaders/map.frag", sf::Shader::Type::Fragment)) { return false; }
        map_shader_.setUniform("texture", sf::Shader::CurrentTexture);
        map_shader_.setUniform("size", sf::Vector2f(static_cast<float_t>(map_image.width()),
            static_cast<float_t>(map_image.height())));
        map_shader_.setUniform("selectedIndex", -1);
        map_shader_.setUniform("drawOutline", draw_outline_);
        map_shader_.setUniform("dim", static_cast<float>(mechanics::province_colors.getSize().x));
        map_shader_.setUniform("provinceColors", mechanics::province_colors);
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

    auto drawing::recalculate_sprite_coords(const std::array<int_fast32_t, 2UZ> offset, const double_t zoom) -> void {
        const auto zoom_f = static_cast<float_t>(zoom);
        map_sprite_.setPosition(sf::Vector2f(static_cast<float_t>(offset[0UZ]), static_cast<float_t>(offset[1UZ])));
        map_sprite_.setScale(sf::Vector2f(zoom_f, zoom_f));
        crossing_sprite_.setPosition(sf::Vector2f(static_cast<float_t>(offset[0UZ]),
            static_cast<float_t>(offset[1UZ])));
        crossing_sprite_.setScale(sf::Vector2f(zoom_f, zoom_f));
    }

    auto drawing::draw_loading_message(sf::RenderWindow &) const -> void {
        ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F));
        ImGui::SetNextWindowSize(ImVec2(500.0F, 100.0F));
        if (ImGui::Begin("Loading")) { ImGui::TextWrapped("%s", loading_text_.c_str()); }
        ImGui::End();
    }

    auto drawing::draw_map(sf::RenderWindow &window) -> void {
        static int selected_index = -1;
        if (!simulation_.selected_province().has_value()) {
            if (constexpr auto new_index = -1; new_index != selected_index) {
                selected_index = new_index;
                map_shader_.setUniform("selectedIndex", new_index);
            }
        }
        else {
            if (const auto new_index = static_cast<int>(simulation_.selected_province()->get().id());
                new_index != selected_index) {
                selected_index = new_index;
                map_shader_.setUniform("selectedIndex", new_index);
            }
        }
        window.draw(map_sprite_, &map_shader_);
        if (draw_crossings_) { window.draw(crossing_sprite_, &crossing_shader_); }
        const auto offset = static_cast<float>(texture_.getSize().x) * map_sprite_.getScale().x;
        map_sprite_.move(sf::Vector2f(offset, 0.0F));
        crossing_sprite_.move(sf::Vector2f(offset, 0.0F));
        window.draw(map_sprite_, &map_shader_);
        if (draw_crossings_) { window.draw(crossing_sprite_, &crossing_shader_); }
        map_sprite_.move(sf::Vector2f(-offset, 0.0F));
        crossing_sprite_.move(sf::Vector2f(-offset, 0.0F));
    }

    auto drawing::draw_checkboxes() -> void {
        ImGui::Checkbox("Crossings", &draw_crossings_);
        if (ImGui::Checkbox("Outlines", &draw_outline_)) { map_shader_.setUniform("drawOutline", draw_outline_); }
    }

    auto drawing::draw_gui(const sf::RenderWindow &window) -> void {
        ImGui::SetNextWindowPos({0.0F, 0.0F});
        ImGui::SetNextWindowSize({200.0F, static_cast<float_t>(window.getSize().y)});
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
