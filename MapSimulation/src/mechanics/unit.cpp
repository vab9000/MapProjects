#include "unit.hpp"
#include <numeric>
#include <utility>
#include "army.hpp"
#include "character.hpp"
#include "province.hpp"
#include "tag.hpp"

namespace mechanics {
    unit::unit(army &parent_army, province &location) : parent_army_(parent_army),
                                                        location_(location) {
    }

    unit::~unit() {
        transfer_pops(pops_, location_.get().pops());
        if (has_captain()) {
            captain_->remove_flag(character_flag_t::captain);
        }
    }

    auto unit::add_pop(pop new_pop) -> void {
        pops_.emplace_back(std::move(new_pop));
    }

    auto unit::size() const -> uint_fast32_t {
        return std::accumulate(pops_.begin(), pops_.end(), 0, [](const uint_fast32_t sum, const pop &p) {
            return sum + p.size();
        });
    }

    auto unit::parent() const -> army & {
        return parent_army_;
    }

    auto unit::set_parent(army &new_parent) -> void {
        parent_army_ = new_parent;
    }

    auto unit::set_captain(character *new_captain) -> void {
        if (has_captain()) {
            captain_->remove_flag(character_flag_t::captain);
        }
        if (new_captain != nullptr) {
            new_captain->add_flag(character_flag_t::captain);
        }
        captain_ = new_captain;
    }

    auto unit::captain() const -> character * {
        return captain_;
    }

    auto unit::has_captain() const -> bool {
        return captain_ != nullptr;
    }

    auto unit::location() const -> province & {
        return location_;
    }

    auto unit::path() const -> const std::list<std::reference_wrapper<province>> & {
        return path_;
    }

    auto unit::travel_progress() const -> double_t {
        return travel_progress_;
    }

    auto unit::retreating() const -> bool {
        return retreating_;
    }

    auto unit::set_destination(province &destination) -> void {
        if (retreating_) {
            return;
        }

        auto generated_path = location_.get().path_to<unit>(
            destination,
            [](const province &self, const province &other, const unit *this_unit) {
                return this_unit->parent().parent().has_army_access(other);
            },
            [](const province &self, const province &other, unit *this_unit) { return 1.0; }, this);
        path_ = std::move(generated_path);
    }

    auto unit::move() -> void {
        travel_progress_ += 1.0;
        if (travel_progress_ < 100.0) {
            return;
        }

        if (!parent_army_.get().parent().has_army_access(path_.front())) {
            path_.clear();
            travel_progress_ = 0.0;
            return;
        }

        location_ = path_.front();
        path_.erase(path_.begin());
        travel_progress_ = 0.0;
    }
}
