#include "character.hpp"
#include <stack>

namespace mechanics {
    utils::id_generator character::id_gen_;

    character::character(const date birthday, province &location) : birthday_(birthday), location_(location),
                                                                    id(id_gen_.next_id()) {
    }

    character::~character() {
        id_gen_.return_id(id);
    }

    [[nodiscard]] auto character::age(const date &current_date) const -> uint_fast32_t {
        return current_date.year - birthday_.year;
    }

    auto character::birthday() const -> const date & {
        return birthday_;
    }

    auto character::location() const -> province & {
        return location_;
    }

    auto character::set_location(province &location) -> void {
        location_ = location;
    }

    auto character::update_ai() -> void {
    }

    auto character::add_flag(const character_flag_t flag) -> void {
        flags_.insert(flag);
    }

    auto character::remove_flag(const character_flag_t flag) -> void {
        flags_.erase(flag);
    }

    auto character::has_flag(const character_flag_t flag) const -> bool {
        return flags_.contains(flag);
    }

    auto character::flags() const -> const std::set<character_flag_t> & {
        return flags_;
    }

    auto character::add_trait(const personality_trait_t trait) -> void {
        traits_.insert(trait);
    }

    auto character::remove_trait(const personality_trait_t trait) -> void {
        traits_.erase(trait);
    }

    auto character::has_trait(const personality_trait_t trait) const -> bool {
        return traits_.contains(trait);
    }

    auto character::traits() const -> const std::set<personality_trait_t> & {
        return traits_;
    }
}