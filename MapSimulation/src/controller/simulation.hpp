#pragma once

#include <algorithm>
#include <execution>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <SFML/Graphics.hpp>
#include "image.hpp"
#include "load_image.hpp"
#include "../model/base/province.hpp"
#include "../model/base/utils.hpp"
#include "../view/window.hpp"
#include "../view/drawing.hpp"
#include "data.hpp"

class simulation {
    std::array<int, 2> offset_ = {0, 0};
    double zoom_ = 1.0;
    std::array<int, 2> previous_mouse_ = {0, 0};
    bool mouse_down_ = false;
    bool mouse_moved_ = false;
    double progress_ = 0.0;
    bool open_ = true;

    std::vector<unsigned char> bytes_ = {};
    image map_image_ = {};

    data data_ = {};

    drawing drawer_ = drawing(&data_, &progress_);
    window window_ = window::window(this, &drawer_, &simulation::handle_event);

    static void set_pixel(std::vector<unsigned char> &bytes, const int index, const unsigned char r,
                          const unsigned char g, const unsigned char b) {
        bytes[index] = r;
        bytes[index + 1] = g;
        bytes[index + 2] = b;
        bytes[index + 3] = 255;
    }

    void change_map_mode(const map_modes mode) {
        data_.map_mode = mode;

        const auto province_values = data_.provinces | std::views::values;

        std::for_each(std::execution::par, province_values.begin(), province_values.end(),
                      [&](const std::unique_ptr<province> &province) { province->recolor(data_.map_mode); });

        reload_bitmap();
    }

    void select_province(province *province) {
        const auto old_selected_province = data_.selected_province;
        data_.selected_province = province;
        if (old_selected_province != nullptr) {
            reload_bitmap_province(*old_selected_province);
        }
        if (data_.selected_province != nullptr) {
            reload_bitmap_province(*data_.selected_province);
        }
    }

    void reload_bitmap_province(const province &reload_province) {
        const auto color = reload_province.color;
        const auto pixels = reload_province.get_pixels();

        std::vector<int> indices(reload_province.get_num_pixels());
        std::iota(indices.begin(), indices.end(), 0);

        std::for_each(std::execution::par, indices.begin(), indices.end(), [&](const int i) {
            const auto pixel = pixels[i];
            const auto index = (pixel[0] + pixel[1] * map_image_.width) * 4;
            set_pixel(bytes_, index, static_cast<unsigned char>(color), static_cast<unsigned char>(color >> 8),
                      static_cast<unsigned char>(color >> 16));
        });

        const auto outline = reload_province.get_outline();
        auto updated_provinces = std::unordered_set<province *>();
        for (auto i = 0; i < reload_province.get_num_outline(); ++i) {
            const auto pixel = outline[i].second;
            const auto other_province = outline[i].first;
            const auto index = (pixel[0] + pixel[1] * map_image_.width) * 4;
            if (data_.selected_province != nullptr && reload_province.base_color == data_.selected_province->
                base_color) {
                set_pixel(bytes_, index, 255, 255, 255);
            } else {
                if (other_province != nullptr && other_province->color != color) {
                    set_pixel(bytes_, index, 0, 0, 0);
                } else {
                    set_pixel(bytes_, index, static_cast<unsigned char>(color), static_cast<unsigned char>(color >> 8),
                              static_cast<unsigned char>(color >> 16));
                }
            }
            if (!updated_provinces.contains(other_province)) {
                updated_provinces.insert(other_province);
                if (other_province == nullptr) {
                    return;
                }
                const auto other_outline = other_province->get_outline();
                for (unsigned int j = 0; j < other_province->get_num_outline(); ++j) {
                    const auto other_pixel = other_outline[j].second;
                    const auto other_index = (other_pixel[0] + other_pixel[1] * map_image_.width) * 4;
                    if (other_outline[j].first == &const_cast<province &>(reload_province)) {
                        if (other_province->color == color) {
                            set_pixel(bytes_, other_index, static_cast<unsigned char>(color),
                                      static_cast<unsigned char>(color >> 8),
                                      static_cast<unsigned char>(color >> 16));
                        } else {
                            set_pixel(bytes_, other_index, 0, 0, 0);
                        }
                    }
                }
            }
        }

        drawer_.update_map_texture(bytes_.data() + map_image_.width * reload_province.get_bounds()[1] * 4,
                                   sf::Vector2u(map_image_.width,
                                                reload_province.get_bounds()[3] - reload_province.get_bounds()[1]),
                                   sf::Vector2u(0, reload_province.get_bounds()[1]));
    }

    void reload_bitmap() {
        const auto province_values = data_.provinces | std::views::values;

        std::for_each(
            std::execution::par, province_values.begin(), province_values.end(),
            [&](const std::unique_ptr<province> &province) {
                const auto pixels = province->get_pixels();

                std::vector<int> indices(province->get_num_pixels());
                std::iota(indices.begin(), indices.end(), 0);

                std::for_each(std::execution::par, indices.begin(), indices.end(), [&](const int i) {
                    const auto pixel = pixels[i];
                    const auto color = province->color;
                    const auto index = (pixel[0] + pixel[1] * map_image_.width) * 4;
                    set_pixel(bytes_, index, static_cast<unsigned char>(color), static_cast<unsigned char>(color >> 8),
                              static_cast<unsigned char>(color >> 16));
                });

                const auto outline = province->get_outline();

                indices = std::vector<int>(province->get_num_outline());
                std::iota(indices.begin(), indices.end(), 0);
                std::for_each(std::execution::par, indices.begin(), indices.end(), [&](const int i) {
                    const auto other_province = outline[i].first;
                    const auto pixel = outline[i].second;
                    const auto color = province->color;
                    const auto index = (pixel[0] + pixel[1] * map_image_.width) * 4;
                    if (data_.selected_province != nullptr && color == data_.selected_province->color) {
                        set_pixel(bytes_, index, 255, 255, 255);
                    } else {
                        if (other_province != nullptr && other_province->color != color) {
                            set_pixel(bytes_, index, 0, 0, 0);
                        } else {
                            set_pixel(bytes_, index, static_cast<unsigned char>(color),
                                      static_cast<unsigned char>(color >> 8),
                                      static_cast<unsigned char>(color >> 16));
                        }
                    }
                });
            });

        drawer_.update_map_texture(bytes_.data());
    }

    void start_processing() {
        window_.add_render_func(&drawing::draw_loading_screen);
        load_image(data_, map_image_, &progress_);
        bytes_ = std::vector<unsigned char>(map_image_.width * map_image_.height * 4);
        reload_bitmap();
        if (!drawer_.init_sprites(map_image_, bytes_)) {
            throw std::runtime_error("Failed to initialize sprites");
        }
        window_.clear_render_funcs();
        window_.add_render_func(&drawing::draw_map);

        while (open_) {
        }

        window_.stop_event_loop();
    }

    void handle_event(const sf::Event &event) {
        if (const auto &scroll_data = event.getIf<sf::Event::MouseWheelScrolled>()) {
            const auto dimensions = window_.window_dimensions();
            if (scroll_data->delta > 0) {
                if (zoom_ > 5) {
                    return;
                }
                zoom_ *= 1.1;
                offset_[0] -= static_cast<int>((dimensions.x / 2.0 - offset_[0]) * 0.1);
                offset_[1] -= static_cast<int>((dimensions.y / 2.0 - offset_[1]) * 0.1);
            } else {
                offset_[0] += static_cast<int>((dimensions.x / 2.0 - offset_[0]) / 11.0);
                offset_[1] += static_cast<int>((dimensions.y / 2.0 - offset_[1]) / 11.0);
                zoom_ /= 1.1;
            }
            if (offset_[0] > 0) {
                offset_[0] -= static_cast<int>(map_image_.width * zoom_);
            }
            if (offset_[1] > 0) {
                offset_[1] = 0;
            }
            if (offset_[0] < -(map_image_.width * zoom_)) {
                offset_[0] += static_cast<int>(map_image_.width * zoom_);
            }
            if (offset_[1] < -(map_image_.height * zoom_ - dimensions.y)) {
                offset_[1] = static_cast<int>(-(map_image_.height * zoom_ - dimensions.y));
            }
            drawer_.recalculate_sprite_coords(offset_, zoom_, map_image_.width);
        } else if (const auto &release_data = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (release_data->button != sf::Mouse::Button::Left) return;

            const auto x = static_cast<int>((release_data->position.x - offset_[0]) / zoom_);
            const auto y = static_cast<int>((release_data->position.y - offset_[1]) / zoom_);
            if (!mouse_moved_) {
                const auto color = map_image_.get_color(x % map_image_.width, y);
                const auto province = data_.provinces.at(color).get();
                select_province(province);
            }
            mouse_down_ = false;
            mouse_moved_ = false;
        } else if (const auto &press_data = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (press_data->button != sf::Mouse::Button::Left) return;

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
                    offset_[0] -= static_cast<int>(map_image_.width * zoom_);
                }
                if (offset_[1] > 0) {
                    offset_[1] = 0;
                }
                if (offset_[0] < -(map_image_.width * zoom_)) {
                    offset_[0] += static_cast<int>(map_image_.width * zoom_);
                }
                if (offset_[1] < -(map_image_.height * zoom_ - dimensions.y)) {
                    offset_[1] = static_cast<int>(-(map_image_.height * zoom_ - dimensions.y));
                }

                previous_mouse_[0] = x;
                previous_mouse_[1] = y;

                drawer_.recalculate_sprite_coords(offset_, zoom_, map_image_.width);
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

public:
    void start_simulation() {
        std::thread process_thread(&simulation::start_processing, std::ref(*this));
        window_.start_event_loop();
        process_thread.join();
    }
};
