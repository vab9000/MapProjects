#include "character.hpp"

class army;

character::character(const date birthday) {
    traits = std::unordered_set<personality_trait>();
    this->birthday_ = birthday;
    commander_ = false;
    army_ = nullptr;
}

[[nodiscard]] int character::age(const date current_date) const {
    return static_cast<int>(current_date.year) - static_cast<int>(birthday_.year);
}

void character::update_ai() {
}

void character::make_commander(army *army) {
    commander_ = true;
    this->army_ = army;
}

void character::remove_commander() {
    commander_ = false;
    army_ = nullptr;
}

[[nodiscard]] bool character::is_commander() const {
    return commander_;
}
