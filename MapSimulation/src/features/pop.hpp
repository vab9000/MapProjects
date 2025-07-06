#pragma once

#include <cstdint>
#include <list>

enum class lifestyle_t {
    tribal,
};

class pop {
    uint_fast32_t size_;
    lifestyle_t lifestyle_;

public:
    pop();

    pop(pop &&p) = default;

    pop &operator=(pop &&p) = default;

    // The number of individuals in the pop
    [[nodiscard]] uint_fast32_t size() const;
};

using pop_container = std::list<pop>;

void transfer_pop(pop_container &from, pop_container &to, pop *p);

void transfer_pops(pop_container &from, pop_container &to);
