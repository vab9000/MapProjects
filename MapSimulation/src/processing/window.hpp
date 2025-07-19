#pragma once

#include <functional>
#include <vector>
#include <SFML/Graphics.hpp>

namespace processing {
    class window {
        sf::RenderWindow window_;
        std::vector<std::function<void(sf::RenderWindow &)> > render_funcs_;
        std::array<uint_fast32_t, 4> gui_area_;
        std::function<void(const sf::Event &)> event_func_;

        // Handles every frame of the event loop
        auto event_loop(sf::Clock &clock) -> void;

        // Handles event signals
        auto handle_event() -> void;

    public:
        explicit window(std::function<void(const sf::Event &)> &&event_func);

        // Get the dimensions of the window
        auto window_dimensions() const -> sf::Vector2u;

        // Stop the event loop (and by extension, the program)
        auto stop_event_loop() -> void;

        // Add a function to be called during the render phase
        auto add_render_func(const std::function<void(sf::RenderWindow &)> &func) -> void;

        // Remove all render functions
        auto clear_render_funcs() -> void;

        // Start the event loop
        auto start_event_loop() -> void;

        // Get the GUI area
        auto gui_area() -> const std::array<uint_fast32_t, 4> &;
    };
}
