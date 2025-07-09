#include "character.hpp"
#include <stack>
#include "../utils/id_generator.hpp"

id_generator character::id_gen_;

character::character(const date birthday, province *location) : birthday_(birthday), location_(location),
                                                                id(id_gen_.next_id()) {
}

character::~character() {
    id_gen_.return_id(id);
}

[[nodiscard]] uint_fast32_t character::age(const date &current_date) const {
    return current_date.year - birthday_.year;
}

const date &character::birthday() const {
    return birthday_;
}

province &character::location() const {
    return *location_;
}

void character::set_location(province *location) {
    location_ = location;
}

void character::update_ai() {
}

void character::add_flag(const character_flag_t flag) {
    flags_.insert(flag);
}

void character::remove_flag(const character_flag_t flag) {
    flags_.erase(flag);
}

bool character::has_flag(const character_flag_t flag) const {
    return flags_.contains(flag);
}

const std::set<character_flag_t> &character::flags() const {
    return flags_;
}

void character::add_trait(const personality_trait_t trait) {
    traits_.insert(trait);
}

void character::remove_trait(const personality_trait_t trait) {
    traits_.erase(trait);
}

bool character::has_trait(const personality_trait_t trait) const {
    return traits_.contains(trait);
}

const std::set<personality_trait_t> &character::traits() const {
    return traits_;
}
