#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "ai/action.hpp"

namespace mechanics {
    class ai {
        std::vector<std::unique_ptr<action_base>> actions_;

    protected:
        // Adds an action to the AI's action list
        auto add_action(std::unique_ptr<action_base> &&act) -> void;

    public:
        // Do the AI's actions (if they are valid, and the random chance is met)
        auto run_ai() -> void;
    };

    template<typename Self, typename Other>
    auto make_action(std::function<void(Self &, Other &)> &&valid_func,
        std::function<void(Self &, Other &)> &&action_func, std::function<int(Self &, Other &)> &&weight_func,
        Self &s_param, Other &o_param) -> std::unique_ptr<action_base> {
        return std::make_unique<action<Self, Other>>(std::move(valid_func), std::move(action_func),
            std::move(weight_func), s_param, o_param);
    }
}
