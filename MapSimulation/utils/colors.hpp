#ifndef UTILS_COLORS
#define UTILS_COLORS

namespace utils {
    /// Gets the integer representation of an RGB color.
    /// The first 8 bits are set to 0.
    /// @param r The red component (0-255).
    /// @param g The green component (0-255).
    /// @param b The blue component (0-255).
    /// @return The integer representation of the color.
    constexpr auto
    to_integer_color(const unsigned char r, const unsigned char g, const unsigned char b) -> unsigned int {
        return static_cast<unsigned int>(r) << 16 | static_cast<unsigned int>(g) << 8 | static_cast<unsigned int>(b);
    }
}
#endif
