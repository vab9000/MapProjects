#pragma once

#include <unordered_map>
#include "../features/province.hpp"
#include "../features/tag.hpp"
#include "../features/date.hpp"
#include "../features/river.hpp"

class province;

struct data {
    std::unordered_map<uint_fast32_t, province> provinces;
    std::unordered_map<uint_fast32_t, std::unique_ptr<tag> > tags;
    std::vector<river> rivers;

    province *selected_province = nullptr;
    map_modes map_mode{map_modes::provinces};
    date date{};
};
