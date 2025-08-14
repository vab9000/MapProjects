#include "window.hpp"
#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace processing {
    window::window(std::function<void(const sf::Event &)> &&event_func) : window_(sf::RenderWindow(
            sf::VideoMode(sf::Vector2u(800U, 600U)),
            "Map Simulation")),
        gui_area_{0U, 0U, 200U, 600U},
        event_func_(std::move(event_func)) {
        window_.setFramerateLimit(60U);
        // window_.setVerticalSyncEnabled(true);
        if (!ImGui::SFML::Init(window_)) { throw std::runtime_error("Failed to initialize ImGui-SFML"); }
    }

    auto window::window_dimensions() const -> sf::Vector2u { return window_.getSize(); }

    auto window::stop_event_loop() -> void { if (window_.isOpen()) { window_.close(); } }

    auto window::add_render_func(const std::function<void(sf::RenderWindow &)> &func) -> void {
        render_funcs_.emplace_back(func);
    }

    auto window::clear_render_funcs() -> void { render_funcs_.clear(); }

    auto window::event_loop(sf::Clock &clock) -> void {
        handle_event();

        ImGui::SFML::Update(window_, clock.restart());

        window_.clear();

        for (const auto &render_func : render_funcs_) { render_func(window_); }

        ImGui::SFML::Render(window_);

        window_.display();
    }

    auto window::handle_event() -> void {
        while (const auto event = window_.pollEvent()) {
            if (!event.has_value()) { continue; }
            ImGui::SFML::ProcessEvent(window_, *event);
            if (const auto &resize_data = event->getIf<sf::Event::Resized>()) {
                sf::FloatRect view(sf::Vector2f(0.0F, 0.0F),
                    sf::Vector2f(static_cast<float>(resize_data->size.x),
                        static_cast<float>(resize_data->size.y)));
                window_.setView(sf::View(view));
            }
            else if (event->getIf<sf::Event::Closed>() != nullptr) { stop_event_loop(); }
            event_func_(event.value());
        }
    }

    auto window::start_event_loop() -> void {
        sf::Clock clock;

        while (window_.isOpen()) { event_loop(clock); }

        ImGui::SFML::Shutdown();
    }

    auto window::gui_area() -> const std::array<unsigned int, 4UZ> & {
        const auto dimensions = window_dimensions();
        gui_area_[2UZ] = 200U;
        gui_area_[3UZ] = dimensions.y;
        return gui_area_;
    }
}
