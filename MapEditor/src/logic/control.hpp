#ifndef MAPEDITOR_CONTROL_HPP
#define MAPEDITOR_CONTROL_HPP
#include <array>
#include <cstddef>
#include <magic_enum.hpp>
#include <memory>
#include <span>
#include <string>
#include <vector>
#include "location.hpp"
#include "../graphics/sprite.hpp"

namespace graphics {
    class paletted_texture;
} // namespace graphics

namespace logic {
    class control {
        friend class saver;

    public:
        static constexpr size_t width = 10800;
        static constexpr size_t height = 5400;
        static constexpr auto gui_fraction = 0.3;

        struct image_ref {
            std::weak_ptr<graphics::sprite> sprite;
            std::string name;
            float opacity;
        };

        static auto init() -> void;

        static auto end() -> void;

        static auto key_callback(int key, int action, int scancode, int mods) -> void;

        static auto mouse_callback(int button, int action, int mods) -> void;

        static auto cursor_callback(double x_pos, double y_pos) -> void;

        static auto scroll_callback(double x_offset, double y_offset) -> void;

        static auto drop_callback(int count, const char **paths) -> void;

        static auto resize_callback(int width, int height) -> void;

        static auto draw_callback() -> void;

        static auto color_change_callback(std::span<const location> locations) -> void;

        static auto canvas() -> graphics::paletted_texture &;

    private:
        enum class mode {
            gui, pan, paint,
            select, fill,
        };

        static inline auto running_ = false;
        static inline auto dragging_ = false;
        static inline auto mode_ = mode::gui;
        static inline size_t current_location_ = 0;
        static inline std::array<size_t, magic_enum::enum_count<mode>()> cursors_ = {};
        static inline std::array<double, 2> last_cursor_pos_ = {0.0, 0.0};
        static inline std::array<double, 2> offset_ = {0, 0};
        static inline auto zoom_ = 1.0;
        static inline std::weak_ptr<graphics::paletted_texture> canvas_ = {};
        static inline std::weak_ptr<graphics::sprite> canvas_sprite_ = {};
        static inline size_t selected_idx_ = location::base_color_index;
        static inline size_t property_value_idx_ = 0;
        static inline std::vector<image_ref> images_ = {};
        static inline std::array<double, 2> sprite_sizes_ = {width, height};
    };
} // namespace logic

#endif
