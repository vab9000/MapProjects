#ifndef MAPEDITOR_ZSTRING_VIEW_HPP
#define MAPEDITOR_ZSTRING_VIEW_HPP
#include <cstddef>
#include <string>
#include <string_view>

/// String view guaranteed to be null terminated
class zstring_view {
    std::string_view data_;

public:
    constexpr zstring_view() = default;

    /// Construct from a string
    /// @param str The string
    constexpr explicit zstring_view(const std::string &str) : data_(str) {};

    /// Construct from a string pointer
    /// @param str The string pointer
    constexpr explicit zstring_view(const char *str) : data_(str) {};

    /// Get the data pointer
    /// @returns The data pointer
    [[nodiscard]] constexpr auto str() const -> const char * { return data_.data(); }

    [[nodiscard]] constexpr auto string() const -> std::string { return std::string(data_); }
};

auto constexpr operator ""_zsv(const char *str, size_t /*size*/) noexcept -> zstring_view { return zstring_view(str); }

#endif
