#pragma once

#include <SFML/Graphics.hpp>
#include "image.hpp"
#include "../features/province.hpp"
#include "../ui/window.hpp"
#include "../ui/drawing.hpp"
#include "data.hpp"
#include "bitmap.hpp"

class simulation {
    std::array<int_fast32_t, 2> offset_{0, 0};
    double_t zoom_ = 1.0;
    std::array<int_fast32_t, 2> previous_mouse_{0, 0};
    bool mouse_down_ = false;
    bool mouse_moved_ = false;
    volatile bool open_ = true;

    std::string loading_text_;

    bitmap bitmap_{};
    image map_image_{};

    data data_{};

    drawing drawer_{data_, *this, loading_text_};
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
    void change_map_mode(map_modes mode);

    // Get the dimensions of the map
    std::array<int, 2> map_dimensions() const;
};
