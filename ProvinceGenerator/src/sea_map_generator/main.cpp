#include <array>
#include "../common/image.hpp"

auto is_ocean_pixel(const image &img, const int x, const int y) -> bool {
    const auto pixel = img(x, y);
    return pixel.r() == 0U && pixel.g() == 255U && pixel.b() == 255U;
}

auto is_lake_pixel(const image &img, const int x, const int y) -> bool {
    const auto pixel = img(x, y);
    return pixel.r() == 255U && pixel.g() == 0U && pixel.b() == 255U;
}

auto is_coast_pixel(const image &img, const int x, const int y) -> bool {
    if (!is_ocean_pixel(img, x, y)) { return false; }

    constexpr std::array<std::pair<int, int>, 4> directions = {
        {
            {0, -1},
            {-1, 0},
            {1, 0},
            {0, 1},
        }
    };
    for (const auto &[dx, dy] : directions) {
        const int nx = x + dx;
        const int ny = y + dy;
        if (nx >= 0 && nx < img.width() && ny >= 0 && ny < img.height()) {
            if (!is_ocean_pixel(img, nx, ny)) { return true; }
        }
    }
    return false;
}

auto fill_circle(const image &img, const int x, const int y, const int radius, const std::array<unsigned char, 3> color) -> void {
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (dx * dx + dy * dy <= radius * radius) {
                const int nx = x + dx;
                if (const int ny = y + dy; nx >= 0 && nx < img.width() && ny >= 0 && ny < img.height()) {
                    const auto pixel = img(nx, ny);
                    pixel.r() = color[0];
                    pixel.g() = color[1];
                    pixel.b() = color[2];
                }
            }
        }
    }
}

auto generate_coastal_areas(const image &base_map, const image &sea_map) -> void {
    for (auto y = 0; y < base_map.height(); ++y) {
        for (auto x = 0; x < base_map.width(); ++x) {
            if (!is_coast_pixel(base_map, x, y)) {
                continue;
            }
            fill_circle(sea_map, x, y, 30, {0, 0, 255});
        }
    }
    for (auto y = 0; y < base_map.height(); ++y) {
        for (auto x = 0; x < base_map.width(); ++x) {
            if (!is_ocean_pixel(base_map, x, y)) {
                auto pixel = sea_map(x, y);
                pixel.r() = 0;
                pixel.g() = 0;
                pixel.b() = 0;
            }
        }
    }
}

auto zero_image(const image &img) -> void {
    for (auto y = 0; y < img.height(); ++y) {
        for (auto x = 0; x < img.width(); ++x) {
            const auto pixel = img(x, y);
            pixel.r() = 0;
            pixel.g() = 0;
            pixel.b() = 0;
        }
    }
}

auto main() -> int {
    const image base_map{"images/map.png"};
    const image sea_map{"images/sea_map.png", base_map.width(), base_map.height()};

    zero_image(sea_map);
    generate_coastal_areas(base_map, sea_map);
    sea_map.write();

    return 0;
}
