#include "character.hpp"

namespace mechanics {
    character::character(const date &birthday, province &location) : birthday_(birthday), location_(location) {}

    auto character::is_alive() const -> bool { return alive_; }

    auto character::kill() -> void { alive_ = false; }

    [[nodiscard]] auto character::age(const date &current_date) const -> int {
        return (current_date - birthday_) / 365;
    }

    auto character::birthday() const -> const date & { return birthday_; }

    auto character::location() const -> province & { return location_; }

    auto character::set_location(province &location) -> void { location_ = location; }

    auto character::roles() const -> const utils::tied_flags<role_t, role_type> &{ return roles_; }

    auto character::roles() -> utils::tied_flags<role_t, role_type> &{ return roles_; }

    auto character::traits() const -> const magic_enum::containers::set<personality_trait_t> &{ return traits_; }

    auto character::traits() -> magic_enum::containers::set<personality_trait_t> &{ return traits_; }

    auto character::tick(tick_t tick_type) -> void {}
}
