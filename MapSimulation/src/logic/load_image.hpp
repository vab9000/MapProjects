#pragma once
#include <string>

class image;
struct data;

void load_image(data &data, image &map_image, double &progress, const std::string &province_filepath);
