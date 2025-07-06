#pragma once
#include <vector>
#include "image.hpp"

class province;
class data;
class drawing;

class bitmap {
    image *map_image_ = nullptr;
    std::vector<uint8_t> bytes_{};

public:
    bitmap();

    bitmap(image *image, const data &data, drawing &drawer);

    // Reloads the bitmap for the region of a province
    void reload_bitmap_province(const province &reload_province, const data &data, drawing &drawer);

    // Reloads the entire bitmap
    void reload_bitmap(const data &data, drawing &drawer);

    // Gets the byte array of the bitmap
    [[nodiscard]] const std::vector<uint8_t> &bytes() const;
};
