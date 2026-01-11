#include <array>
#include <exception>
#include <print>
#include <utility>
#include <zstring_view.hpp>
#include "graphics/window.hpp"
#include "logic/control.hpp"

auto main() -> int {
    try {
        if (constexpr std::array starting_dimensions = {800, 600}; graphics::window::init("Map Editor"_zsv,
            starting_dimensions, std::move(graphics::window::callbacks{
                .key_callback = logic::control::key_callback,
                .mouse_callback = logic::control::mouse_callback,
                .cursor_callback = logic::control::cursor_callback,
                .scroll_callback = logic::control::scroll_callback,
                .drop_callback = logic::control::drop_callback,
                .resize_callback = logic::control::resize_callback,
                .draw_callback = logic::control::draw_callback,
            }))) {
            logic::control::init();

            graphics::window::run();

            logic::control::end();

            graphics::window::end();
        }
    }
    catch (const std::exception &e) {
        std::println("Exception: {}\n", e.what());
        return 1;
    }
    catch (...) { return -1; }

    return 0;
}
