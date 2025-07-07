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

    // Get the dimensions of the window
    sf::Vector2u window_dimensions() const;

    // Stop the event loop (and by extension, the program)
    void stop_event_loop();

    // Add a function to be called during the render phase
    void add_render_func(const std::function<void(sf::RenderWindow &)>& func);

    // Remove all render functions
    void clear_render_funcs();

    // Start the event loop
    void start_event_loop();

    // Get the GUI area
    const std::array<uint_fast32_t, 4> &gui_area();
};
