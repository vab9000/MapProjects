#include "army.hpp"
#include "province.hpp"
#include "character.hpp"

army::army(tag *parent_tag) : parent_tag_(parent_tag) {
}

army::~army() {
    if (has_commander()) commander_->remove_flag(character_flag_t::commander);
}

unit &army::new_unit(province *location) {
    units_.emplace_back(this, location);
    return units_.back();
}

void army::set_commander(character *new_commander) {
    if (has_commander()) commander_->remove_flag(character_flag_t::commander);
    if (new_commander != nullptr) new_commander->add_flag(character_flag_t::commander);
    commander_ = new_commander;
}

character &army::commander() const {
    return *commander_;
}

bool army::has_commander() const {
    return commander_ != nullptr;
}

army_directive army::directive() const {
    return directive_;
}

void army::set_directive(const army_directive directive) {
    directive_ = directive;
}

tag &army::parent() const {
    return *parent_tag_;
}

void army::set_parent(tag *new_parent) {
    parent_tag_ = new_parent;
}

const std::list<unit> &army::units() const {
    return units_;
}
