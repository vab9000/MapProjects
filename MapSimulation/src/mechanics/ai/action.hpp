#pragma once
#include <functional>
#include "action_base.hpp"

namespace mechanics {
    template<typename Self, typename Other>
    class action final : public action_base {
        std::function<void(Self *, Other *)> action_func_;
        std::function<int_fast32_t(Self *, Other *)> weight_func_;
        Self *s_param_;
        Other *o_param_;

    public:
        action(std::function<void(Self *, Other *)> &&action, Self *s_param, Other *o_param,
               std::function<int_fast32_t(Self *, Other *)> &&weight_func);

        // Calls the action function with the parameters
        virtual auto perform() const -> void override;

        // Returns the weight of the action (cached)
        [[nodiscard]] virtual auto weight() const -> int_fast32_t override;
    };

    template<typename Self, typename Other>
    action<Self, Other>::action(std::function<void(Self *, Other *)> &&action, Self *s_param, Other *o_param,
                                std::function<int_fast32_t(Self *, Other *)> &&weight_func): action_func_(
            std::move(action)),
        weight_func_(std::move(weight_func)),
        s_param_(s_param),
        o_param_(o_param) {}

    template<typename Self, typename Other>
    auto action<Self, Other>::perform() const -> void override { action_func_(s_param_, o_param_); }

    template<typename Self, typename Other>
    auto action<Self, Other>::weight() const -> int_fast32_t override { return weight_func_(s_param_, o_param_); }
}
