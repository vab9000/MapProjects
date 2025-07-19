#pragma once
#include <vector>
#include "image.hpp"

namespace mechanics {
    class province;
    class data;
}

namespace processing {
    class drawing;

    class bitmap {
        image *map_image_ = nullptr;
        std::vector<uint8_t> bytes_;

    public:
        bitmap();

        bitmap(image *image, const mechanics::data &data, drawing &drawer);

        // Reloads the bitmap for the region of a province
        auto reload_bitmap_province(const mechanics::province &reload_province, const mechanics::data &data,
                                    drawing &drawer) -> void;

        // Reloads the entire bitmap
        auto reload_bitmap(const mechanics::data &data, drawing &drawer) -> void;

        // Gets the byte array of the bitmap
        [[nodiscard]] auto bytes() const -> const std::vector<uint8_t> &;
    };
}
