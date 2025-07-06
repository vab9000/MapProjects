#include "unit.hpp"
#include <numeric>
#include <utility>
#include "province.hpp"
#include "army.hpp"
#include "tag.hpp"
#include "character.hpp"

unit::unit(army *parent_army, province *location) : parent_army_(parent_army),
                                                    location_(location),
                                                    travel_progress_(0) {
}

unit::~unit() {
    transfer_pops(pops_, location_->pops());
    if (has_captain()) {
        captain_->remove_flag(character_flag_t::captain);
    }
}

void unit::add_pop(pop new_pop) {
    pops_.emplace_back(std::move(new_pop));
}

uint_fast32_t unit::size() const {
    return std::accumulate(pops_.begin(), pops_.end(), 0, [](const uint_fast32_t sum, const pop &p) {
        return sum + p.size();
    });
}

army &unit::parent() const {
    return *parent_army_;
}

void unit::set_parent(army *new_parent) {
    parent_army_ = new_parent;
}

bool unit::has_parent() const {
    return parent_army_ != nullptr;
}

void unit::set_captain(character *new_captain) {
    if (has_captain()) captain_->remove_flag(character_flag_t::captain);
    if (new_captain != nullptr) new_captain->add_flag(character_flag_t::captain);
    captain_ = new_captain;
}

character &unit::captain() const {
    return *captain_;
}

bool unit::has_captain() const {
    return captain_ != nullptr;
}

province &unit::location() const {
    return *location_;
}

const std::list<province *> &unit::path() const {
    return path_;
}

double unit::travel_progress() const {
    return travel_progress_;
}

bool unit::retreating() const {
    return retreating_;
}

void unit::set_destination(const province &destination) {
    if (retreating_) return;

    auto generated_path = location_->path_to<unit>(
        destination,
        [](const province &self, const province &other, const unit *this_unit) {
            return this_unit->parent().parent().has_army_access(other);
        },
        [](const province &self, const province &other, unit *this_unit) { return 1.0; }, this);
    path_ = std::move(generated_path);
}

void unit::move() {
    travel_progress_ += 1.0;
    if (travel_progress_ < 100.0) return;

    if (!parent_army_->parent().has_army_access(*path_.front())) {
        path_.clear();
        travel_progress_ = 0.0;
        return;
    }

    location_ = path_.front();
    path_.erase(path_.begin());
    travel_progress_ = 0.0;
}
