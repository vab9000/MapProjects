#pragma once
#include <cstdint>
#include <list>

namespace mechanics {
    enum class lifestyle_t {
        tribal,
    };

    class pop {
        uint_fast32_t size_ = 0;
        lifestyle_t lifestyle_;

    public:
        pop();

        pop(pop &&p) = default;

        auto operator=(pop &&p) -> pop & = default;

        // The number of individuals in the pop
        [[nodiscard]] auto size() const -> uint_fast32_t;
    };

    using pop_container = std::list<pop>;

    auto transfer_pop(pop_container &from, pop_container &to, pop *p) -> void;

    auto transfer_pops(pop_container &from, pop_container &to) -> void;
}
