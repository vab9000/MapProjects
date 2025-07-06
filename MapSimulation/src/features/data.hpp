#pragma once

#include <unordered_map>
#include "province.hpp"
#include "tag.hpp"
#include "date.hpp"
#include "river.hpp"
#include "character.hpp"

class data {
    date current_date_{};
    std::unordered_map<uint_fast32_t, province> provinces_;
    std::unordered_map<uint_fast32_t, tag> tags_;
    std::unordered_map<uint_fast32_t, character> characters_;
    std::vector<std::unique_ptr<river> > rivers_;

    static data instance_;

    data() = default;

public:
    data &operator=(const data &) = delete;

    data(const data &) = delete;

    // Get the current date
    [[nodiscard]] const date &current_date() const;

    // Get the provinces
    [[nodiscard]] const std::unordered_map<uint_fast32_t, province> &provinces() const;

    // Get the provinces
    std::unordered_map<uint_fast32_t, province> &provinces();

    // Add a province
    void add_province(province &&p);

    // Get the tags
    [[nodiscard]] const std::unordered_map<uint_fast32_t, tag> &tags() const;

    // Get the tags
    std::unordered_map<uint_fast32_t, tag> &tags();

    // Get the characters
    [[nodiscard]] const std::unordered_map<uint_fast32_t, character> &characters() const;

    // Get the characters
    std::unordered_map<uint_fast32_t, character> &characters();

    // Get the rivers
    [[nodiscard]] const std::vector<std::unique_ptr<river> > &rivers() const;

    // Add a river
    river &add_river();

    // Get the singleton instance of data
    static data &instance();
};
