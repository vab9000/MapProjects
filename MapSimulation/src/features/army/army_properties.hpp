#pragma once

enum class army_directive_type { attack, none };

struct army_directive {
    army_directive_type type = army_directive_type::none;
    void *target = nullptr;
};
