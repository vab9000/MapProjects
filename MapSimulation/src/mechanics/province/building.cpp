#include "building.hpp"

namespace mechanics {
    building::building(const utils::ref<province> location, pop &owner) : location_(location), owner_{.pop = owner},
        owner_type_(owner_type_t::pop) {}

    building::building(const utils::ref<province> location, tag &owner) : location_(location), owner_{.tag = owner},
        owner_type_(owner_type_t::tag) {}

    building::building(const utils::ref<province> location, character &owner) : location_(location),
        owner_{.character = owner}, owner_type_(owner_type_t::character) {}

    auto building::location() const -> province & { return location_; }

    auto building::workers() const -> const std::vector<employee> & { return workers_; }

    auto building::owner_type() const -> owner_type_t { return owner_type_; }
}
