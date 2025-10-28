#ifndef UTILS_ENUMS
#define UTILS_ENUMS
#include <magic_enum/magic_enum.hpp>

namespace utils {
    /// A concept that checks if a type is an enum class.
    /// @tparam E The type to check.
    template<typename E>
    concept is_enum_class = magic_enum::is_scoped_enum_v<E>;

    /// A function that calculates the number of bits required to represent a given number.
    /// @param x The number to calculate the number of bits for.
    /// @returns The number of bits required to represent the number.
    consteval auto number_of_bits(size_t x) -> size_t {
        if (x == 0) {
            return 1;
        }
        size_t result = 0;
        while (x != 0) {
            result++;
            x = x >> 1;
        }
        return result;
    }

    /// The minimum number of bits required to store all values of the enum class.
    /// @tparam E The enum class type.
    template<is_enum_class E>
    constexpr size_t enum_bits_required_v = number_of_bits(magic_enum::enum_count<E>());
}
#endif
