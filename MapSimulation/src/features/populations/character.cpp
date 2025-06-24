#include "character.hpp"
#include "../warfare/army.hpp"

character::character(const date birthday) : birthday_(birthday), commander_(false), army_(nullptr) {
}

[[nodiscard]] int character::age(const date current_date) const {
    return static_cast<int>(current_date.year) - static_cast<int>(birthday_.year);
}

void character::update_ai() {
}

void character::make_commander(army &army) {
    commander_ = true;
    army_ = &army;
    army.set_commander(*this);
}

void character::remove_commander() {
    if (army_ != nullptr) {
        army_->remove_commander();
    }
    commander_ = false;
    army_ = nullptr;
}

[[nodiscard]] bool character::is_commander() const {
    return commander_;
}
