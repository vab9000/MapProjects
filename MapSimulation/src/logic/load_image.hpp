#pragma once
#include <string>

class image;
struct data;

// Loads data from map image and files
void load_image(data &data, image &map_image, std::string &loading_text);
