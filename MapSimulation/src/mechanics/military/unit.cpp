#include "unit.hpp"
#include <numeric>
#include <utility>
#include "army.hpp"
#include "../society/tag.hpp"
#include "../life/character.hpp"
#include "../province/province.hpp"

namespace mechanics {
    unit::unit(army &parent_army, province &location) : parent_army_(parent_army),
        location_(location) {}

    unit::~unit() { if (captain_ != nullptr) { captain_->roles().remove(role_t::captain); } }

    auto unit::add_pop(pop &new_pop) -> void { pops_.emplace_back(new_pop); }

    auto unit::pops() const -> const std::vector<utils::ref<pop>> & { return pops_; }

    auto unit::pops() -> std::vector<utils::ref<pop>> & { return pops_; }

    auto unit::size() const -> unsigned int {
        return std::accumulate(pops_.begin(), pops_.end(), 0U, [](const unsigned int sum, const pop &p) {
            return sum + p.size();
        });
    }

    auto unit::parent() const -> army & { return parent_army_; }

    auto unit::set_parent(army &new_parent) -> void { parent_army_ = new_parent; }

    auto unit::set_captain(character &new_captain) -> void {
        if (captain_ != nullptr) { captain_->roles().remove(role_t::captain); }
        new_captain.roles().add<role_t::captain>(this);
        captain_ = &new_captain;
    }

    auto unit::remove_captain() -> void {
        if (captain_ != nullptr) {
            captain_->roles().remove(role_t::captain);
            captain_ = nullptr;
        }
    }

    auto unit::captain() const -> character * { return captain_; }

    auto unit::location() const -> province & { return location_; }

    auto unit::path() const -> const std::vector<utils::ref<province>> & { return path_; }

    auto unit::travel_progress() const -> float { return travel_progress_; }

    auto unit::retreating() const -> bool { return retreating_; }

    auto unit::set_destination(province &destination) -> void {
        if (retreating_) { return; }

        const province_connection_func<bool, unit> accessible = [](
            const std::pair<utils::ref<const province>, utils::ref<const province>>
            connection,
            const unit &this_unit) {
            return this_unit.parent().parent().has_army_access(connection.second);
        };
        const province_connection_func<float, unit> cost_modifier = [](
            const std::pair<utils::ref<const province>, utils::ref<const province>>,
            const unit &) {
            return 1.0F;
        };
        path_ = std::move(location_.get().path_to<unit>(
            destination,
            accessible,
            cost_modifier, *this));
    }

    auto unit::move() -> void {
        travel_progress_ += 1.0F / location_.get().distance(path_.back());
        if (travel_progress_ < 100.0F) { return; }

        if (!parent_army_.get().parent().has_army_access(path_.back())) {
            path_.clear();
            travel_progress_ = 0.0F;
            return;
        }

        location_.get().remove_unit(*this);
        location_ = path_.back();
        location_.get().add_unit(*this);
        for (auto &p : pops_) { p.get().set_location(location_.get()); }
        path_.pop_back();
        travel_progress_ = 0.0F;
    }
}
