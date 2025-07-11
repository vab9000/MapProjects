#pragma once

#include <cstdint>

class action_base {
public:
    virtual ~action_base() = default;

    virtual void perform() const = 0;

    [[nodiscard]] virtual int_fast32_t weight() const = 0;
};