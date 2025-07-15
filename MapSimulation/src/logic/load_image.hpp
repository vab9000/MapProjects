#pragma once
#include <string>
#include <vector>

class image;
class data;

// Loads data from map image and files
void load_image(data &d, image &map_image, std::vector<uint8_t> &crossing_bytes, std::string &loading_text);
