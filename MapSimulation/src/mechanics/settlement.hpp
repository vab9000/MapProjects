#pragma once
#include "pop.hpp"

namespace mechanics {
    class settlement {
        std::vector<std::unique_ptr<pop>> pops_;

    public:
        settlement() = default;

        auto tick(tick_t tick_type) -> void;
    };
}
