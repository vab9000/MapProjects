#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <imgui-SFML.h>
#include "window.hpp"

window::window(
    std::function<void(const sf::Event &)> &&event_func) : window_(sf::RenderWindow(
                                                               sf::VideoMode(sf::Vector2u(800, 600)),
                                                               "Map Simulation")),
                                                           gui_area_{0, 0, 200, 600},
                                                           event_func_(std::move(event_func)) {
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

void window::add_render_func(const std::function<void(sf::RenderWindow &)>& func) {
    render_funcs_.emplace_back(func);
}

void window::clear_render_funcs() {
    render_funcs_.clear();
}

void window::event_loop(sf::Clock &clock) {
    handle_event();

    ImGui::SFML::Update(window_, clock.restart());

    window_.clear();

    for (const auto &render_func: render_funcs_) {
        render_func(window_);
    }

    ImGui::SFML::Render(window_);

    window_.display();
}

void window::handle_event() {
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
        event_func_(event.value());
    }
}

void window::start_event_loop() {
    sf::Clock clock;
    while (window_.isOpen()) {
        event_loop(clock);
    }

    ImGui::SFML::Shutdown();
}

const std::array<uint_fast32_t, 4> &window::gui_area() {
    const auto dimensions = window_dimensions();
    gui_area_[2] = 200;
    gui_area_[3] = dimensions.y;
    return gui_area_;
}
