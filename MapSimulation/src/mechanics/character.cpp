#include "character.hpp"
#include <stack>

namespace mechanics {
    utils::id_generator character::id_gen_;

    character::character(const date birthday, province &location) : birthday_(birthday), location_(location),
        id(id_gen_.next_id()) {}

    character::~character() { id_gen_.return_id(id); }

    [[nodiscard]] auto character::age(const date &current_date) const -> uint_fast32_t {
        return current_date.year - birthday_.year;
    }

    auto character::birthday() const -> const date & { return birthday_; }

    auto character::location() const -> province & { return location_; }

    auto character::set_location(province &location) -> void { location_ = location; }

    auto character::update_ai() -> void {}

    auto character::flags() const -> const utils::flags<character_flag_t> & { return flags_; }

    auto character::flags() -> utils::flags<character_flag_t> & { return flags_; }

    auto character::traits() const -> const utils::flags<personality_trait_t> & { return traits_; }

    auto character::traits() -> utils::flags<personality_trait_t> & { return traits_; }
}
