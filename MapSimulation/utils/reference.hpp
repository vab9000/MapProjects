#pragma once
#include <functional>
#include <memory>
#include <type_traits>

namespace utils {
    /// A reference wrapper, essentially std::reference_wrapper but with added operators.
    /// Should be treated as a pointer that cannot be null, but does not necessarily point to a valid object.
    /// @tparam T The type to wrap a reference to.
    template<typename T>
    using ref = std::reference_wrapper<T>;

    /// A concept that checks if two types are the same, ignoring constness.
    /// @tparam T The first type.
    /// @tparam U The second type.
    template<typename T, typename U>
    concept same_sans_const = requires(T, U) { std::is_same_v<std::remove_const<T>, std::remove_const<U>>; };

    /// A hash function for ref<T> that hashes the address of the referenced object.
    /// @tparam T The type of the referenced object.
    template<typename T>
    struct ref_hash {
        auto operator()(const ref<T> &value) const noexcept -> size_t {
            return std::hash<const void *>()(&value.get());
        }
    };
}

template<typename T, typename U> requires utils::same_sans_const<T, U>
auto operator<=>(utils::ref<T> lhs, utils::ref<U> rhs) -> std::strong_ordering { return &lhs.get() <=> &rhs.get(); }

template<typename T, typename U> requires utils::same_sans_const<T, U>
auto operator==(utils::ref<T> lhs, utils::ref<U> rhs) -> bool { return &lhs.get() == &rhs.get(); }
