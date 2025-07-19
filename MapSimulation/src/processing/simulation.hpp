#pragma once
#include <SFML/Graphics.hpp>
#include "bitmap.hpp"
#include "drawing.hpp"
#include "image.hpp"
#include "window.hpp"
#include "../mechanics/data.hpp"
#include "../mechanics/map_mode.hpp"

namespace processing {
    class simulation {
        std::array<int_fast32_t, 2> offset_{0, 0};
        double_t zoom_ = 1.0;
        std::array<int_fast32_t, 2> previous_mouse_{0, 0};
        bool mouse_down_ = false;
        bool mouse_moved_ = false;
        std::atomic_bool open_ = true;

        std::string loading_text_;

        bitmap bitmap_;
        image map_image_;

        mechanics::map_mode_t map_mode_ = mechanics::map_mode_t::provinces;
        mechanics::province *selected_province_ = nullptr;

        mechanics::data &data_ = mechanics::data::instance();

        drawing drawer_{*this, loading_text_};
        window window_{[this](const sf::Event &event) { this->handle_event(event); }};

        // Select a province
        auto select_province(mechanics::province *province) -> void;

        // Start the game logic thread
        auto start_processing() -> void;

        // Process event from the window
        auto handle_event(const sf::Event &event) -> void;

    public:
        // Run the simulation
        auto start_simulation() -> void;

        // Transform coordinates from map to screen
        auto transform_to_screen_coordinates(std::array<int_fast32_t, 4> &coordinates) const -> void;

        // Change the map mode
        auto change_map_mode(mechanics::map_mode_t mode) -> void;

        // Get the dimensions of the map
        auto map_dimensions() const -> std::array<uint_fast32_t, 2>;

        auto map_mode() const -> mechanics::map_mode_t;

        auto selected_province() const -> mechanics::province *;
    };
}
