#ifndef MAPEDITOR_WINDOW_HPP
#define MAPEDITOR_WINDOW_HPP
#define GLFW_INCLUDE_NONE
#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <span>
#include <utility>
#include <vector>
#include <zstring_view.hpp>
#include <GLFW/glfw3.h>
#include "image.hpp"
#include "sprite.hpp"

namespace graphics {
    class window {
    public:
        constexpr static size_t cursor_dims = 32;
        using dimensions = std::array<int, 2>;

        struct callbacks {
            std::function<void(int key, int action, int scancode, int mods)> key_callback;
            std::function<void(int button, int action, int mods)> mouse_callback;
            std::function<void(double x_pos, double y_pos)> cursor_callback;
            std::function<void(double x_del, double y_del)> scroll_callback;
            std::function<void(int count, const char **paths)> drop_callback;
            std::function<void(int width, int height)> resize_callback;
            std::function<void()> draw_callback;
        };

        static auto init(zstring_view name, dimensions dims, callbacks &&calls) -> bool;

        static auto run() -> void;

        static auto end() -> void;

        static auto add_sprite(std::shared_ptr<sprite> &&spr) -> void;

        static auto remove_sprite(const sprite &spr) -> void;

        static auto sprites() -> std::span<std::shared_ptr<sprite>>;

        static auto size() -> dimensions;

        static auto cursor_pos() -> std::array<double, 2>;

        static auto create_cursor(const image &img, int x_offset, int y_offset) -> size_t;

        static auto set_cursor(size_t idx) -> void;

    private:
        static inline GLFWwindow *window_ = nullptr;
        static inline dimensions size_ = {0, 0};
        static inline dimensions offset_ = {0, 0};
        static inline std::vector<std::pair<GLFWcursor *, std::array<unsigned char, cursor_dims * cursor_dims * 4>>>
        cursors_ = {};
        static inline std::vector<std::shared_ptr<sprite>> sprites_ = {};
        static inline callbacks callbacks_ = {};

        static auto error_callback(int error, const char *description) -> void;

        static auto key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) -> void;

        static auto mouse_callback(GLFWwindow *window, int button, int action, int mods) -> void;

        static auto cursor_callback(GLFWwindow *window, double x_pos, double y_pos) -> void;

        static auto scroll_callback(GLFWwindow *window, double x_offset, double y_offset) -> void;

        static auto drop_callback(GLFWwindow *window, int count, const char **paths) -> void;

        static auto resize_callback(GLFWwindow *window, int width, int height) -> void;
    };
} // namespace graphics
#endif
