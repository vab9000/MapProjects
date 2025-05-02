#include "window.hpp"
#include "drawing.hpp"

window::window(simulation &simulation, drawing &drawer,
               void (simulation::*event_func)(const sf::Event &)) : window_(sf::RenderWindow(
                                                                        sf::VideoMode(sf::Vector2u(800, 600)),
                                                                        "Map Simulation")), drawer_(drawer),
                                                                    simulation_(simulation),
                                                                    event_func_(event_func) {
    window_.setFramerateLimit(60);
    window_.setVerticalSyncEnabled(true);
}

sf::Vector2u window::window_dimensions() const {
    return window_.getSize();
}

void window::stop_event_loop() {
    if (window_.isOpen()) {
        window_.close();
    }
}

void window::add_render_func(void (drawing::*func)(sf::RenderWindow &) const) {
    render_funcs_.emplace_back(func);
}

void window::clear_render_funcs() {
    render_funcs_.clear();
}

void window::start_event_loop() {
    while (window_.isOpen()) {
        while (const auto event = window_.pollEvent()) {
            if (!event.has_value()) {
                sleep(sf::milliseconds(5));
                continue;
            }
            if (const auto &resize_data = event->getIf<sf::Event::Resized>()) {
                sf::FloatRect view(sf::Vector2f(0.0f, 0.0f),
                                   sf::Vector2f(static_cast<float>(resize_data->size.x),
                                                static_cast<float>(resize_data->size.y)));
                window_.setView(sf::View(view));
            } else if (const auto &_ = event->getIf<sf::Event::Closed>()) {
                stop_event_loop();
            }
            (simulation_.*event_func_)(event.value());
        }

        window_.clear();

        for (const auto &render_func: render_funcs_) {
            (drawer_.*render_func)(window_);
        }

        window_.display();
    }
}
