#pragma once

#include <list>
#include <functional>
#include <memory>
#include "ai/action.hpp"

class ai {
    std::list<std::unique_ptr<action_base> > actions_;

protected:
    // Adds an action to the AI's action list
    void add_action(std::unique_ptr<action_base> &&act);

    // Removes all actions from the AI's action list
    void clear_actions();

    // Does all actions if weight is above a certain threshold
    void perform_actions() const;

public:
    virtual ~ai() = default;

    // Updates the AI's actions based on the current state
    virtual void update_ai() = 0;
};

template<typename Self, typename Other>
std::unique_ptr<action_base> &&make_action(std::function<void(Self *, Other *)> &&action_func, Self *s_param,
                                           Other *o_param,
                                           std::function<int_fast32_t(Self *, Other *)> &&weight_func) {
    return std::make_unique<action<Self, Other> >(std::move(action_func), s_param, o_param, std::move(weight_func));
}
