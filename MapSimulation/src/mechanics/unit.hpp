#pragma once
#include <list>
#include <reference.hpp>
#include <vector>
#include "pop.hpp"

namespace mechanics {
    class army;
    class province;
    class character;

    class unit {
        std::vector<utils::ref<pop>> pops_;
        utils::ref<army> parent_army_;
        character *captain_{nullptr};
        utils::ref<province> location_;
        std::list<utils::ref<province> > path_;
        double travel_progress_{0.0};
        bool retreating_{false};

    public:
        unit(army &parent_army, province &location);

        ~unit();

        // Add a pop to the unit
        auto add_pop(pop &new_pop) -> void;

        // List of pops that are in this unit
        [[nodiscard]] auto pops() const -> const std::vector<utils::ref<pop>> &;

        // List of pops that are in this unit
        auto pops() -> std::vector<utils::ref<pop>> &;

        // Get the total number of troops in the unit
        [[nodiscard]] auto size() const -> unsigned int;

        // Get the parent army of the unit
        [[nodiscard]] auto parent() const -> army &;

        // Set the parent army of the unit
        auto set_parent(army &new_parent) -> void;

        // Set the captain of the unit
        auto set_captain(character &new_captain) -> void;

        // Remove the captain of the unit
        auto remove_captain() -> void;

        // Get the captain of the unit
        [[nodiscard]] auto captain() const -> character *;

        // Get the location of the unit
        [[nodiscard]] auto location() const -> province &;

        // Get the path of the unit
        [[nodiscard]] auto path() const -> const std::list<utils::ref<province> > &;

        // Get the travel progress of the unit to the next province
        [[nodiscard]] auto travel_progress() const -> double;

        // Get if the unit is retreating
        [[nodiscard]] auto retreating() const -> bool;

        // Set the destination province for the unit to move towards
        auto set_destination(province &destination) -> void;

        // Move the unit towards its destination province
        auto move() -> void;
    };
}
