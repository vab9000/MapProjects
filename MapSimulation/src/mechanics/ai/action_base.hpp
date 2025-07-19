#pragma once

#include <cstdint>

namespace mechanics {
    class action_base {
    public:
        virtual ~action_base() = default;

        virtual auto perform() const -> void = 0;

        [[nodiscard]] virtual auto weight() const -> int_fast32_t = 0;
    };
}
