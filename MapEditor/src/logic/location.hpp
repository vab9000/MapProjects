#ifndef MAPEDITOR_LOCATION_HPP
#define MAPEDITOR_LOCATION_HPP
#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <span>
#include <string>
#include <vector>

namespace logic {
    class location {
        friend class saver;

    public:
        static constexpr size_t base_color_index = std::numeric_limits<size_t>::max();

        struct value {
            std::array<unsigned char, 4> color;
            std::string name;
        };

        struct property {
            std::vector<value> values;
            std::string name;
        };

        location();

        explicit location(size_t idx);

        location(location &&other) noexcept;

        location(const location &other) = delete;

        auto operator=(location &&other) noexcept -> location &;

        auto operator=(const location &other) -> location & = delete;

        ~location();

        [[nodiscard]] auto idx() const -> size_t;

        [[nodiscard]] auto name() const -> const std::string &;

        [[nodiscard]] auto prop_value(size_t prop_index) const -> size_t;

        [[nodiscard]] auto prop_color(size_t prop_index) const -> std::array<unsigned char, 4>;

        [[nodiscard]] auto size() const -> int;

        [[nodiscard]] auto base_color() const -> std::array<unsigned char, 4>;

        static auto set_property(size_t location_idx, size_t prop_index, size_t value_index) -> void;

        static auto locations() -> std::span<const location>;

        static auto properties() -> std::span<const property>;

        static auto prop(size_t index) -> const property &;

        static auto prop_value(size_t prop_index, size_t value_index) -> const value &;

        static auto add_property(std::string &&name) -> size_t;

        static auto remove_property(size_t index) -> void;

        static auto add_property_value(std::string &&name, std::array<unsigned char, 4> color) -> size_t;

        static auto remove_property_value(size_t prop_index, size_t value_index) -> void;

        static auto change_property_color(size_t prop_index, size_t value_index,
            std::array<unsigned char, 4> color) -> void;

        static auto set_color_change_callback(
            std::function<void(std::span<const location>)> &&callback) -> void;

        static auto new_location() -> const location &;

        static auto change_size(size_t location_idx, int delta) -> void;

        static auto change_current_property(size_t prop_index) -> void;

        static auto current_property() -> size_t;

    private:
        static auto free_color() -> std::array<unsigned char, 4>;

        static inline size_t current_property_ = base_color_index;
        static inline std::function<void(std::span<const location>)> on_color_change_ =
            {};
        static inline std::vector<property> properties_ = {};
        static inline std::vector<size_t> free_locations_ = {};
        static std::vector<location> locations_;

        int size_ = 0;
        size_t idx_;
        std::string name_;
        std::vector<size_t> property_values_;
        std::array<unsigned char, 4> base_color_;
    };
} // namespace logic

#endif
