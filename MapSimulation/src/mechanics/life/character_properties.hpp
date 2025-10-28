#ifndef MECHANICS_CHARACTER_PROPERTIES
#define MECHANICS_CHARACTER_PROPERTIES
#include <type_traits>

namespace mechanics {
    class army;
    class unit;

    enum class personality_trait_t {
        ambitious,
    };

    enum class role_t {
        commander, captain,
    };

    struct role_type {
        template<role_t R>
        using type = std::conditional_t<R == role_t::commander, army,
            std::conditional_t<R == role_t::captain, unit, void>> *;
    };
}
#endif
