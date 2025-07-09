#include "pop.hpp"

pop::pop() : lifestyle_(lifestyle_t::tribal) {
}

uint_fast32_t pop::size() const {
    return size_;
}

void transfer_pop(pop_container &from, pop_container &to, pop *p) {
    if (const auto loc = std::ranges::find_if(from,
        [p](const pop &elem) { return &elem == p; });
        loc != from.end()) {
        to.splice(to.begin(), from, loc);
    }
}

void transfer_pops(pop_container &from, pop_container &to) {
    to.splice(to.begin(), from);
}
