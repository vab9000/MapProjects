#pragma once

#include <SFML/Graphics.hpp>
#include "image.hpp"
#include "../features/province.hpp"
#include "../ui/window.hpp"
#include "../ui/drawing.hpp"
#include "../features/data.hpp"
#include "bitmap.hpp"

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

    map_mode_t map_mode_ = map_mode_t::provinces;
    province *selected_province_ = nullptr;

    data &data_ = data::instance();

    drawing drawer_{*this, loading_text_};
    window window_{[this](const sf::Event &event) { this->handle_event(event); }};

    // Select a province
    void select_province(province *province);

    // Start the game logic thread
    void start_processing();

    // Process event from the window
    void handle_event(const sf::Event &event);

public:
    // Run the simulation
    void start_simulation();

    // Transform coordinates from map to screen
    void transform_to_screen_coordinates(std::array<int_fast32_t, 4> &coordinates) const;

    // Change the map mode
    void change_map_mode(map_mode_t mode);

    // Get the dimensions of the map
    std::array<uint_fast32_t, 2> map_dimensions() const;

    map_mode_t map_mode() const;

    province *selected_province() const;
};
