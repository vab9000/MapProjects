#pragma once

#include <list>
#include <memory>
#include "pop.hpp"

class army;
class province;
class character;

class unit {
    std::list<std::pair<province *, std::unique_ptr<pop> > > pops_;
    army *parent_army_;
    std::weak_ptr<character> captain_;
    province *location_;
    std::list<province *> path_;
    double_t travel_progress_;
    bool retreating_;

public:
    unit(army *parent_army, province *location);

    ~unit();

    // Add a pop to the unit, transferring it from the home province
    void add_pop(province *home, std::unique_ptr<pop> &&new_pop);

    // Get the total number of troops in the unit
    [[nodiscard]] size_t size() const;

    // Get the parent army of the unit
    [[nodiscard]] army &parent() const;

    // Set the parent army of the unit
    void set_parent(army *new_parent);

    // Set the captain of the unit
    void set_captain(std::weak_ptr<character> new_captain);

    // Get the captain of the unit
    [[nodiscard]] const std::weak_ptr<character> &captain() const;

    // Get the location of the unit
    [[nodiscard]] province &location() const;

    // Get the path of the unit
    [[nodiscard]] const std::list<province *> &path() const;

    // Get the travel progress of the unit to the next province
    [[nodiscard]] double travel_progress() const;

    // Get if the unit is retreating
    [[nodiscard]] bool retreating() const;

    // Set the destination province for the unit to move towards
    void set_destination(const province &destination);

    // Move the unit towards its destination province
    void move();
};
