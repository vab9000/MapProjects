#pragma once

#include <execution>
#include <SFML/Graphics.hpp>
#include "image.hpp"
#include "../model/base/province.hpp"
#include "../model/base/utils.hpp"
#include "../view/window.hpp"
#include "../view/drawing.hpp"
#include "data.hpp"

class simulation {
    std::array<int, 2> offset_{0, 0};
    double zoom_ = 1.0;
    std::array<int, 2> previous_mouse_{0, 0};
    bool mouse_down_ = false;
    bool mouse_moved_ = false;
    double progress_ = 0.0;
    volatile bool open_ = true;

    std::vector<unsigned char> bytes_{};
    image map_image_{};

    data data_{};

    window window_{*this, drawer_, &simulation::handle_event};
    drawing drawer_{data_, progress_};

    void change_map_mode(map_modes mode);

    void select_province(province *province);

    void reload_bitmap_province(const province &reload_province);

    void reload_bitmap();

    void start_processing();

    void handle_event(const sf::Event &event);

public:
    void start_simulation();
};
