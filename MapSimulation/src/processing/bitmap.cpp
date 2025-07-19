#include "bitmap.hpp"
#include <algorithm>
#include <execution>
#include <ranges>
#include "../mechanics/data.hpp"
#include "../mechanics/province.hpp"
#include "drawing.hpp"

namespace {
    auto set_pixel(std::vector<uint8_t> &bytes, const size_t index, const uint8_t r,
                   const uint8_t g, const uint8_t b) -> void {
        bytes[index] = r;
        bytes[index + 1] = g;
        bytes[index + 2] = b;
        bytes[index + 3] = 255;
    }
}

namespace processing {
    bitmap::bitmap() = default;

    bitmap::bitmap(image *image, const mechanics::data &data, drawing &drawer) : map_image_(image),
        bytes_(
            map_image_->width() * map_image_->height() *
            4,
            0) { reload_bitmap(data, drawer); }

    auto bitmap::reload_bitmap_province(const mechanics::province &reload_province, const mechanics::data &data,
                                        drawing &drawer) -> void {
        const auto x_indices = std::views::iota(reload_province.bounds()[0],
                                                reload_province.bounds()[2] + 1);

        const auto y_indices = std::views::iota(reload_province.bounds()[1],
                                                reload_province.bounds()[3] + 1);

        std::for_each(std::execution::par_unseq, x_indices.begin(), x_indices.end(), [&](const uint_fast32_t i) {
            std::for_each(std::execution::par_unseq, y_indices.begin(), y_indices.end(),
                          [&](const uint_fast32_t j) {
                              const auto index = (i + j * map_image_->width()) * 4;
                              const auto color = data.provinces().at(map_image_->color(i, j)).color();
                              set_pixel(bytes_, index, static_cast<uint8_t>(color),
                                        static_cast<uint8_t>(color >> 8),
                                        static_cast<uint8_t>(color >> 16));
                          });
        });

        drawer.update_map_texture(bytes_.data() + map_image_->width() * reload_province.bounds()[1] * 4,
                                  sf::Vector2u(map_image_->width(),
                                               reload_province.bounds()[3] - reload_province.bounds()[1] + 1),
                                  sf::Vector2u(0, reload_province.bounds()[1]));
    }

    auto bitmap::reload_bitmap(const mechanics::data &data, drawing &drawer) -> void {
        const auto x_indices = std::views::iota(0u, map_image_->width());
        const auto y_indices = std::views::iota(0u, map_image_->height());

        std::for_each(std::execution::par_unseq, x_indices.begin(), x_indices.end(), [&](const uint_fast32_t i) {
            std::for_each(std::execution::par_unseq, y_indices.begin(), y_indices.end(),
                          [&](const uint_fast32_t j) {
                              const auto index = (i + j * map_image_->width()) * 4;
                              const auto color = data.provinces().at(map_image_->color(i, j)).color();
                              set_pixel(bytes_, index, static_cast<uint8_t>(color),
                                        static_cast<uint8_t>(color >> 8),
                                        static_cast<uint8_t>(color >> 16));
                          });
        });

        drawer.update_map_texture(bytes_.data());
    }

    auto bitmap::bytes() const -> const std::vector<uint8_t> & { return bytes_; }
}
