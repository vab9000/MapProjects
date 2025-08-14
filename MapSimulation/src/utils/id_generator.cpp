#include "id_generator.hpp"

namespace utils {
    auto id_generator::next_id() -> unsigned int {
        unsigned int id{};
        if (id_stack_.empty()) { id = id_counter_++; } else {
            id = id_stack_.top();
            id_stack_.pop();
        }
        return id;
    }

    auto id_generator::return_id(const unsigned int id) -> void { id_stack_.push(id); }
}
