#pragma once

#include <unordered_map>
#include "../model/base/province.hpp"
#include "../model/base/utils.hpp"
#include "../model/tags/tag.hpp"

struct data {
    std::unordered_map<unsigned int, std::unique_ptr<province> > provinces = {};
    std::unordered_map<unsigned int, std::unique_ptr<tag> > tags = {};

    province *selected_province = nullptr;
    map_modes map_mode = map_modes::owner;
    date date = {};
};
