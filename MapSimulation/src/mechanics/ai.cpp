#include "ai.hpp"
#include <random>

namespace {
    // How unlikely the AI is to perform an action
    constexpr auto inertia = 10.0;
}

namespace mechanics {
    auto ai::add_action(std::unique_ptr<action_base> &&act) -> void { actions_.emplace_back(std::move(act)); }

    auto ai::run_ai() -> void {
        static std::random_device rd;
        static std::default_random_engine rng{rd()};
        static std::uniform_real_distribution dis{0.0, 1.0};

        const auto random = sqrt(dis(rng));

        std::erase_if(actions_, [random](const std::unique_ptr<action_base> &a) {
            if (!*a) { return true; }
            if (const auto weight = **a; weight >= 1) {
                if (const auto chance = 1.0 - 1.0 / (static_cast<double>(weight * weight) / inertia + 1.0);
                    random < chance) { (*a)(); }
            }
            return false;
        });
    }
}
