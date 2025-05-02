#pragma once

#include <vector>
#include <memory>

// How unlikely the AI is to perform an action
constexpr double inertia = 10.0;

class action {
    int weight_cache_;
    int num_cache_ = 0;

    void (*action_func_)(void *, void *);

    void *s_param_;
    void *o_param_;

    int (*weight_func_)(void *, void *);

public:
    action(void (*action)(void *, void *), void (*s_param), void (*o_param), int (*weight_func)(void *, void *));

    ~action();

    void perform() const;

    [[nodiscard]] int get_weight();
};

class ai {
    std::vector<std::unique_ptr<action> > actions_;

public:
    ai();

    void add_action(std::unique_ptr<action> &&action);

    void clear_actions();

    void perform_actions() const;
};

class has_ai {
public:
    ai ai_instance = ai();

    virtual ~has_ai() = default;

    virtual void update_ai() = 0;
};
