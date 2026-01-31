#include "pop.hpp"
#include "../society/culture.hpp"

namespace mechanics {
    pop::pop(province &prov, culture &cul) : location_(prov), culture_(cul), properties_{} {
        cul.add_pop(*this);
    }

    pop::~pop() {
        culture_.get().remove_pop(*this);
    }

    auto pop::size() const -> unsigned int { return size_; }

    auto pop::tick(tick_t tick_type) -> void {}

    auto pop::location() const -> province & { return location_; }
}
