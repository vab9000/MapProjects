#include "settlement.hpp"
#include <algorithm>
#include <execution>

namespace mechanics {
    auto settlement::tick(tick_t tick_type) -> void {
        std::for_each(std::execution::seq, pops_.begin(), pops_.end(), [tick_type](const std::unique_ptr<pop> &elem) {
            elem->tick(tick_type);
        });
    }
}
