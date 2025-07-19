#pragma once

namespace mechanics {
    enum class army_directive_t { attack, none };

    struct army_directive {
        army_directive_t type = army_directive_t::none;
        void *target = nullptr;
    };
}
