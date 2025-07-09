#pragma once
#include <string>

class image;
class data;

// Loads data from map image and files
void load_image(data &d, image &map_image, std::string &loading_text);
