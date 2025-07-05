#include "army.hpp"
#include "province.hpp"
#include "character.hpp"

army::army(tag *parent_tag) : directive_(army_directive{army_directive_type::none, nullptr}), parent_tag_(parent_tag) {
}

unit &army::new_unit(province *location) {
    units_.emplace_back(std::make_unique<unit>(this, location));
    return *units_.back();
}

void army::set_commander(std::weak_ptr<character> commander) {
    if (!commander_.expired()) commander_.lock()->remove_flag(character_flag_t::commander);
    if (!commander.expired()) commander.lock()->add_flag(character_flag_t::commander);
    commander_ = std::move(commander);
}

const std::weak_ptr<character> &army::commander() const {
    return commander_;
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

const std::list<std::unique_ptr<unit> > &army::units() const {
    return units_;
}
