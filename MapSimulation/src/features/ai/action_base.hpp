#pragma once

#include <cstdint>

class action_base {
public:
    virtual ~action_base() = default;

    virtual void perform() const = 0;

    virtual int_fast32_t weight() = 0;
};