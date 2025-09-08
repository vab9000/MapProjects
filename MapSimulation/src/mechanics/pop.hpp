#pragma once
#include <di_link.hpp>
#include "tickable.hpp"

namespace mechanics {
    class culture;

    class pop final : public tickable {
        unsigned int size_{0U};
        utils::di_link<pop, culture> culture_;

    public:
        explicit pop(culture &cul);

        pop(pop &&other) noexcept = default;

        // The number of individuals in the pop
        [[nodiscard]] auto size() const -> unsigned int;

        auto tick(tick_t tick_type) -> void override;
    };
}
