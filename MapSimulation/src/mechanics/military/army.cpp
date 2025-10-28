#include "army.hpp"
#include "../life/character.hpp"
#include "../province/province.hpp"

namespace mechanics {
    army::army(tag &parent_tag) : parent_tag_(parent_tag) {}

    army::~army() { if (commander_ != nullptr) { commander_->roles().remove(role_t::commander); } }

    auto army::new_unit(province &location) -> unit & {
        units_.emplace_back(std::make_unique<unit>(*this, location));
        return *units_.back();
    }

    auto army::set_commander(character &new_commander) -> void {
        if (commander_ != nullptr) { commander_->roles().remove(role_t::commander); }
        new_commander.roles().add<role_t::commander>(this);
        commander_ = &new_commander;
    }

    auto army::remove_commander() -> void {
        if (commander_ != nullptr) {
            commander_->roles().remove(role_t::commander);
            commander_ = nullptr;
        }
    }

    auto army::commander() const -> character * { return commander_; }

    auto army::parent() const -> tag & { return parent_tag_; }

    auto army::set_parent(tag &new_parent) -> void { parent_tag_ = new_parent; }

    auto army::units() const -> const std::vector<std::unique_ptr<unit>> &{ return units_; }
}
