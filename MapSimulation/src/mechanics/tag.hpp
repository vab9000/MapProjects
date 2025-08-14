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
        unsigned int color_;
        int gold_{0};

        const unsigned int id_;

        static utils::id_generator id_gen_;

    public:
        tag();

        explicit tag(unsigned int color);

        ~tag() override;

        [[nodiscard]] auto id() const -> unsigned int;

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
        [[nodiscard]] auto provinces() const -> const std::list<std::reference_wrapper<province>> &;

        // Get the list of provinces owned by the tag
        auto provinces() -> std::list<std::reference_wrapper<province>> &;

        // Check if the tag has army access to a specific province
        [[nodiscard]] auto has_army_access(const province &access_province) const -> bool;

        // Tick the tag
        auto tick(tick_t tick_type) -> void override;
    };
}
