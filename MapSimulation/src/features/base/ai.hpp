#pragma once

#include <vector>

// How unlikely the AI is to perform an action
constexpr double inertia = 10.0;

class action {
    int weight_cache_;
    int num_cache_ = 0;

    void (* const action_func_)(void *, void *);

    void * const s_param_;
    void * const o_param_;

    int (* const weight_func_)(void *, void *);

public:
    action(void (*action)(void *, void *), void (*s_param), void (*o_param), int (*weight_func)(void *, void *));

    ~action();

    void perform() const;

    [[nodiscard]] int weight();
};

class ai {
    std::vector<action> actions_ = {};

public:
    void add_action(const action& act);

    void clear_actions();

    void perform_actions();
};

class has_ai {
public:
    ai ai_instance = ai();

    virtual ~has_ai() = default;

    virtual void update_ai() = 0;
};
