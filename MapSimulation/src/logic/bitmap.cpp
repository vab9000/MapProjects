#include "bitmap.hpp"
#include <numeric>
#include <algorithm>
#ifdef __cpp_lib_execution
#include <execution>
#endif
#ifndef __cpp_lib_execution
#include <ranges>
#endif
#include "../features/province.hpp"
#include "data.hpp"
#include "../ui/drawing.hpp"

inline void set_pixel(std::vector<uint8_t> &bytes, const size_t index, const uint8_t r,
                      const uint8_t g, const uint8_t b) {
    bytes[index] = r;
    bytes[index + 1] = g;
    bytes[index + 2] = b;
    bytes[index + 3] = 255;
}

void bitmap::init_bitmap(image *image, const data &data, drawing &drawer) {
    map_image_ = image;

    bytes_ = std::vector<uint8_t>(map_image_->width() * map_image_->height() * 4, 0);

    reload_bitmap(data, drawer);
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void bitmap::reload_bitmap_province(const province &reload_province, const data &data, drawing &drawer) {
    std::vector<uint_fast32_t> x_indices(reload_province.bounds()[2] - reload_province.bounds()[0] + 1);
    std::iota(x_indices.begin(), x_indices.end(), reload_province.bounds()[0]);

    std::vector<uint_fast32_t> y_indices(reload_province.bounds()[3] - reload_province.bounds()[1] + 1);
    std::iota(y_indices.begin(), y_indices.end(), reload_province.bounds()[1]);

#ifdef __cpp_lib_execution
    std::for_each(std::execution::par_unseq, x_indices.begin(), x_indices.end(), [&](const uint_fast32_t i) {
        std::for_each(std::execution::par_unseq, y_indices.begin(), y_indices.end(), [&](const uint_fast32_t j) {
            const auto index = (i + j * map_image_->width()) * 4;
            const auto color = data.provinces.at(map_image_->color(i, j)).color();
            set_pixel(bytes_, index, static_cast<uint8_t>(color),
                      static_cast<uint8_t>(color >> 8), static_cast<uint8_t>(color >> 16));
        });
    });
#else
    std::ranges::for_each(x_indices, [&](const uint_fast32_t i) {
        std::ranges::for_each(y_indices [&](const uint_fast32_t j) {
            const auto index = (i + j * map_image_.width()) * 4;
            set_pixel(bytes_, index, static_cast<uint8_t>(color), static_cast<uint8_t>(color >> 8),
                      static_cast<uint8_t>(color >> 16));
        });
    });
#endif

    drawer.update_map_texture(bytes_.data() + map_image_->width() * reload_province.bounds()[1] * 4,
                              sf::Vector2u(map_image_->width(),
                                           reload_province.bounds()[3] - reload_province.bounds()[1] + 1),
                              sf::Vector2u(0, reload_province.bounds()[1]));
}

void bitmap::reload_bitmap(const data &data, drawing &drawer) {
    std::vector<uint_fast32_t> x_indices(map_image_->width());
    std::iota(x_indices.begin(), x_indices.end(), 0);

    std::vector<uint_fast32_t> y_indices(map_image_->height());
    std::iota(y_indices.begin(), y_indices.end(), 0);

#ifdef __cpp_lib_execution
    std::for_each(
        std::execution::par_unseq, x_indices.begin(), x_indices.end(),
        [&](const uint_fast32_t i) {
            std::for_each(std::execution::par_unseq, y_indices.begin(), y_indices.end(), [&](const uint_fast32_t j) {
                const auto index = (i + j * map_image_->width()) * 4;
                const auto color = data.provinces.at(map_image_->color(i, j)).color();
                set_pixel(bytes_, index, static_cast<uint8_t>(color),
                          static_cast<uint8_t>(color >> 8), static_cast<uint8_t>(color >> 16));
            });
        });
#else
    std::ranges::for_each(province_values
                          ,
                          [&](const province &province) {
                              const auto &pixels = province.pixels();

                              std::vector<uint_fast32_t> indices(province.num_pixels());
                              std::iota(indices.begin(), indices.end(), 0);

                              std::ranges::for_each(indices, [&](const uint_fast32_t i) {
                                  const auto pixel = pixels[i];
                                  const auto color = province.color;
                                  const auto index = (pixel[0] + pixel[1] * map_image_.width()) * 4;
                                  set_pixel(bytes_, index, static_cast<uint8_t>(color),
                                            static_cast<uint8_t>(color >> 8),
                                            static_cast<uint8_t>(color >> 16));
                              });

                              const auto &outline = province.outline();

                              indices = std::vector<int>(province.num_outline());
                              std::iota(indices.begin(), indices.end(), 0);

                              std::ranges::for_each(indices, [&](const uint_fast32_t i) {
                                  const auto other_province = outline[i].first;
                                  const auto pixel = outline[i].second;
                                  const auto color = province.color;
                                  const auto index = (pixel[0] + pixel[1] * map_image_.width()) * 4;
                                  if (data_.selected_province != nullptr && color == data_.selected_province->color) {
                                      set_pixel(bytes_, index, 255, 255, 255);
                                  } else {
                                      if (other_province != nullptr && other_province->color != color) {
                                          set_pixel(bytes_, index, 0, 0, 0);
                                      } else {
                                          set_pixel(bytes_, index, static_cast<uint8_t>(color),
                                                    static_cast<uint8_t>(color >> 8),
                                                    static_cast<uint8_t>(color >> 16));
                                      }
                                  }
                              });
                          });
#endif

    drawer.update_map_texture(bytes_.data());
}

const std::vector<uint8_t> &bitmap::bytes() const {
    return bytes_;
}
