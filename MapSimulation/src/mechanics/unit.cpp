#include "unit.hpp"
#include <numeric>
#include <utility>
#include "army.hpp"
#include "character.hpp"
#include "province.hpp"
#include "tag.hpp"

namespace mechanics {
    unit::unit(army &parent_army, province &location) : parent_army_(parent_army),
        location_(location) {}

    unit::~unit() {
        if (captain_.has_value()) { captain_->get().flags().remove(character_flag_t::captain); }
    }

    auto unit::add_pop(pop& new_pop) -> void { pops_.emplace_back(new_pop); }

    auto unit::pops() const -> const std::vector<std::reference_wrapper<pop>> & { return pops_; }

    auto unit::pops() -> std::vector<std::reference_wrapper<pop>> & { return pops_; }

    auto unit::size() const -> unsigned int {
        return std::accumulate(pops_.begin(), pops_.end(), 0U, [](const unsigned int sum, const pop &p) {
            return sum + p.size();
        });
    }

    auto unit::parent() const -> army & { return parent_army_; }

    auto unit::set_parent(army &new_parent) -> void { parent_army_ = new_parent; }

    auto unit::set_captain(character &new_captain) -> void {
        if (captain_.has_value()) { captain_->get().flags().remove(character_flag_t::captain); }
        new_captain.flags().add(character_flag_t::captain);
        captain_ = new_captain;
    }

    auto unit::remove_captain() -> void {
        if (captain_.has_value()) {
            captain_->get().flags().remove(character_flag_t::captain);
            captain_ = std::nullopt;
        }
    }

    auto unit::captain() const -> std::optional<std::reference_wrapper<character>> { return captain_; }

    auto unit::location() const -> province & { return location_; }

    auto unit::path() const -> const std::list<std::reference_wrapper<province>> & { return path_; }

    auto unit::travel_progress() const -> double { return travel_progress_; }

    auto unit::retreating() const -> bool { return retreating_; }

    auto unit::set_destination(province &destination) -> void {
        if (retreating_) { return; }

        const std::function accessible = [](const std::pair<std::reference_wrapper<const province>, std::reference_wrapper<const province>>
            connection,
            const unit &this_unit) {
            return this_unit.parent().parent().has_army_access(connection.second);
        };
        const std::function cost_modifier = [](const std::pair<std::reference_wrapper<const province>, std::reference_wrapper<const province>>
            connection,
            const unit &this_unit) {
            return 1.0;
        };
        auto generated_path = location_.get().path_to<unit>(
            destination,
            accessible,
            cost_modifier, *this);
        path_ = std::move(generated_path);
    }

    auto unit::move() -> void {
        travel_progress_ += 1.0;
        if (travel_progress_ < 100.0) { return; }

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
