#pragma once
#include <string>
#include <vector>

namespace mechanics {
    class data;
}

namespace processing {
    class image;

    // Loads data from map image and files
    auto load_image(mechanics::data &d, image &map_image, std::vector<uint8_t> &crossing_bytes,
                    std::string &loading_text) -> void;
}
