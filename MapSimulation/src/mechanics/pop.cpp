#include "pop.hpp"
#include "culture.hpp"

namespace mechanics {
    pop::pop(culture &cul) : culture_(*this, cul.new_pop_link()) {}

    auto pop::size() const -> unsigned int { return size_; }

    auto pop::tick(tick_t tick_type) -> void {}
}
