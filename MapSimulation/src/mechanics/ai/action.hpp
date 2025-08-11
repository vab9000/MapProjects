#pragma once
#include <functional>
#include "action_base.hpp"

namespace mechanics {
    template<typename Self, typename Other>
    class action final : public action_base {
        std::function<bool(Self &, Other &)> valid_func_;
        std::function<void(Self &, Other &)> action_func_;
        std::function<int_fast32_t(Self &, Other &)> weight_func_;
        Self &s_param_;
        Other &o_param_;

    public:
        action(std::function<bool(Self &, Other &)> &&valid, std::function<void(Self &, Other &)> &&action,
            std::function<int_fast32_t(Self &, Other &)> &&weight_func,
            Self &s_param, Other &o_param) : valid_func_(std::move(valid)), action_func_(std::move(action)),
            weight_func_(std::move(weight_func)),
            s_param_(s_param),
            o_param_(o_param) {}

        auto operator()() const -> void override { action_func_(s_param_, o_param_); }

        [[nodiscard]] auto operator*() const -> int_fast32_t override { return weight_func_(s_param_, o_param_); }

        explicit operator bool() const override { return valid_func_(s_param_, o_param_); }
    };
}
