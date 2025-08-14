#include "character.hpp"
#include <stack>

namespace mechanics {
    character::character(const date &birthday, province &location) : birthday_(birthday), location_(location) {}

    character::~character() = default;

    auto character::is_alive() const -> bool { return alive_; }

    auto character::kill() -> void { alive_ = false; }

    [[nodiscard]] auto character::age(const date &current_date) const -> int {
        return (current_date - birthday_) / 365;
    }

    auto character::birthday() const -> const date & { return birthday_; }

    auto character::location() const -> province & { return location_; }

    auto character::set_location(province &location) -> void { location_ = location; }

    auto character::flags() const -> const utils::flags<character_flag_t> & { return flags_; }

    auto character::flags() -> utils::flags<character_flag_t> & { return flags_; }

    auto character::traits() const -> const utils::flags<personality_trait_t> & { return traits_; }

    auto character::traits() -> utils::flags<personality_trait_t> & { return traits_; }

    auto character::tick(tick_t tick_type) -> void {}
}
