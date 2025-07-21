#pragma once
#include <set>

namespace utils {
    template<typename E>
    class flags : std::set<E> {
        static_assert(std::is_enum_v<E> && !std::is_convertible_v<E, int>, "Flags only accepts enum classes!");

    public:
        flags() = default;

        auto add(const E flag) -> void { std::set<E>::insert(flag); }

        [[nodiscard]] auto has(const E flag) const -> bool { return std::set<E>::contains(flag); }

        auto remove(const E flag) -> void { std::set<E>::erase(flag); }

        [[nodiscard]] auto all() const -> const std::set<E> & { return *this; }
    };
}
