#include "event.hpp"

namespace mechanics {
    event::event(std::function<void()> &&func) : event_func_(std::move(func)) {}

    auto event::operator<=>(const event &other) const -> std::strong_ordering { return date_ <=> other.date_; }

    auto event::date() const -> const mechanics::date & { return date_; }

    auto event::operator()() const -> void { event_func_(); }
}
