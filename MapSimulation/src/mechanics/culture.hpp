#pragma once
#include <di_link.hpp>
#include <vector>

namespace mechanics {
    class pop;

    class culture {
        std::vector<utils::di_link<culture, pop>> pops_;

    public:
        culture();

        culture(const culture &other) = delete;

        auto operator=(const culture &other) -> culture & = delete;

        auto new_pop_link() -> utils::di_link<culture, pop> &;
    };
}
