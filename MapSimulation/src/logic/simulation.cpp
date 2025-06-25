#include "simulation.hpp"
#include <thread>
#include <algorithm>
#include <ranges>
#ifdef __cpp_lib_execution
#include <execution>
#endif
#include "load_image.hpp"

void simulation::change_map_mode(const map_modes mode) {
    data_.map_mode = mode;

    const auto province_values = data_.provinces | std::views::values;

#ifdef __cpp_lib_execution
    std::for_each(std::execution::par_unseq, province_values.begin(), province_values.end(),
                  [&](province &province) { province.recolor(data_.map_mode); });
#else
    std::ranges::for_each(province_values, [&](province &province) { province.recolor(data_.map_mode); });
#endif

    bitmap_.reload_bitmap(data_, drawer_);
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void simulation::select_province(province *province) {
    const auto old_selected_province = data_.selected_province;
    data_.selected_province = province;

    if (old_selected_province != nullptr) {
        bitmap_.reload_bitmap_province(*old_selected_province, data_, drawer_);
    }

    if (data_.selected_province != nullptr) {
        bitmap_.reload_bitmap_province(*data_.selected_province, data_, drawer_);
    }
}

void simulation::start_processing() {
    window_.add_render_func(&drawing::draw_loading_screen);

    load_image(data_, map_image_, progress_, "assets/provinces.txt");

    bitmap_.init_bitmap(&map_image_, data_, drawer_);

    if (!drawer_.init_sprites(map_image_, bitmap_.bytes())) {
        throw std::runtime_error("Failed to initialize sprites");
    }

    window_.clear_render_funcs();
    window_.add_render_func(&drawing::draw_gui);
    window_.add_render_func(&drawing::draw_map);

    while (open_) {
    }

    window_.stop_event_loop();
}

void simulation::handle_event(const sf::Event &event) {
    if (const auto &scroll_data = event.getIf<sf::Event::MouseWheelScrolled>()) {
        const auto dimensions = window_.window_dimensions();
        if (const auto gui_area = window_.gui_area(); scroll_data->position.x > gui_area[0] &&
                                                      scroll_data->position.x < gui_area[2] &&
                                                      scroll_data->position.y > gui_area[1] &&
                                                      scroll_data->position.y < gui_area[3]) {
            return;
        }

        offset_[0] += static_cast<int>(dimensions.x / 2 - scroll_data->position.x);
        offset_[1] += static_cast<int>(dimensions.y / 2 - scroll_data->position.y);

        if (scroll_data->delta > 0) {
            if (zoom_ > 5) {
                return;
            }
            zoom_ *= 1.1;
            offset_[0] -= static_cast<int>((dimensions.x / 2.0 - offset_[0]) * 0.1);
            offset_[1] -= static_cast<int>((dimensions.y / 2.0 - offset_[1]) * 0.1);
        } else {
            if (zoom_ / 1.1 < static_cast<double>(dimensions.y) / map_image_.height()) {
                zoom_ = static_cast<double>(dimensions.y) / map_image_.height();
            } else {
                offset_[0] += static_cast<int>((dimensions.x / 2.0 - offset_[0]) / 11.0);
                offset_[1] += static_cast<int>((dimensions.y / 2.0 - offset_[1]) / 11.0);
                zoom_ /= 1.1;
            }
        }

        offset_[0] -= static_cast<int>(dimensions.x / 2 - scroll_data->position.x);
        offset_[1] -= static_cast<int>(dimensions.y / 2 - scroll_data->position.y);

        if (offset_[0] > 0) {
            offset_[0] -= static_cast<int>(map_image_.width() * zoom_);
        } else if (offset_[0] < -(map_image_.width() * zoom_)) {
            offset_[0] += static_cast<int>(map_image_.width() * zoom_);
        }

        if (offset_[1] > 0) {
            offset_[1] = 0;
        } else if (offset_[1] < -(map_image_.height() * zoom_ - dimensions.y)) {
            offset_[1] = static_cast<int>(-(map_image_.height() * zoom_ - dimensions.y));
        }

        drawer_.recalculate_sprite_coords(offset_, zoom_, map_image_.width());
    } else if (const auto &release_data = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (release_data->button != sf::Mouse::Button::Left) return;

        if (!mouse_moved_) {
            const auto x = static_cast<int>((release_data->position.x - offset_[0]) / zoom_);
            const auto y = static_cast<int>((release_data->position.y - offset_[1]) / zoom_);

            const auto color = map_image_.color(x % map_image_.width(), y);
            const auto province = &data_.provinces.at(color);
            select_province(province);
        }

        mouse_down_ = false;
        mouse_moved_ = false;
    } else if (const auto &press_data = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (press_data->button != sf::Mouse::Button::Left) return;
        if (const auto gui_area = window_.gui_area(); press_data->position.x > gui_area[0] &&
                                                      press_data->position.x < gui_area[2] &&
                                                      press_data->position.y > gui_area[1] &&
                                                      press_data->position.y < gui_area[3]) {
            return;
        }

        previous_mouse_[0] = press_data->position.x;
        previous_mouse_[1] = press_data->position.y;

        mouse_down_ = true;
        mouse_moved_ = false;
    } else if (const auto &move_data = event.getIf<sf::Event::MouseMoved>()) {
        if (mouse_down_) {
            const auto x = move_data->position.x;
            const auto y = move_data->position.y;

            offset_[0] += x - previous_mouse_[0];
            offset_[1] += y - previous_mouse_[1];

            const auto dimensions = window_.window_dimensions();

            if (offset_[0] > 0) {
                offset_[0] -= static_cast<int>(map_image_.width() * zoom_);
            }
            if (offset_[1] > 0) {
                offset_[1] = 0;
            }
            if (offset_[0] < -(map_image_.width() * zoom_)) {
                offset_[0] += static_cast<int>(map_image_.width() * zoom_);
            }
            if (offset_[1] < -(map_image_.height() * zoom_ - dimensions.y)) {
                offset_[1] = static_cast<int>(-(map_image_.height() * zoom_ - dimensions.y));
            }

            previous_mouse_[0] = x;
            previous_mouse_[1] = y;

            drawer_.recalculate_sprite_coords(offset_, zoom_, map_image_.width());
        }
        mouse_moved_ = true;
    } else if (const auto &key_data = event.getIf<sf::Event::KeyPressed>()) {
        if (key_data->code == sf::Keyboard::Key::Escape) {
            select_province(nullptr);
        }
    } else if (const auto &_ = event.getIf<sf::Event::Closed>()) {
        open_ = false;
    }
}

void simulation::start_simulation() {
    std::thread process_thread(&simulation::start_processing, this);

    window_.start_event_loop();

    process_thread.join();
}

void simulation::transform_to_screen_coordinates(std::array<int, 4> &coordinates) const {
    coordinates[0] = static_cast<int>(coordinates[0] * zoom_ + offset_[0]);
    coordinates[2] = static_cast<int>(coordinates[2] * zoom_ + offset_[0]);
    if (coordinates[2] < 0) {
        coordinates[0] = coordinates[0] + static_cast<int>(map_image_.width() * zoom_);
    }
    coordinates[1] = static_cast<int>(coordinates[1] * zoom_ + offset_[1]);
    coordinates[3] = static_cast<int>(coordinates[3] * zoom_ + offset_[1]);
}
