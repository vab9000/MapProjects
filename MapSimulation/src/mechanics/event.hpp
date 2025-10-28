#ifndef MECHANICS_EVENT
#define MECHANICS_EVENT
#include <functional>
#include "date.hpp"

namespace mechanics {
    class event {
        date date_;
        std::function<void()> event_func_;

    public:
        explicit event(std::function<void()> &&func);

        event(event &&other) noexcept = default;

        auto operator=(event &&other) noexcept -> event & = default;

        auto operator<=>(const event &other) const -> std::strong_ordering;

        [[nodiscard]] auto date() const -> const date &;

        auto operator()() const -> void;
    };
}

auto operator<=>(const std::unique_ptr<mechanics::event> &lhs,
    const std::unique_ptr<mechanics::event> &rhs) -> std::strong_ordering;
#endif
