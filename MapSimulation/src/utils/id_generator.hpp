#pragma once
#include <stack>

namespace utils {
    class id_generator {
        std::stack<uint_fast32_t> id_stack_;
        uint_fast32_t id_counter_{0U};

    public:
        auto next_id() -> uint_fast32_t;

        auto return_id(uint_fast32_t id) -> void;
    };
}
