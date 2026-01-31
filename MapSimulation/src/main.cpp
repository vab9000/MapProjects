#include <print>
#include <stdexcept>
#include "processing/simulation.hpp"

auto main() -> int {
    try {
        processing::simulation::start_simulation();
    } catch (std::runtime_error &e) {
        std::println("Runtime error: {}", e.what());
        return 1;
    }

    return 0;
}
