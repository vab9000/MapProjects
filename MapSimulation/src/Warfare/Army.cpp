#include "Army.hpp"
#include <vector>
#include "../Base/AI.hpp"
#include "../Base/Province.hpp"
#include "../Populations/Character.hpp"
#include "../Tags/Tag.hpp"

unit::unit(const army &parent_army, const province &location) {
    size = 0;
    this->location = &const_cast<province &>(location);
    this->parent_army = &const_cast<army &>(parent_army);
    path = std::vector<province *>();
    travel_progress = 0;
    general = nullptr;
    retreating = false;
    speed = 1.0;
}

void unit::set_destination(const province &destination) {
    const auto generated_path = location->get_path_to(
        destination,
        [](const province &province, void *param) {
            const auto this_unit = static_cast<unit *>(param);
            return this_unit->parent_army->parent_tag->has_army_access(province);
        },
        [](const province &, void *) { return 1.0; }, this);
    for (const auto &province: generated_path) {
        path.emplace_back(province);
    }
}

void unit::move() {
    travel_progress += speed;
    if (travel_progress >= 100) {
        location = path.at(0);
        path.erase(path.begin());
        travel_progress = 0;
    }
}

army::army(const tag &parent_tag) {
    this->parent_tag = &const_cast<tag &>(parent_tag);
    units = std::vector<std::unique_ptr<unit> >();
    directive_ = {.type = army_directive_type::attack, .target = nullptr};
    commander_ = nullptr;
}

void set_unit_destination(void *s_param, void *o_param) {
    const auto c_unit = static_cast<unit *>(s_param);
    const auto destination = static_cast<province *>(o_param);
    c_unit->set_destination(*destination);
}

int get_set_unit_destination_weight(void *s_param, void *o_param) {
    // const auto unit = static_cast<Unit *>(sParam);
    // const auto province = static_cast<Province*>(oParam);
    return 100;
}

[[nodiscard]] unit *army::new_unit(const province &location) {
    auto new_unit = std::make_unique<unit>(*this, location);
    units.push_back(std::move(new_unit));
    return units.back().get();
}
