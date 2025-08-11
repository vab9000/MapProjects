#pragma once
#include <list>
#include "unit.hpp"

namespace mechanics {
    class province;
    class character;
    class tag;

    class army {
        std::optional<std::reference_wrapper<character>> commander_ = std::nullopt;
        std::reference_wrapper<tag> parent_tag_;
        std::list<unit> units_;

    public:
        explicit army(tag &parent_tag);

        ~army();

        // Creates a new unit in the army at the specified location
        [[nodiscard]] auto new_unit(province &location) -> unit &;

        // Sets the commander of the army
        auto set_commander(character &new_commander) -> void;

        // Removes the commander of the army
        auto remove_commander() -> void;

        // Returns the commander of the army
        [[nodiscard]] auto commander() const -> std::optional<std::reference_wrapper<character>>;

        // Returns the parent tag of the army
        [[nodiscard]] auto parent() const -> tag &;

        // Sets the parent tag of the army
        auto set_parent(tag &new_parent) -> void;

        // Returns the list of units in the army
        [[nodiscard]] auto units() const -> const std::list<unit> &;
    };
}
