#ifndef MECHANICS_CULTURE
#define MECHANICS_CULTURE
#include <numbered.hpp>
#include <reference.hpp>
#include <unordered_set>

namespace mechanics {
    class pop;

    class culture : public utils::numbered<culture> {
        std::unordered_set<utils::ref<pop>, utils::ref<pop>::hash> pops_;

    public:
        culture();

        culture(const culture &other) = delete;

        auto operator=(const culture &other) -> culture & = delete;

        auto add_pop(pop &p) -> void;

        auto remove_pop(pop &p) -> void;
    };
}
#endif
