#pragma once

#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

class window {
    sf::RenderWindow window_;
    std::vector<std::function<void(sf::RenderWindow &)> > render_funcs_;
    std::array<uint_fast32_t, 4> gui_area_;

    std::function<void(const sf::Event &)> event_func_;

public:
    explicit window(std::function<void(const sf::Event &)> &&event_func);

    sf::Vector2u window_dimensions() const;

    void stop_event_loop();

    void add_render_func(std::function<void(sf::RenderWindow &)> func);

    void clear_render_funcs();

    void start_event_loop();

    const std::array<uint_fast32_t, 4> &gui_area();
};
