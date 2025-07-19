#include "id_generator.hpp"

namespace utils {
    auto id_generator::next_id() -> uint_fast32_t {
        uint_fast32_t id;
        if (id_stack_.empty()) { id = id_counter_++; } else {
            id = id_stack_.top();
            id_stack_.pop();
        }
        return id;
    }

    auto id_generator::return_id(const uint_fast32_t id) -> void { id_stack_.push(id); }
}
