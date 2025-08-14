#pragma once
#include <cstdint>
#include "tickable.hpp"

namespace mechanics {
    class pop final : public tickable {
        unsigned int size_{0U};

    public:
        pop();

        // The number of individuals in the pop
        [[nodiscard]] auto size() const -> unsigned int;

        auto tick(tick_t tick_type) -> void override;
    };
}
