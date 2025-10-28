#ifndef MECHANICS_TAG
#define MECHANICS_TAG
#include <numbered.hpp>
#include "../military/army.hpp"
#include "../date.hpp"

namespace mechanics {
    class province;

    class tag final : utils::numbered<tag> {
        std::vector<utils::ref<province>> provinces_;
        std::vector<std::unique_ptr<army>> armies_;
        unsigned int color_;
        int gold_{0};

    public:
        tag();

        explicit tag(unsigned int color);

        // Get the color of the tag
        [[nodiscard]] auto color() const -> unsigned int;

        // Set the color of the tag
        auto set_color(unsigned int color) -> void;

        // Get the amount of gold the tag has
        [[nodiscard]] auto gold() const -> int;

        // Add gold to the tag
        auto add_gold(int amount) -> void;

        // Remove gold from the tag
        auto remove_gold(int amount) -> void;

        // Create a new army for the tag
        auto new_army() -> army &;

        // Add a province to the tag
        auto add_province(province &added_province) -> void;

        // Remove a province from the tag
        auto remove_province(const province &removed_province) -> void;

        // Check if the tag has a specific province
        [[nodiscard]] auto has_province(const province &found_province) const -> bool;

        // Get the list of provinces owned by the tag
        [[nodiscard]] auto provinces() const -> const std::vector<utils::ref<province>> &;

        // Get the list of provinces owned by the tag
        auto provinces() -> std::vector<utils::ref<province>> &;

        // Check if the tag has army access to a specific province
        [[nodiscard]] auto has_army_access(const province &access_province) const -> bool;

        // Tick the tag
        auto tick(tick_t tick_type) -> void;
    };
}
#endif
