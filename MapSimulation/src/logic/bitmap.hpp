#pragma once
#include <vector>
#include "image.hpp"

class province;
struct data;
class drawing;

class bitmap {
    std::vector<uint8_t> bytes_{};

    image *map_image_ = nullptr;

public:
    void init_bitmap(image *image, const data &data, drawing &drawer);

    void reload_bitmap_province(const province &reload_province, const data &data, drawing &drawer);

    void reload_bitmap(const data &data, drawing &drawer);

    [[nodiscard]] const std::vector<uint8_t> &bytes() const;
};
