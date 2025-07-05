#pragma once

#include <functional>
#include "action_base.hpp"

template<typename Self, typename Other>
class action final : public action_base {
    std::function<void(Self *, Other *)> action_func_;
    std::function<int_fast32_t(Self *, Other *)> weight_func_;
    Self *s_param_;
    Other *o_param_;

    int_fast32_t weight_cache_;
    int_fast8_t num_cache_ = 0;

public:
    action(std::function<void(Self *, Other *)> &&action, Self *s_param, Other *o_param,
           std::function<int_fast32_t(Self *, Other *)> &&weight_func);

    // Calls the action function with the parameters
    void perform() const override;

    // Returns the weight of the action (cached)
    [[nodiscard]] int_fast32_t weight() override;
};
