#pragma once

#include <vector>
#include <random>

// How unlikely the AI is to perform an action
constexpr double inertia = 10.0;

class action {
    int weight_cache_;
    int num_cache_ = 0;

public:
    void (*action_func)(void *, void *);

    void *s_param;
    void *o_param;

    int (*weight_func)(void *, void *);

    action(void (*action)(void *, void *), void (*s_param), void (*o_param), int (*get_weight)(void *, void *)) {
        this->action_func = action;
        this->s_param = s_param;
        this->o_param = o_param;
        this->weight_func = get_weight;
        weight_cache_ = this->get_weight();
    }

    ~action() = default;

    void perform() const {
        action_func(s_param, o_param);
    }

    [[nodiscard]] int get_weight() {
        if (num_cache_ != 0) {
            num_cache_--;
            return weight_cache_;
        }
        num_cache_ = 30;
        return weight_func(s_param, o_param);
    }
};

class ai {
    std::vector<std::unique_ptr<action> > actions_;
    std::default_random_engine gen_;

public:
    ai() {
        actions_ = std::vector<std::unique_ptr<action> >();
        std::random_device rd;
        gen_ = std::default_random_engine(rd());
    }

    void add_action(std::unique_ptr<action> &&action) {
        actions_.push_back(std::move(action));
    }

    void clear_actions() {
        actions_.clear();
    }

    void perform_actions() {
        auto dis = std::uniform_real_distribution(0.0, 1.0);

        const auto random = dis(gen_);

        for (const auto &action: actions_) {
            if (const int weight = action->get_weight(); weight >= 1) {
                if (const auto chance = 1.0 - (1 / ((weight * weight) / inertia + 1)); random < chance) {
                    action->perform();
                }
            }
        }
    }
};

class has_ai {
public:
    ai ai;

    has_ai() {
        ai = ai::ai();
    }

    virtual ~has_ai() = default;

    virtual void update_ai() = 0;
};
