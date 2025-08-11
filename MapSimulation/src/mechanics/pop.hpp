#pragma once
#include <cstdint>
#include "tickable.hpp"

namespace mechanics {
    class pop final : public tickable {
        uint_fast32_t size_{0U};

    public:
        pop();

        // The number of individuals in the pop
        [[nodiscard]] auto size() const -> uint_fast32_t;

        auto tick(tick_t tick_type) -> void override;
    };
}
