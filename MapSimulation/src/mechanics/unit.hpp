#pragma once
#include <list>
#include <memory>
#include "pop.hpp"

namespace mechanics {
    class army;
    class province;
    class character;

    class unit {
        pop_container pops_;
        std::reference_wrapper<army> parent_army_;
        character *captain_{nullptr};
        std::reference_wrapper<province> location_;
        std::list<std::reference_wrapper<province> > path_;
        double_t travel_progress_{0.0};
        bool retreating_{false};

    public:
        unit(army &parent_army, province &location);

        ~unit();

        // Add a pop to the unit
        auto add_pop(pop new_pop) -> void;

        // Get the total number of troops in the unit
        [[nodiscard]] auto size() const -> uint_fast32_t;

        // Get the parent army of the unit
        [[nodiscard]] auto parent() const -> army &;

        // Set the parent army of the unit
        auto set_parent(army &new_parent) -> void;

        // Set the captain of the unit
        auto set_captain(character *new_captain) -> void;

        // Get the captain of the unit
        [[nodiscard]] auto captain() const -> character *;

        [[nodiscard]] auto has_captain() const -> bool;

        // Get the location of the unit
        [[nodiscard]] auto location() const -> province &;

        // Get the path of the unit
        [[nodiscard]] auto path() const -> const std::list<std::reference_wrapper<province> > &;

        // Get the travel progress of the unit to the next province
        [[nodiscard]] auto travel_progress() const -> double_t;

        // Get if the unit is retreating
        [[nodiscard]] auto retreating() const -> bool;

        // Set the destination province for the unit to move towards
        auto set_destination(province &destination) -> void;

        // Move the unit towards its destination province
        auto move() -> void;
    };
}
