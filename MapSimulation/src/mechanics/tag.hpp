#pragma once
#include <list>
#include "army.hpp"
#include "tickable.hpp"
#include "../utils/id_generator.hpp"

namespace mechanics {
    class province;

    class tag final : public tickable {
        std::list<std::reference_wrapper<province>> provinces_;
        std::list<army> armies_;
        uint_fast32_t color_;
        int_fast32_t gold_{0};

        const uint_fast32_t id_;

        static utils::id_generator id_gen_;

    public:
        tag();

        explicit tag(uint_fast32_t color);

        ~tag() override;

        [[nodiscard]] auto id() const -> uint_fast32_t;

        // Get the color of the tag
        [[nodiscard]] auto color() const -> uint_fast32_t;

        // Set the color of the tag
        auto set_color(uint_fast32_t color) -> void;

        // Get the amount of gold the tag has
        [[nodiscard]] auto gold() const -> int_fast32_t;

        // Add gold to the tag
        auto add_gold(int_fast32_t amount) -> void;

        // Remove gold from the tag
        auto remove_gold(int_fast32_t amount) -> void;

        // Create a new army for the tag
        auto new_army() -> army &;

        // Add a province to the tag
        auto add_province(province &added_province) -> void;

        // Remove a province from the tag
        auto remove_province(const province &removed_province) -> void;

        // Check if the tag has a specific province
        [[nodiscard]] auto has_province(const province &found_province) const -> bool;

        // Get the list of provinces owned by the tag
        [[nodiscard]] auto provinces() const -> const std::list<std::reference_wrapper<province>> &;

        // Get the list of provinces owned by the tag
        auto provinces() -> std::list<std::reference_wrapper<province>> &;

        // Check if the tag has army access to a specific province
        [[nodiscard]] auto has_army_access(const province &access_province) const -> bool;

        // Tick the tag
        auto tick(tick_t tick_type) -> void override;
    };
}
