#ifndef MECHANICS_ACTION
#define MECHANICS_ACTION
#include <functional>
#include "action_base.hpp"

namespace mechanics {
    template<typename Self, typename Other>
    class action final : public action_base {
        const std::function<bool(Self &, Other &)> valid_func_;
        const std::function<void(Self &, Other &)> action_func_;
        const std::function<int(Self &, Other &)> weight_func_;
        const Self &s_param_;
        const Other &o_param_;

    public:
        action(std::function<bool(Self &, Other &)> &&valid, std::function<void(Self &, Other &)> &&action,
            std::function<int(Self &, Other &)> &&weight_func,
            Self &s_param, Other &o_param) : valid_func_(std::move(valid)), action_func_(std::move(action)),
            weight_func_(std::move(weight_func)),
            s_param_(s_param),
            o_param_(o_param) {}

        auto operator()() const -> void override { action_func_(s_param_, o_param_); }

        [[nodiscard]] auto operator*() const -> int override { return weight_func_(s_param_, o_param_); }

        explicit operator bool() const override { return valid_func_(s_param_, o_param_); }
    };
}
#endif
