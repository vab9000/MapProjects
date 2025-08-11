#pragma once
#include <set>

namespace utils {
    template<typename E>
    class flags {
        static_assert(std::is_enum_v<E> && !std::is_convertible_v<E, int>, "Flags only accepts enum classes!");

        std::set<E> flags_;

    public:
        flags() = default;

        // Add a new flag
        auto add(const E flag) -> void { flags_.insert(flag); }

        // Check if a flag is present
        [[nodiscard]] auto has(const E flag) const -> bool { return flags_.contains(flag); }

        // Remove a flag
        auto remove(const E flag) -> void { flags_.erase(flag); }

        // Get all flags
        [[nodiscard]] auto all() const -> const std::set<E> & { return *this; }
    };
}
