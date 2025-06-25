#include "bitmap.hpp"
#include <numeric>
#include <algorithm>
#include <unordered_set>
#ifdef __cpp_lib_execution
#include <execution>
#endif
#include <ranges>
#include "../features/base/province.hpp"
#include "data.hpp"
#include "../ui/drawing.hpp"

inline void set_pixel(std::vector<unsigned char> &bytes, const int index, const unsigned char r,
                      const unsigned char g, const unsigned char b) {
    bytes[index] = r;
    bytes[index + 1] = g;
    bytes[index + 2] = b;
    bytes[index + 3] = 255;
}

void bitmap::init_bitmap(image *image, const data &data, drawing &drawer) {
    map_image_ = image;

    bytes_ = std::vector<unsigned char>(map_image_->width() * map_image_->height() * 4, 0);

    reload_bitmap(data, drawer);
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void bitmap::reload_bitmap_province(const province &reload_province, const data &data, drawing &drawer) {
    const auto color = reload_province.color;
    const auto &pixels = reload_province.pixels();

    std::vector<int> indices(reload_province.num_pixels());
    std::iota(indices.begin(), indices.end(), 0);

#ifdef __cpp_lib_execution
    std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](const int i) {
        const auto pixel = pixels[i];
        const auto index = (pixel[0] + pixel[1] * map_image_->width()) * 4;
        set_pixel(bytes_, index, static_cast<unsigned char>(color), static_cast<unsigned char>(color >> 8),
                  static_cast<unsigned char>(color >> 16));
    });
#else
    std::ranges::for_each(indices, [&](const int i) {
        const auto pixel = pixels[i];
        const auto index = (pixel[0] + pixel[1] * map_image_.width()) * 4;
        set_pixel(bytes_, index, static_cast<unsigned char>(color), static_cast<unsigned char>(color >> 8),
                  static_cast<unsigned char>(color >> 16));
    });
#endif

    const auto &outline = reload_province.outline();
    auto updated_provinces = std::unordered_set<province *>();
    for (auto i = 0; i < reload_province.num_outline(); ++i) {
        const auto pixel = outline[i].second;
        const auto other_province = outline[i].first;
        const auto index = (pixel[0] + pixel[1] * map_image_->width()) * 4;

        if (data.selected_province != nullptr && reload_province.base_color() == data.selected_province->
            base_color()) {
            set_pixel(bytes_, index, 255, 255, 255);
        } else {
            if (other_province != nullptr && other_province->color != color) {
                set_pixel(bytes_, index, 0, 0, 0);
            } else {
                set_pixel(bytes_, index, static_cast<unsigned char>(color), static_cast<unsigned char>(color >> 8),
                          static_cast<unsigned char>(color >> 16));
            }
        }

        if (!updated_provinces.contains(other_province)) {
            updated_provinces.insert(other_province);

            if (other_province == nullptr) {
                throw std::invalid_argument("Province is null");
            }

            const auto other_outline = other_province->outline();

            for (unsigned int j = 0; j < other_province->num_outline(); ++j) {
                const auto other_pixel = other_outline[j].second;
                const auto other_index = (other_pixel[0] + other_pixel[1] * map_image_->width()) * 4;

                if (other_outline[j].first == &const_cast<province &>(reload_province)) {
                    if (other_province->color == color) {
                        set_pixel(bytes_, other_index, static_cast<unsigned char>(color),
                                  static_cast<unsigned char>(color >> 8),
                                  static_cast<unsigned char>(color >> 16));
                    } else {
                        set_pixel(bytes_, other_index, 0, 0, 0);
                    }
                }
            }
        }
    }

    drawer.update_map_texture(bytes_.data() + map_image_->width() * reload_province.bounds()[1] * 4,
                               sf::Vector2u(map_image_->width(),
                                            reload_province.bounds()[3] - reload_province.bounds()[1] + 1),
                               sf::Vector2u(0, reload_province.bounds()[1]));
}

void bitmap::reload_bitmap(const data &data, drawing &drawer) {
    const auto province_values = data.provinces | std::views::values;

#ifdef __cpp_lib_execution
    std::for_each(
        std::execution::par_unseq, province_values.begin(), province_values.end(),
        [&](const province &province) {
            const auto &pixels = province.pixels();

            std::vector<int> indices(province.num_pixels());
            std::iota(indices.begin(), indices.end(), 0);

            std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](const int i) {
                const auto pixel = pixels[i];
                const auto color = province.color;
                const auto index = (pixel[0] + pixel[1] * map_image_->width()) * 4;
                set_pixel(bytes_, index, static_cast<unsigned char>(color), static_cast<unsigned char>(color >> 8),
                          static_cast<unsigned char>(color >> 16));
            });

            const auto &outline = province.outline();

            indices = std::vector<int>(province.num_outline());
            std::iota(indices.begin(), indices.end(), 0);

            std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](const int i) {
                const auto other_province = outline[i].first;
                const auto pixel = outline[i].second;
                const auto color = province.color;
                const auto index = (pixel[0] + pixel[1] * map_image_->width()) * 4;
                if (data.selected_province != nullptr && province.base_color() == data.selected_province->base_color()) {
                    set_pixel(bytes_, index, 255, 255, 255);
                } else {
                    if (other_province != nullptr && other_province->color != color) {
                        set_pixel(bytes_, index, 0, 0, 0);
                    } else {
                        set_pixel(bytes_, index, static_cast<unsigned char>(color),
                                  static_cast<unsigned char>(color >> 8),
                                  static_cast<unsigned char>(color >> 16));
                    }
                }
            });
        });
#else
    std::ranges::for_each(province_values
                          ,
                          [&](const province &province) {
                              const auto &pixels = province.pixels();

                              std::vector<int> indices(province.num_pixels());
                              std::iota(indices.begin(), indices.end(), 0);

                              std::ranges::for_each(indices, [&](const int i) {
                                  const auto pixel = pixels[i];
                                  const auto color = province.color;
                                  const auto index = (pixel[0] + pixel[1] * map_image_.width()) * 4;
                                  set_pixel(bytes_, index, static_cast<unsigned char>(color),
                                            static_cast<unsigned char>(color >> 8),
                                            static_cast<unsigned char>(color >> 16));
                              });

                              const auto &outline = province.outline();

                              indices = std::vector<int>(province.num_outline());
                              std::iota(indices.begin(), indices.end(), 0);

                              std::ranges::for_each(indices, [&](const int i) {
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
                                          set_pixel(bytes_, index, static_cast<unsigned char>(color),
                                                    static_cast<unsigned char>(color >> 8),
                                                    static_cast<unsigned char>(color >> 16));
                                      }
                                  }
                              });
                          });
#endif

    drawer.update_map_texture(bytes_.data());
}

const std::vector<unsigned char> &bitmap::bytes() const {
    return bytes_;
}
