#pragma once
#include <list>
#include "unit.hpp"
#include "army/army_properties.hpp"

namespace mechanics {
    class province;
    class character;
    class tag;

    class army {
        army_directive directive_;
        character *commander_ = nullptr;
        std::reference_wrapper<tag> parent_tag_;
        std::list<unit> units_;

    public:
        explicit army(tag &parent_tag);

        ~army();

        // Creates a new unit in the army at the specified location
        [[nodiscard]] auto new_unit(province &location) -> unit &;

        // Sets the commander of the army
        auto set_commander(character *new_commander) -> void;

        // Returns the commander of the army
        [[nodiscard]] auto commander() const -> character *;

        [[nodiscard]] auto has_commander() const -> bool;

        // Returns the directive of the army
        [[nodiscard]] auto directive() const -> army_directive;

        // Sets the directive of the army
        auto set_directive(army_directive directive) -> void;

        // Returns the parent tag of the army
        [[nodiscard]] auto parent() const -> tag &;

        // Sets the parent tag of the army
        auto set_parent(tag &new_parent) -> void;

        // Returns the list of units in the army
        [[nodiscard]] auto units() const -> const std::list<unit> &;
    };
}
