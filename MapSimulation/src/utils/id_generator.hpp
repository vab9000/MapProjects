#pragma once
#include <stack>

namespace utils {
    class id_generator {
        std::stack<unsigned int> id_stack_;
        unsigned int id_counter_{0U};

    public:
        auto next_id() -> unsigned int;

        auto return_id(unsigned int id) -> void;
    };
}
