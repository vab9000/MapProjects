#pragma once
#include <string_view>

namespace utils {
    // String view guaranteed to be null terminated
    class zstring_view : std::string_view {
    public:
        constexpr zstring_view() = default;

        constexpr zstring_view(const std::string &s) : std::string_view(s) {}

        constexpr zstring_view(const char *s) : std::string_view(s) {}

        [[nodiscard]] constexpr auto str() const -> const char * {
            return data();
        }
    };
}
