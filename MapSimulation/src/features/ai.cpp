#include "ai.hpp"

#include <random>
#include <cmath>

// How unlikely the AI is to perform an action
constexpr double inertia = 10.0;

template<typename Self, typename Other>
std::unique_ptr<action_base> &&make_action(std::function<void(Self *, Other *)> &&action_func, Self *s_param,
                                           Other *o_param, std::function<int_fast32_t(Self *, Other *)> &&weight_func) {
    return std::make_unique<action<Self, Other> >(std::move(action_func), s_param, o_param, std::move(weight_func));
}

void ai::add_action(std::unique_ptr<action_base> &&act) {
    actions_.emplace_back(std::move(act));
}

void ai::clear_actions() {
    actions_.clear();
}

void ai::perform_actions() const {
    static auto rng = std::default_random_engine(std::random_device{}());
    static auto dis = std::uniform_real_distribution(0.0, 1.0);

    const auto random = sqrt(dis(rng));

    for (const auto &action: actions_) {
        if (const auto weight = action->weight(); weight >= 1) {
            if (const auto chance = 1.0 - 1 / (weight * weight / inertia + 1); random < chance) {
                action->perform();
            }
        }
    }
}
