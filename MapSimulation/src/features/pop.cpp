#include "pop.hpp"

pop::pop() : size_(0), lifestyle_(lifestyle_t::tribal) {
}

size_t pop::size() const {
    return size_;
}
