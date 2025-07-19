#include "ai.hpp"
#include <random>

namespace {
    // How unlikely the AI is to perform an action
    constexpr auto inertia = 10.0;
}

namespace mechanics {
    auto ai::add_action(std::unique_ptr<action_base> &&act) -> void { actions_.emplace_back(std::move(act)); }

    auto ai::clear_actions() -> void { actions_.clear(); }

    auto ai::perform_actions() const -> void {
        static std::random_device rd;
        static std::default_random_engine rng{rd()};
        static std::uniform_real_distribution dis{0.0, 1.0};

        const auto random = sqrt(dis(rng));

        for (const auto &action: actions_) {
            if (const auto weight = action->weight(); weight >= 1) {
                if (const auto chance = 1.0 - 1 / (weight * weight / inertia + 1); random < chance) {
                    action->perform();
                }
            }
        }
    }
}
