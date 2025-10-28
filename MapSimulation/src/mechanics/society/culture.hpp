#ifndef MECHANICS_CULTURE
#define MECHANICS_CULTURE
#include <di_link.hpp>
#include <numbered.hpp>
#include <vector>

namespace mechanics {
    class pop;

    class culture : public utils::numbered<culture> {
        std::vector<utils::di_link<culture, pop>> pops_;

    public:
        culture();

        culture(const culture &other) = delete;

        auto operator=(const culture &other) -> culture & = delete;

        auto new_pop_link() -> utils::di_link<culture, pop> &;
    };
}
#endif
