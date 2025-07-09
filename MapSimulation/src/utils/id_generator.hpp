#pragma once

#include <stack>

class id_generator {
    std::stack<uint_fast32_t> id_stack_;
    uint_fast32_t id_counter_ = 0;

public:
    uint_fast32_t next_id();

    void return_id(uint_fast32_t id);
};