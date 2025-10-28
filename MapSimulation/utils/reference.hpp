#ifndef UTILS_REFERENCE
#define UTILS_REFERENCE
#include <functional>
#include <memory>
#include <type_traits>

namespace utils {
    /// A concept that checks if two types are the same, ignoring constness.
    /// @tparam T The first type.
    /// @tparam U The second type.
    template<typename T, typename U>
    concept same_sans_const = requires(T, U) { std::is_same_v<std::remove_const<T>, std::remove_const<U>>; };

    /// A reference wrapper, essentially std::reference_wrapper but with added operators, and not meant to hold functions.
    /// Should be treated as a pointer that cannot be null, but does not necessarily point to a valid object.
    /// @tparam T The type to wrap a reference to.
    template<typename T>
    class ref {
        T *ptr_;

    public:
        /// Construct a ref from a reference.
        /// @param r The reference to wrap.
        constexpr ref(T &r) noexcept : ptr_(&r) {}

        constexpr ref(const ref &other) noexcept = default;

        constexpr auto operator=(const ref &other) noexcept -> ref & = default;

        constexpr operator T &() const noexcept { return *ptr_; }

        /// Get the reference.
        /// @return Get the referenced object.
        constexpr auto get() const noexcept -> T & { return *ptr_; }

        template<typename U> requires std::is_convertible_v<T &, U &>
        constexpr operator ref<U>() const noexcept { return ref<U>(get()); }

        template<typename U> requires same_sans_const<T, U>
        constexpr auto operator<=>(const ref<U> &other) const noexcept -> std::strong_ordering {
            return ptr_ <=> &other.get();
        }

        template<typename U> requires same_sans_const<T, U>
        constexpr auto operator==(const ref<U> &other) const noexcept -> bool { return ptr_ == &other.get(); }

        /// A hash struct for ref.
        struct hash {
            constexpr auto operator()(const ref &r) const noexcept -> std::size_t {
                return std::hash<T *>()(&r.get());
            }
        };
    };
}
#endif
