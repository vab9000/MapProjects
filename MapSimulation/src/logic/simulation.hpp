#pragma once

#include <SFML/Graphics.hpp>
#include "image.hpp"
#include "../features/base/province.hpp"
#include "../features/base/utils.hpp"
#include "../ui/window.hpp"
#include "../ui/drawing.hpp"
#include "data.hpp"
#include "bitmap.hpp"

class simulation {
    std::array<int, 2> offset_{0, 0};
    double zoom_ = 1.0;
    std::array<int, 2> previous_mouse_{0, 0};
    bool mouse_down_ = false;
    bool mouse_moved_ = false;
    double progress_ = 0.0;
    volatile bool open_ = true;

    bitmap bitmap_{};
    image map_image_{};

    data data_{};

    drawing drawer_{data_, progress_, this};
    window window_{*this, drawer_, &simulation::handle_event};

    void select_province(province *province);

    void start_processing();

    void handle_event(const sf::Event &event);

public:
    void start_simulation();

    void transform_to_screen_coordinates(std::array<int, 4> &coordinates) const;

    void change_map_mode(map_modes mode);
};
