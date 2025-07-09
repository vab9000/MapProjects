#pragma once

#include <list>
#include <memory>
#include "pop.hpp"

class army;
class province;
class character;

class unit {
    pop_container pops_;
    army *parent_army_;
    character *captain_ = nullptr;
    province *location_;
    std::list<province *> path_;
    double_t travel_progress_ = 0.0;
    bool retreating_ = false;

public:
    unit(army *parent_army, province *location);

    ~unit();

    // Add a pop to the unit
    void add_pop(pop new_pop);

    // Get the total number of troops in the unit
    [[nodiscard]] uint_fast32_t size() const;

    // Get the parent army of the unit
    [[nodiscard]] army &parent() const;

    // Set the parent army of the unit
    void set_parent(army *new_parent);

    [[nodiscard]] bool has_parent() const;

    // Set the captain of the unit
    void set_captain(character *new_captain);

    // Get the captain of the unit
    [[nodiscard]] character &captain() const;

    [[nodiscard]] bool has_captain() const;

    // Get the location of the unit
    [[nodiscard]] province &location() const;

    // Get the path of the unit
    [[nodiscard]] const std::list<province *> &path() const;

    // Get the travel progress of the unit to the next province
    [[nodiscard]] double travel_progress() const;

    // Get if the unit is retreating
    [[nodiscard]] bool retreating() const;

    // Set the destination province for the unit to move towards
    void set_destination(province *destination);

    // Move the unit towards its destination province
    void move();
};
