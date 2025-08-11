#include "army.hpp"
#include "character.hpp"
#include "province.hpp"

namespace mechanics {
    army::army(tag &parent_tag) : parent_tag_(parent_tag) {}

    army::~army() { if (commander_.has_value()) { commander_->get().flags().remove(character_flag_t::commander); } }

    auto army::new_unit(province &location) -> unit & {
        units_.emplace_back(*this, location);
        return units_.back();
    }

    auto army::set_commander(character &new_commander) -> void {
        if (commander_.has_value()) { commander_->get().flags().remove(character_flag_t::commander); }
        new_commander.flags().add(character_flag_t::commander);
        commander_ = new_commander;
    }

    auto army::remove_commander() -> void {
        if (commander_.has_value()) {
            commander_->get().flags().remove(character_flag_t::commander);
            commander_ = std::nullopt;
        }
    }

    auto army::commander() const -> std::optional<std::reference_wrapper<character>> { return commander_; }

    auto army::parent() const -> tag & { return parent_tag_; }

    auto army::set_parent(tag &new_parent) -> void { parent_tag_ = new_parent; }

    auto army::units() const -> const std::list<unit> & { return units_; }
}
