#include "army.hpp"
#include "character.hpp"
#include "province.hpp"

namespace mechanics {
    army::army(tag &parent_tag) : parent_tag_(parent_tag) {}

    army::~army() { if (has_commander()) { commander_->remove_flag(character_flag_t::commander); } }

    auto army::new_unit(province &location) -> unit & {
        units_.emplace_back(*this, location);
        return units_.back();
    }

    auto army::set_commander(character *new_commander) -> void {
        if (has_commander()) { commander_->remove_flag(character_flag_t::commander); }
        if (new_commander != nullptr) { new_commander->add_flag(character_flag_t::commander); }
        commander_ = new_commander;
    }

    auto army::commander() const -> character * { return commander_; }

    auto army::has_commander() const -> bool { return commander_ != nullptr; }

    auto army::directive() const -> army_directive { return directive_; }

    auto army::set_directive(const army_directive directive) -> void { directive_ = directive; }

    auto army::parent() const -> tag & { return parent_tag_; }

    auto army::set_parent(tag &new_parent) -> void { parent_tag_ = new_parent; }

    auto army::units() const -> const std::list<unit> & { return units_; }
}
