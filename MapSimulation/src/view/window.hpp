#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

class drawing;
class simulation;

class window {
    sf::RenderWindow window_;
    std::vector<void (drawing::*)(sf::RenderWindow &) const> render_funcs_;
    drawing &drawer_;
    simulation &simulation_;

    void (simulation::*event_func_)(const sf::Event &);

public:
    explicit window(simulation &simulation, drawing &drawer,
                    void (simulation::*event_func)(const sf::Event &));

    sf::Vector2u window_dimensions() const;

    void stop_event_loop();

    void add_render_func(void (drawing::*func)(sf::RenderWindow &) const);

    void clear_render_funcs();

    void start_event_loop();
};
