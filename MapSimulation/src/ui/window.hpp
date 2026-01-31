#pragma once

#include <functional>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../processing/simulation.hpp"

namespace ui {
    class window {
        inline static auto window_{
            sf::RenderWindow(
                sf::VideoMode(sf::Vector2u(800U, 600U)),
                "Map Simulation")
        };
        inline static std::vector<std::function<void(sf::RenderWindow &)>> render_funcs_{};
        inline static std::array<int, 4UZ> gui_area_{0, 0, 250, 600};
        inline static std::function<void(const sf::Event &)> event_func_{
            [](const sf::Event &event) { processing::simulation::handle_event(event); }
        };

        static auto event_loop(sf::Clock &clock) -> void;

        static auto handle_event() -> void;

    public:
        static auto init_window() -> void;

        static auto window_dimensions() -> sf::Vector2u;

        static auto stop_event_loop() -> void;

        static auto add_render_func(const std::function<void(sf::RenderWindow &)> &func) -> void;

        static auto clear_render_funcs() -> void;

        static auto start_event_loop() -> void;

        static auto gui_area() -> const std::array<int, 4UZ> &;
    };
}
