#include "pop.hpp"

namespace mechanics {
    pop::pop() : lifestyle_(lifestyle_t::tribal) {}

    auto pop::size() const -> uint_fast32_t { return size_; }

    auto transfer_pop(pop_container &from, pop_container &to, pop *p) -> void {
        if (const auto loc = std::ranges::find_if(from,
                                                  [p](const pop &elem) { return &elem == p; });
            loc != from.end()) { to.splice(to.begin(), from, loc); }
    }

    auto transfer_pops(pop_container &from, pop_container &to) -> void { to.splice(to.begin(), from); }
}
