#include "army.hpp"
#include <vector>
#include "../base/province.hpp"
#include "../populations/character.hpp"
#include "../tags/tag.hpp"

unit::unit(army &parent_army, province &location) : size(0), location(&location),
                                                    parent_army(&parent_army),
                                                    travel_progress(0), general(nullptr), retreating(false),
                                                    speed(0.0) {
}

void unit::set_destination(const province &destination) {
    const auto generated_path = location->get_path_to(
        destination,
        [](const province &province, void *param) {
            const auto this_unit = static_cast<unit *>(param);
            return this_unit->parent_army->parent_tag->has_army_access(province);
        },
        [](const province &, void *) { return 1.0; }, this);
    path = generated_path;
}

void unit::move() {
    travel_progress += speed;
    if (travel_progress >= 100) {
        location = path.at(0);
        path.erase(path.begin());
        travel_progress = 0;
    }
}

army::army(tag &parent_tag) : commander_(nullptr), parent_tag(&parent_tag) {
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

[[nodiscard]] unit &army::new_unit(province &location) {
    units.emplace_back(unit(*this, location));
    return units.back();
}
