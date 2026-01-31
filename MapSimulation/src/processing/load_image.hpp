#pragma once
#include <string>
#include <vector>

namespace mechanics {
    class data;
}

namespace processing {
    class image;

    // Loads data from map image and files
    auto load_image(image &map_image, std::string &loading_text) -> void;
}
