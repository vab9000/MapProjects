#pragma once
#include <future>
#include <SFML/Graphics.hpp>
#include "image.hpp"
#include "../mechanics/data.hpp"

namespace processing {
    class simulation {
        inline static std::array<int, 2UZ> offset_{0, 0};
        inline static auto zoom_ = 1.0;
        inline static std::array<int, 2UZ> previous_mouse_{0, 0};

        inline static std::string loading_text_{};

        inline static image map_image_{};

        inline static mechanics::province *hovered_province_ = nullptr;

        inline static std::future<void> processing_{};

        inline static auto mouse_down_ = false;
        inline static auto mouse_moved_ = false;
        inline static std::atomic_bool open_{true};
        inline static std::atomic_bool paused_{true};
        inline static std::atomic_bool loaded_{false};

        // Start the game logic thread
        static auto start_processing() -> void;

        static auto check_process_thread() -> bool;

    public:
        // Run the simulation
        static auto start_simulation() -> void;

        // Transform coordinates from map to screen
        static auto transform_to_screen_coordinates(std::array<int, 4UZ> &coordinates) -> void;

        // Get the dimensions of the map
        static auto map_dimensions() -> std::array<unsigned int, 2UZ>;

        static auto hovered_province() -> mechanics::province *;

        // Process event from the window
        static auto handle_event(const sf::Event &event) -> void;

        static auto loading_text() -> const std::string &;
    };
}
