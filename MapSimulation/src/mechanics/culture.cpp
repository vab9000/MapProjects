#include "culture.hpp"

namespace mechanics {
    culture::culture() = default;

    auto culture::new_pop_link() -> utils::di_link<culture, pop> & {
        if (constexpr auto erase_multiple = 8UZ; pops_.size() % erase_multiple == 0UZ) {
            std::erase_if(pops_, [](const utils::di_link<culture, pop> &link) {
                return !link.has_value();
            });
        }
        pops_.emplace_back(*this);
        return pops_.back();
    }
}
