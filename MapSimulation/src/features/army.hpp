#pragma once

#include <list>
#include "unit.hpp"
#include "army/army_properties.hpp"

class province;
class character;
class tag;

class army {
    army_directive directive_;
    character *commander_ = nullptr;
    tag *parent_tag_;
    std::list<unit> units_;

public:
    explicit army(tag *parent_tag);

    ~army();

    // Creates a new unit in the army at the specified location
    [[nodiscard]] unit &new_unit(province *location);

    // Sets the commander of the army
    void set_commander(character *new_commander);

    // Returns the commander of the army
    [[nodiscard]] character &commander() const;

    [[nodiscard]] bool has_commander() const;

    // Returns the directive of the army
    [[nodiscard]] army_directive directive() const;

    // Sets the directive of the army
    void set_directive(army_directive directive);

    // Returns the parent tag of the army
    [[nodiscard]] tag &parent() const;

    // Sets the parent tag of the army
    void set_parent(tag *new_parent);

    // Returns the list of units in the army
    [[nodiscard]] const std::list<unit> &units() const;
};
