#pragma once
#include <functional>
#include "date.hpp"

namespace mechanics {
    class event {
        date date_;
        std::function<void()> event_func_;

    public:
        explicit event(std::function<void()> &&func);

        auto operator<=>(const event &other) const -> std::strong_ordering;

        friend auto operator<=>(const std::unique_ptr<event> &self,
            const std::unique_ptr<event> &other) -> std::strong_ordering { return *self <=> *other; }

        [[nodiscard]] auto date() const -> const date &;

        auto operator()() const -> void;
    };
}
