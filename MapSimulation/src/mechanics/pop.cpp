#include "pop.hpp"

namespace mechanics {
    pop::pop() = default;

    auto pop::size() const -> unsigned int { return size_; }

    auto pop::tick(tick_t tick_type) -> void {}
}
