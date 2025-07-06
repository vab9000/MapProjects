#include "pop.hpp"

pop::pop() : size_(0), lifestyle_(lifestyle_t::tribal) {
}

uint_fast32_t pop::size() const {
    return size_;
}

void transfer_pop(pop_container &from, pop_container &to, pop *p) {
    if (const auto it = std::ranges::find_if(from,
        [p](const pop &elem) { return &elem == p; });
        it != from.end()) {
        to.splice(to.begin(), from, it);
    }
}

void transfer_pops(pop_container &from, pop_container &to) {
    to.splice(to.begin(), from);
}
