#pragma once
#include <string_view>

namespace utils {
    /// String view guaranteed to be null terminated
    class zstring_view : std::string_view {
    public:
        constexpr zstring_view() = default;

        /// Construct from a string
        /// @param s The string
        constexpr zstring_view(const std::string &s) : std::string_view(s) {}

        /// Construct from a string pointer
        /// @param s The string pointer
        constexpr zstring_view(const char *s) : std::string_view(s) {}

        /// Get the data pointer
        /// @returns The data pointer
        [[nodiscard]] constexpr auto str() const -> const char * {
            return data();
        }
    };
}
