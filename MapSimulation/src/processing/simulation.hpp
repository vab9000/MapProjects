#pragma once
#include <SFML/Graphics.hpp>
#include "drawing.hpp"
#include "image.hpp"
#include "window.hpp"
#include "../mechanics/data.hpp"

namespace processing {
    class simulation {
        std::array<int, 2UZ> offset_{0, 0};
        double zoom_{1.0};
        std::array<int, 2UZ> previous_mouse_{0, 0};
        bool mouse_down_{false};
        bool mouse_moved_{false};
        std::atomic_bool open_{true};
        std::atomic_bool paused_{true};
        std::atomic_bool loaded_{false};

        std::string loading_text_;

        image map_image_;

        std::optional<std::reference_wrapper<mechanics::province>> selected_province_{std::nullopt};
        std::optional<std::reference_wrapper<mechanics::province>> hovered_province_{std::nullopt};

        mechanics::data &data_{mechanics::data::instance()};

        drawing drawer_{*this, loading_text_};
        window window_{[this](const sf::Event &event) { this->handle_event(event); }};

        // Select a province
        auto select_province(std::reference_wrapper<mechanics::province> province) -> void;

        // Deselect the currently selected province
        auto deselect_province() -> void;

        // Start the game logic thread
        auto start_processing() -> void;

        // Process event from the window
        auto handle_event(const sf::Event &event) -> void;

    public:
        // Run the simulation
        auto start_simulation() -> void;

        // Transform coordinates from map to screen
        auto transform_to_screen_coordinates(std::array<int, 4UZ> &coordinates) const -> void;

        // Get the dimensions of the map
        auto map_dimensions() const -> std::array<unsigned int, 2UZ>;

        auto hovered_province() const -> std::optional<std::reference_wrapper<mechanics::province>>;

        auto selected_province() const -> std::optional<std::reference_wrapper<mechanics::province>>;
    };
}
