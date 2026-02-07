#include "control.hpp"
#define GLFW_INCLUDE_NONE
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <format>
#include <imgui.h>
#include <limits>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include <zstring_view.hpp>
#include <GLFW/glfw3.h>
#include <misc/cpp/imgui_stdlib.h>
#include "location.hpp"
#include "magic_enum.hpp"
#include "saver.hpp"
#include "../graphics/image.hpp"
#include "../graphics/paletted_texture.hpp"
#include "../graphics/sprite.hpp"
#include "../graphics/window.hpp"

namespace logic {
    static constexpr auto max_color_f = 255.0F;

    static auto fill(const int canvas_x, const int canvas_y, graphics::paletted_texture &canvas,
        const size_t current_location) -> void {
        struct segment {
            int y;
            int x1, x2;
        };

        std::vector<segment> segments;

        const int seed_x = canvas_x;
        const int seed_y = canvas_y;
        const auto target_idx = canvas.index({seed_x, seed_y});

        if (target_idx == current_location) { return; }

        std::vector<unsigned char> visited(control::width * control::height, 0);

        auto is_target = [&](const int col, const int row) -> bool { return canvas.index({col, row}) == target_idx; };

        auto mark_visited = [&](const int col, const int row) -> void { visited.at(row * control::width + col) = 1; };

        auto was_visited = [&](const int col, const int row) -> bool {
            return visited.at(row * control::width + col) != 0;
        };

        std::vector<std::pair<int, int>> stack;
        stack.emplace_back(seed_x, seed_y);

        while (!stack.empty()) {
            auto [col, row] = stack.back();
            stack.pop_back();

            if (col < 0 || std::cmp_greater_equal(col, control::width) || row < 0 || std::cmp_greater_equal(row,
                    control::height) || was_visited(col, row)) { continue; }

            if (!is_target(col, row)) { continue; }

            int l_col = col;
            while (l_col >= 0 && !was_visited(l_col, row) && is_target(l_col, row)) { --l_col; }
            ++l_col;

            int r_col = col;
            while (std::cmp_less(r_col, control::width) && !was_visited(r_col, row) && is_target(r_col, row)) {
                ++r_col;
            }
            --r_col;

            segments.push_back({.y = row, .x1 = l_col, .x2 = r_col});

            for (int xi = l_col; xi <= r_col; ++xi) { mark_visited(xi, row); }

            for (int xi = l_col; xi <= r_col; ++xi) {
                stack.emplace_back(xi, row - 1);
                stack.emplace_back(xi, row + 1);
            }
        }

        size_t total_pixels = 0;

        for (auto &[y, x1, x2] : segments) {
            const int count = x2 - x1 + 1;
            total_pixels += count;

            std::vector<unsigned int> indices(count, current_location);

            canvas.edit(indices, {x1, y}, count);
        }

        // update sizes once
        location::change_size(current_location, static_cast<int>(total_pixels));
        location::change_size(target_idx, -static_cast<int>(total_pixels));
    }

    auto control::init() -> void {
        constexpr unsigned char max = std::numeric_limits<unsigned char>::max();
        constexpr std::array<unsigned char, 4> palette = {10U, max, max, max};
        auto indices = std::vector<unsigned int>(width * height, 0);
        auto texture = std::make_shared<graphics::paletted_texture>(indices, static_cast<int>(width), palette);
        canvas_ = texture;
        auto spr = std::make_shared<graphics::sprite>(graphics::sprite::create_paletted(std::move(texture),
            {.position = {0, 0}, .size = {static_cast<int>(width), static_cast<int>(height)}},
            {static_cast<int>(width), static_cast<int>(height)}));
        graphics::window::add_sprite(std::move(spr));
        running_ = true;
        cursors_.at(0) = graphics::window::create_cursor(graphics::image("images/gui.png"_zsv), 0, 0);
        cursors_.at(1) = graphics::window::create_cursor(graphics::image("images/pan.png"_zsv), 16, 16);
        cursors_.at(2) = graphics::window::create_cursor(graphics::image("images/paint.png"_zsv), 0, 0);
        cursors_.at(3) = graphics::window::create_cursor(graphics::image("images/select.png"_zsv), 7, 7);
        cursors_.at(4) = graphics::window::create_cursor(graphics::image("images/fill.png"_zsv), 16, 16);
        graphics::window::set_cursor(cursors_.at(0));

        location::set_color_change_callback(color_change_callback);
        const auto &current_location = location::new_location();
        current_location_ = current_location.idx();
        location::change_size(current_location_, width * height);
        if (const auto canvas = canvas_.lock()) { canvas->set_color(current_location_, current_location.base_color()); }
    }

    auto control::end() -> void { running_ = false; }

    auto control::key_callback(const int key, const int action, const int scancode, const int mods) -> void {
        if (ImGui::GetIO().WantCaptureKeyboard) { return; }

        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_ESCAPE) {
                mode_ = mode::gui;
                graphics::window::set_cursor(cursors_.at(0));
            }
            else if (key == GLFW_KEY_SPACE) {
                mode_ = mode::pan;
                graphics::window::set_cursor(cursors_.at(1));
            }
            else if (key == GLFW_KEY_B) {
                mode_ = mode::paint;
                graphics::window::set_cursor(cursors_.at(2));
            }
            else if (key == GLFW_KEY_X) {
                mode_ = mode::select;
                graphics::window::set_cursor(cursors_.at(3));
            }
            else if (key == GLFW_KEY_F) {
                mode_ = mode::fill;
                graphics::window::set_cursor(cursors_.at(4));
            }
            else if (key == GLFW_KEY_S && (mods & GLFW_MOD_CONTROL) != 0) { saver::save("saves/default"_zsv); }
            else if (key == GLFW_KEY_L && (mods & GLFW_MOD_CONTROL) != 0) { saver::load("saves/default"_zsv); }
            dragging_ = false;
        }
    }

    auto control::mouse_callback(const int button, const int action, const int mods) -> void {
        last_cursor_pos_ = graphics::window::cursor_pos();
        if (last_cursor_pos_.at(0) > graphics::window::size().at(0) * (1.0 - gui_fraction)) { return; }
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                dragging_ = true;
                if (const auto canvas = canvas_.lock()) {
                    const auto canvas_x = static_cast<int>((last_cursor_pos_.at(0) - offset_.at(0)) / zoom_);
                    const auto canvas_y = static_cast<int>((last_cursor_pos_.at(1) - offset_.at(1)) / zoom_);
                    if (canvas_x >= 0 && std::cmp_less(canvas_x, width) && canvas_y >= 0 &&
                        std::cmp_less(canvas_y, height)) {
                        if (mode_ == mode::paint) {
                            const std::array indices = {static_cast<unsigned int>(current_location_)};
                            location::change_size(current_location_, 1);
                            location::change_size(canvas->index({canvas_x, canvas_y}), -1);
                            canvas->edit(indices, {canvas_x, canvas_y}, 1);
                        }
                        else if (mode_ == mode::select) { current_location_ = canvas->index({canvas_x, canvas_y}); }
                        else if (mode_ == mode::fill) { fill(canvas_x, canvas_y, *canvas, current_location_); }
                    }
                }
            }
            else if (action == GLFW_RELEASE) { dragging_ = false; }
        }
    }

    static auto between_points(const std::array<int, 2> point1,
        const std::array<int, 2> point2) -> std::vector<std::array<int, 2>> {
        std::vector<std::array<int, 2>> points;
        const auto del_x = point2.at(0) - point1.at(0);
        const auto del_y = point2.at(1) - point1.at(1);
        const auto steps = std::max(std::abs(del_x), std::abs(del_y));
        const auto x_inc = static_cast<double>(del_x) / static_cast<double>(steps);
        const auto y_inc = static_cast<double>(del_y) / static_cast<double>(steps);
        double x_pos = point1.at(0);
        double y_pos = point1.at(1);
        for (auto i = 0; i < steps; ++i) {
            points.push_back({static_cast<int>(x_pos), static_cast<int>(y_pos)});
            x_pos += x_inc;
            y_pos += y_inc;
        }
        points.push_back(point2);
        return points;
    }

    auto control::cursor_callback(const double x_pos, const double y_pos) -> void {
        if (last_cursor_pos_.at(0) > graphics::window::size().at(0) * (1.0 - gui_fraction)) {
            last_cursor_pos_ = {x_pos, y_pos};
            return;
        }
        if (dragging_ && mode_ == mode::pan) {
            const auto x_offset = static_cast<int>(x_pos - last_cursor_pos_.at(0));
            const auto y_offset = static_cast<int>(y_pos - last_cursor_pos_.at(1));

            offset_.at(0) += x_offset;
            offset_.at(1) += y_offset;

            for (const auto &sprite : graphics::window::sprites()) {
                const auto pos = sprite->position();
                sprite->set_position({pos.at(0) + x_offset, pos.at(1) + y_offset});
            }
        }
        else if (dragging_ && mode_ == mode::paint) {
            if (const auto canvas = canvas_.lock()) {
                const auto canvas_x = static_cast<int>((x_pos - offset_.at(0)) / zoom_);
                const auto canvas_y = static_cast<int>((y_pos - offset_.at(1)) / zoom_);
                const auto prev_x = static_cast<int>((last_cursor_pos_.at(0) - offset_.at(0)) / zoom_);
                const auto prev_y = static_cast<int>((last_cursor_pos_.at(1) - offset_.at(1)) / zoom_);

                for (const auto &point : between_points({prev_x, prev_y}, {canvas_x, canvas_y})) {
                    const auto point_x = point.at(0);
                    const auto point_y = point.at(1);
                    if (point_x >= 0 && std::cmp_less(point_x, width) && point_y >= 0 &&
                        std::cmp_less(point_y, height)) {
                        const std::array indices = {static_cast<unsigned int>(current_location_)};
                        location::change_size(current_location_, 1);
                        location::change_size(canvas->index({point_x, point_y}), -1);
                        canvas->edit(indices, {point_x, point_y}, 1);
                    }
                }
            }
        }
        last_cursor_pos_ = {x_pos, y_pos};
    }

    auto control::scroll_callback(const double x_offset, const double y_offset) -> void {
        static std::array<double, 2> sprite_sizes = {width, height};
        static std::array sprite_positions = {0.0, 0.0};

        constexpr auto zoom_factor = 0.1;
        const auto zoom = 1.0 + y_offset * zoom_factor;
        if (zoom == 1.0) { return; }

        sprite_sizes.at(0) *= zoom;
        sprite_sizes.at(1) *= zoom;

        offset_.at(0) = last_cursor_pos_.at(0) - (last_cursor_pos_.at(0) - offset_.at(0)) * zoom;
        offset_.at(1) = last_cursor_pos_.at(1) - (last_cursor_pos_.at(1) - offset_.at(1)) * zoom;

        sprite_positions.at(0) = last_cursor_pos_.at(0) - (last_cursor_pos_.at(0) - sprite_positions.at(0)) * zoom;
        sprite_positions.at(1) = last_cursor_pos_.at(1) - (last_cursor_pos_.at(1) - sprite_positions.at(1)) * zoom;

        for (const auto &sprite : graphics::window::sprites()) {
            sprite->set_position({static_cast<int>(sprite_positions.at(0)), static_cast<int>(sprite_positions.at(1))});
            sprite->set_size({
                static_cast<int>(sprite_sizes.at(0)),
                static_cast<int>(sprite_sizes.at(1)),
            });
        }

        zoom_ *= zoom;
    }

    auto control::drop_callback(const int count, const char **paths) -> void {
        for (auto iter = 0; iter < count; ++iter) {
            const std::string texture_path = paths[iter];
            if (texture_path.ends_with(".png")) {
                auto spr = std::make_shared<graphics::sprite>(graphics::sprite::create_from_image(graphics::image(
                    zstring_view{
                        texture_path
                    }), {
                    .position = {static_cast<int>(offset_.at(0) * zoom_), static_cast<int>(offset_.at(1) * zoom_)},
                    .size = {static_cast<int>(width / zoom_), static_cast<int>(height / zoom_)}
                }, graphics::window::size()));
                std::string filename;
                const auto last_slash = texture_path.find_last_of("/\\");
                if (last_slash != std::string::npos) { filename = texture_path.substr(last_slash + 1); }
                images_.emplace_back(spr, filename, 1.0F);
                graphics::window::add_sprite(std::move(spr));
            }
        }
    }

    auto control::resize_callback(int width, int height) -> void {
        ImGui::GetIO().FontGlobalScale = static_cast<float>(height) / 700.0F;
    }

    static auto draw_property_info(size_t &selected_idx, size_t &property_value_idx) -> void {
        static std::string property_name;
        static std::array color_data = {0.0F, 0.0F, 0.0F};
        ImGui::InputText("New Property", &property_name);
        if (ImGui::Button("Add Property")) { location::add_property(std::move(property_name)); }
        if (ImGui::BeginCombo("Properties", selected_idx == location::base_color_index ? "Base Color" :
            location::properties()[selected_idx].name.c_str())) {
            const auto properties = location::properties();
            if (ImGui::Selectable("Base Color", selected_idx == location::base_color_index)) {
                selected_idx = location::base_color_index;
                location::change_current_property(location::base_color_index);
            }
            for (size_t i = 0; i < properties.size(); ++i) {
                if (ImGui::Selectable(properties[i].name.c_str(), i == selected_idx)) {
                    property_value_idx = 0;
                    selected_idx = i;
                    location::change_current_property(i);
                }
            }
            ImGui::EndCombo();
        }
        if (selected_idx != location::base_color_index && ImGui::BeginCombo("Property Values",
                location::properties()[selected_idx].values.at(property_value_idx).name.c_str())) {
            const auto &prop_values = location::prop(selected_idx).values;
            for (size_t i = 0; i < prop_values.size(); ++i) {
                if (ImGui::Selectable(prop_values.at(i).name.c_str(), i == property_value_idx)) { property_value_idx = i; }
            }
            ImGui::EndCombo();
        }
        if (selected_idx != location::base_color_index) {
            const auto &[color, name] = location::prop_value(selected_idx, property_value_idx);
            color_data = {
                static_cast<float>(color.at(0)) / max_color_f,
                static_cast<float>(color.at(1)) / max_color_f,
                static_cast<float>(color.at(2)) / max_color_f
            };
            if (ImGui::ColorEdit3("Color", color_data.data())) {
                location::change_property_color(selected_idx, property_value_idx, {
                    static_cast<unsigned char>(color_data.at(0) * max_color_f),
                    static_cast<unsigned char>(color_data.at(1) * max_color_f),
                    static_cast<unsigned char>(color_data.at(2) * max_color_f),
                    static_cast<int>(max_color_f)
                });
            }
            static std::string value_name;
            ImGui::InputText("New Value", &value_name);
            if (ImGui::Button("Add Value")) { location::add_property_value(std::move(value_name), {0, 0, 0, 255}); }
        }
    }

    static auto draw_province_info(size_t current_location) -> void {
        ImGui::TextUnformatted(std::format("Selected Location ID: {}", current_location).c_str());
        const auto color = location::locations()[current_location].base_color();
        const ImVec4 color_f = {
            static_cast<float>(color.at(0)) / max_color_f,
            static_cast<float>(color.at(1)) / max_color_f,
            static_cast<float>(color.at(2)) / max_color_f,
            1.0F
        };
        ImGui::ColorButton("Location Color", color_f);
        const auto &loc = location::locations()[current_location];
        for (size_t i = 0; i < location::properties().size(); i++) {
            const auto &[prop_values, prop_name] = location::prop(i);
            const auto location_value_idx = loc.prop_value(i);
            const auto [prop_val_color, prop_val_name] = location::prop_value(i, location_value_idx);
            ImGui::TextUnformatted(std::format("{}: {}", prop_name, prop_val_name).c_str());
            if (ImGui::BeginCombo(prop_name.c_str(), prop_val_name.c_str())) {
                for (size_t j = 0; j < prop_values.size(); ++j) {
                    const auto &[prop_val_color_c, prop_val_name_c] = prop_values.at(j);
                    if (ImGui::Selectable(prop_val_name_c.c_str(), j == location_value_idx)) {
                        location::set_property(loc.idx(), i, j);
                    }
                }
                ImGui::EndCombo();
            }
        }
    }

    static auto draw_image_list(std::vector<control::image_ref> &images) -> void {
        ImGui::TextUnformatted("Images:");
        size_t iter = 0;
        for (auto &[sprite, name, opacity] : images) {
            ImGui::TextUnformatted(name.c_str());
            ImGui::SameLine();
            if (ImGui::Button(std::format("Remove##{}", iter).c_str())) {
                graphics::window::remove_sprite(*sprite.lock());
                images.erase(images.begin() + static_cast<std::ptrdiff_t>(iter));
                break;
            }
            if (ImGui::SliderFloat(
                std::format("##opacity_{}", iter).c_str(),
                &opacity,
                0.0F,
                1.0F)) {
                sprite.lock()->set_uniform("opacity"_zsv, opacity);
            }
            iter++;
        }
    }

    auto control::draw_callback() -> void {
        ImGui::SetNextWindowPos({static_cast<float>(graphics::window::size().at(0) * (1.0 - gui_fraction)), 0});
        ImGui::SetNextWindowSize(
        {
            static_cast<float>(graphics::window::size().at(0) * gui_fraction),
            static_cast<float>(graphics::window::size().at(1))
        });
        if (mode_ != mode::gui) { ImGui::BeginDisabled(true); }
        if (ImGui::Begin("GUI", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                         ImGuiWindowFlags_NoMove)) {
            ImGui::TextUnformatted(
                std::format("Mouse: ({:.1f}, {:.1f})", last_cursor_pos_.at(0), last_cursor_pos_.at(1)).c_str());
            ImGui::TextUnformatted(std::format("Mode: {}", magic_enum::enum_name(mode_).data()).c_str());
            if (ImGui::Button("New Location")) {
                if (location::locations()[current_location_].size() != 0) {
                    const auto &current_location = location::new_location();
                    current_location_ = current_location.idx();
                    if (const auto canvas = canvas_.lock()) {
                        canvas->set_color(current_location_, current_location.base_color());
                    }
                }
            }
            ImGui::Separator();
            draw_property_info(selected_idx_, property_value_idx_);
            ImGui::Separator();
            draw_province_info(current_location_);
            ImGui::Separator();
            draw_image_list(images_);
        }
        ImGui::End();
        if (mode_ != mode::gui) { ImGui::EndDisabled(); }
    }

    auto control::color_change_callback(const std::span<const location> locations) -> void {
        for (const auto &loc : locations) {
            if (const auto canvas = canvas_.lock()) {
                canvas->set_color(loc.idx(), loc.prop_color(location::current_property()));
            }
        }
    }

    auto control::canvas() -> graphics::paletted_texture & { return *canvas_.lock(); }
} // namespace logic
