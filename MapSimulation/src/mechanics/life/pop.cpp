#include "pop.hpp"
#include "../society/culture.hpp"

namespace mechanics {
    pop::pop(province &prov, culture &cul) : location_(prov), culture_(*this, cul.new_pop_link()), properties_{} {}

    auto pop::size() const -> unsigned int { return size_; }

    auto pop::tick(tick_t tick_type) -> void {}

    auto pop::location() const -> province & { return location_; }

    auto pop::set_location(province &prov) -> void { location_ = prov; }
}
