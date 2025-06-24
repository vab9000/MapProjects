#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <imgui-SFML.h>
#include "window.hpp"
#include "drawing.hpp"

window::window(simulation &simulation, const drawing &drawer,
               void (simulation::*event_func)(const sf::Event &)) : window_(sf::RenderWindow(
                                                                        sf::VideoMode(sf::Vector2u(800, 600)),
                                                                        "Map Simulation")), drawer_(drawer),
                                                                    simulation_(simulation),
                                                                    event_func_(event_func), gui_area_({0, 0, 100, 600}) {
    window_.setFramerateLimit(60);
    // window_.setVerticalSyncEnabled(true);
    if (!ImGui::SFML::Init(window_)) {
        throw std::runtime_error("Failed to initialize ImGui-SFML");
    }
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
    sf::Clock clock;
    while (window_.isOpen()) {
        while (const auto event = window_.pollEvent()) {
            if (!event.has_value()) {
                continue;
            }
            ImGui::SFML::ProcessEvent(window_, *event);
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

        ImGui::SFML::Update(window_, clock.restart());

        window_.clear();

        for (const auto &render_func: render_funcs_) {
            (drawer_.*render_func)(window_);
        }

        ImGui::SFML::Render(window_);

        window_.display();
    }

    ImGui::SFML::Shutdown();
}

const std::array<int, 4> & window::gui_area() {
    const auto dimensions = window_dimensions();
    gui_area_[2] = dimensions.x / 8;
    gui_area_[3] = dimensions.y;
    return gui_area_;
}
