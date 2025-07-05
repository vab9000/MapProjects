#include "action.hpp"

template<typename Self, typename Other>
action<Self, Other>::action(std::function<void(Self *, Other *)> &&action, Self *s_param, Other *o_param,
                            std::function<int_fast32_t(Self *, Other *)> &&weight_func) : action_func_(
        std::move(action)),
    weight_func_(std::move(weight_func)),
    s_param_(s_param),
    o_param_(o_param), weight_cache_(weight()) {
}

template<typename Self, typename Other>
void action<Self, Other>::perform() const {
    action_func_(s_param_, o_param_);
}

template<typename Self, typename Other>
[[nodiscard]] int_fast32_t action<Self, Other>::weight() {
    constexpr int_fast8_t cache_amount = 30;
    if (num_cache_ != 0) {
        num_cache_--;
        return weight_cache_;
    }
    num_cache_ = cache_amount;
    return weight_func_(s_param_, o_param_);
}
