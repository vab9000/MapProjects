#include "simulation.hpp"
#include <execution>
#include <ranges>
#include <thread>
#include <utility>
#include "load_image.hpp"

namespace processing {
    auto simulation::map_dimensions() const -> std::array<unsigned int, 2UZ> {
        return {map_image_.width(), map_image_.height()};
    }

    auto simulation::hovered_province() const -> std::optional<std::reference_wrapper<mechanics::province>> {
        return hovered_province_;
    }

    auto simulation::selected_province() const -> std::optional<std::reference_wrapper<mechanics::province>> {
        return selected_province_;
    }

    auto simulation::select_province(const std::reference_wrapper<mechanics::province> province) -> void {
        selected_province_ = province;
    }

    auto simulation::deselect_province() -> void { selected_province_ = std::nullopt; }

    auto simulation::start_processing() -> void {
        window_.add_render_func([this](sf::RenderWindow &window) { drawer_.draw_loading_message(window); });

        std::vector<uint8_t> crossing_bytes;
        load_image(data_, map_image_, crossing_bytes, loading_text_);

        mechanics::set_province_colors_size();
        mechanics::update_all_province_colors();

        std::vector<uint8_t> province_ids(4UZ * map_image_.width() * map_image_.height());

        const auto x_range = std::views::iota(0U, map_image_.width());
        const auto y_range = std::views::iota(0U, map_image_.height());
        for (const auto x : x_range) {
            for (const auto y : y_range) {
                const auto index = (y * map_image_.width() + x) * 4UZ;
                const auto id = data_.province_at(map_image_.color(x, y)).id();
                province_ids[index + 3UZ] = static_cast<uint8_t>(id % 256U);
                province_ids[index + 2UZ] = static_cast<uint8_t>(id / 256U % 256U);
                province_ids[index + 1UZ] = static_cast<uint8_t>(id / 256U / 256U % 256U);
                province_ids[index] = static_cast<uint8_t>(id / 256U / 256U / 256U % 256U);
            }
        }

        if (!drawer_.init_sprites(map_image_, province_ids, crossing_bytes)) {
            throw std::runtime_error("Failed to initialize sprites");
        }
        crossing_bytes.clear();

        window_.clear_render_funcs();
        window_.add_render_func([this](sf::RenderWindow &window) { drawer_.draw_map(window); });
        window_.add_render_func([this](const sf::RenderWindow &window) { drawer_.draw_gui(window); });

        loaded_ = true;

        while (open_) {
            if (paused_) {
                sleep(sf::milliseconds(100));
                continue;
            }
            data_.tick();
        }

        window_.stop_event_loop();
    }

    auto simulation::handle_event(const sf::Event &event) -> void {
        const auto in_gui = [&](const int x, const int y) {
            const auto gui_area = window_.gui_area();
            return std::cmp_greater(x, gui_area[0UZ]) && std::cmp_less(x, gui_area[2UZ]) && std::cmp_greater(y,
                       gui_area[1UZ]) && std::cmp_less(y, gui_area[3UZ]);
        };

        if (const auto &scroll_data = event.getIf<sf::Event::MouseWheelScrolled>()) {
            const auto dimensions = window_.window_dimensions();
            if (in_gui(scroll_data->position.x,
                scroll_data->position.y)) { return; }

            offset_[0UZ] += static_cast<int>(dimensions.x / 2U) - scroll_data->position.x;
            offset_[1UZ] += static_cast<int>(dimensions.y / 2U) - scroll_data->position.y;

            if (scroll_data->delta > 0.0F) {
                zoom_ *= 1.1;
                if (zoom_ > 10.0) {
                    zoom_ /= 1.1;
                    offset_[0UZ] -= static_cast<int>(dimensions.x / 2U) - scroll_data->position.x;
                    offset_[1UZ] -= static_cast<int>(dimensions.y / 2U) - scroll_data->position.y;
                    return;
                }
                offset_[0UZ] -= static_cast<int>((dimensions.x / 2.0 - offset_[0UZ]) * 0.1);
                offset_[1UZ] -= static_cast<int>((dimensions.y / 2.0 - offset_[1UZ]) * 0.1);
            }
            else {
                if (zoom_ / 1.1 < static_cast<double>(dimensions.y) / map_image_.height()) {
                    zoom_ = static_cast<double>(dimensions.y) / map_image_.height();
                }
                else {
                    offset_[0UZ] += static_cast<int>((dimensions.x / 2.0 - offset_[0UZ]) / 11.0);
                    offset_[1UZ] += static_cast<int>((dimensions.y / 2.0 - offset_[1UZ]) / 11.0);
                    zoom_ /= 1.1;
                }
            }

            offset_[0UZ] -= static_cast<int>(dimensions.x / 2U) - scroll_data->position.x;
            offset_[1UZ] -= static_cast<int>(dimensions.y / 2U) - scroll_data->position.y;

            if (offset_[0UZ] > 0) { offset_[0UZ] -= static_cast<int>(map_image_.width() * zoom_); }
            else if (offset_[0UZ] < -(map_image_.width() * zoom_)) {
                offset_[0UZ] += static_cast<int>(map_image_.width() * zoom_);
            }

            if (offset_[1UZ] > 0) { offset_[1UZ] = 0; }
            else if (offset_[1UZ] < -(map_image_.height() * zoom_ - dimensions.y)) {
                offset_[1UZ] = static_cast<int>(-(map_image_.height() * zoom_ - dimensions.y));
            }

            drawer_.recalculate_sprite_coords(offset_, zoom_);
        }
        else if (const auto &release_data = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (release_data->button != sf::Mouse::Button::Left) { return; }
            if (in_gui(release_data->position.x,
                release_data->position.y)) { return; }

            mouse_down_ = false;
            if (mouse_moved_) {
                mouse_moved_ = false;
                return;
            }

            const auto x = static_cast<unsigned int>((release_data->position.x - offset_[0UZ]) / zoom_);
            const auto y = static_cast<unsigned int>((release_data->position.y - offset_[1UZ]) / zoom_);

            if (loaded_ && y < map_image_.height()) {
                const auto color = map_image_.color(x % map_image_.width(), y);
                auto &province = data_.province_at(color);
                select_province(province);
            }
        }
        else if (const auto &press_data = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (press_data->button != sf::Mouse::Button::Left) { return; }
            if (in_gui(press_data->position.x,
                press_data->position.y)) { return; }

            previous_mouse_[0UZ] = press_data->position.x;
            previous_mouse_[1UZ] = press_data->position.y;

            mouse_down_ = true;
            mouse_moved_ = false;
        }
        else if (const auto &move_data = event.getIf<sf::Event::MouseMoved>()) {
            const auto x = move_data->position.x;
            const auto y = move_data->position.y;

            if (loaded_ && !in_gui(x, y)) {
                const auto i = static_cast<unsigned int>((x - offset_[0UZ]) / zoom_);
                const auto j = static_cast<unsigned int>((y - offset_[1UZ]) / zoom_);

                if (j < map_image_.height()) {
                    const auto color = map_image_.color(i % map_image_.width(), j);
                    auto &province = data_.province_at(color);
                    hovered_province_ = province;
                }
            }
            else { hovered_province_ = std::nullopt; }

            if (!mouse_down_) { return; }
            mouse_moved_ = true;

            offset_[0UZ] += x - previous_mouse_[0UZ];
            offset_[1UZ] += y - previous_mouse_[1UZ];

            const auto dimensions = window_.window_dimensions();

            if (offset_[0UZ] > 0) { offset_[0UZ] -= static_cast<int>(map_image_.width() * zoom_); }
            if (offset_[1UZ] > 0) { offset_[1UZ] = 0; }
            if (offset_[0UZ] < -(map_image_.width() * zoom_)) {
                offset_[0UZ] += static_cast<int>(map_image_.width() * zoom_);
            }
            if (offset_[1UZ] < -(map_image_.height() * zoom_ - dimensions.y)) {
                offset_[1UZ] = static_cast<int>(-(map_image_.height() * zoom_ - dimensions.y));
            }

            previous_mouse_[0UZ] = x;
            previous_mouse_[1UZ] = y;

            drawer_.recalculate_sprite_coords(offset_, zoom_);
        }
        else if (const auto &key_data = event.getIf<sf::Event::KeyPressed>()) {
            switch (key_data->code) {
                case sf::Keyboard::Key::Escape: { deselect_province(); }
                break;
                case sf::Keyboard::Key::Space: { paused_ = !paused_; }
                break;
                default: break;
            }
        }
        else if (event.getIf<sf::Event::Closed>()) { open_ = false; }
    }

    auto simulation::start_simulation() -> void {
        std::thread process_thread(&simulation::start_processing, this);

        window_.start_event_loop();

        process_thread.join();
    }

    auto simulation::transform_to_screen_coordinates(std::array<int, 4UZ> &coordinates) const -> void {
        coordinates[0UZ] = static_cast<int>(coordinates[0UZ] * zoom_ + offset_[0UZ]);
        coordinates[2UZ] = static_cast<int>(coordinates[2UZ] * zoom_ + offset_[0UZ]);
        if (coordinates[2UZ] < 0) {
            coordinates[0UZ] = coordinates[0UZ] + static_cast<int>(map_image_.width() * zoom_);
        }
        coordinates[1UZ] = static_cast<int>(coordinates[1UZ] * zoom_ + offset_[1UZ]);
        coordinates[3UZ] = static_cast<int>(coordinates[3UZ] * zoom_ + offset_[1UZ]);
    }
}
