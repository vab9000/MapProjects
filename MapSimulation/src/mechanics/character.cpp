#include "character.hpp"
#include <stack>

namespace mechanics {
    utils::id_generator character::id_gen_;

    character::character(const date &birthday, province &location) : birthday_(birthday), location_(location),
        id_(id_gen_.next_id()) {}

    character::~character() { id_gen_.return_id(id_); }

    auto character::id() const -> unsigned int { return id_; }

    auto character::is_alive() const -> bool { return alive_; }

    auto character::kill() -> void { alive_ = false; }

    [[nodiscard]] auto character::age(const date &current_date) const -> unsigned int {
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
