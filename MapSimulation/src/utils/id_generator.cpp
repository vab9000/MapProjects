#include "id_generator.hpp"

uint_fast32_t id_generator::next_id() {
    uint_fast32_t id;
    if (id_stack_.empty()) {
        id = id_counter_++;
    } else {
        id = id_stack_.top();
        id_stack_.pop();
    }
    return id;
}

void id_generator::return_id(const uint_fast32_t id) {
    id_stack_.push(id);
}