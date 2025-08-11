#pragma once

enum class tick_t {
    day,
    month,
    year,
};

class tickable {
public:
    virtual ~tickable() = default;

    virtual auto tick(tick_t tick_type) -> void = 0;
};
