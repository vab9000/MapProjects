#pragma once

#include <unordered_map>
#include "../features/province.hpp"
#include "../features/tag.hpp"
#include "../features/date.hpp"

class province;

struct data {
    std::unordered_map<unsigned int, province> provinces;
    std::unordered_map<unsigned long long, std::unique_ptr<tag> > tags;

    province *selected_province = nullptr;
    map_modes map_mode{map_modes::provinces};
    date date{};
};
