#pragma once
#include <functional>
#include <list>
#include <memory>
#include "ai/action.hpp"

namespace mechanics {
    class ai {
        std::list<std::unique_ptr<action_base> > actions_;

    protected:
        // Adds an action to the AI's action list
        auto add_action(std::unique_ptr<action_base> &&act) -> void;

        // Removes all actions from the AI's action list
        auto clear_actions() -> void;

        // Does all actions if weight is above a certain threshold
        auto perform_actions() const -> void;

    public:
        virtual ~ai() = default;

        // Updates the AI's actions based on the current state
        virtual auto update_ai() -> void = 0;
    };

    template<typename Self, typename Other>
    auto make_action(std::function<void(Self *, Other *)> &&action_func, Self *s_param,
                     Other *o_param,
                     std::function<int_fast32_t(Self *, Other *)> &&weight_func) -> std::unique_ptr<action_base> && {
        return std::make_unique<action<Self, Other> >(std::move(action_func), s_param, o_param, std::move(weight_func));
    }
}
