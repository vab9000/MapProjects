#include "culture.hpp"

namespace mechanics {
    culture::culture() = default;

    auto culture::add_pop(pop &p) -> void {
        pops_.insert(p);
    }

    auto culture::remove_pop(pop &p) -> void {
        pops_.erase(p);
    }
}
