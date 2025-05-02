#include "ai.hpp"

#include <random>

static auto rng = std::default_random_engine(std::random_device{}());

action::action(void (*action)(void *, void *), void (*s_param), void (*o_param),
               int (*weight_func)(void *, void *)) : weight_cache_(get_weight()),
                                                     action_func_(action),
                                                     s_param_(s_param),
                                                     o_param_(o_param), weight_func_(weight_func) {
}

action::~action() = default;

void action::perform() const {
    action_func_(s_param_, o_param_);
}

[[nodiscard]] int action::get_weight() {
    if (num_cache_ != 0) {
        num_cache_--;
        return weight_cache_;
    }
    num_cache_ = 30;
    return weight_func_(s_param_, o_param_);
}

void ai::add_action(const action& act) {
    actions_.emplace_back(act);
}

void ai::clear_actions() {
    actions_.clear();
}

void ai::perform_actions() {
    auto dis = std::uniform_real_distribution(0.0, 1.0);

    const auto random = dis(rng);

    for (auto &action: actions_) {
        if (const int weight = action.get_weight(); weight >= 1) {
            if (const auto chance = 1.0 - 1 / (weight * weight / inertia + 1); random < chance) {
                action.perform();
            }
        }
    }
}
