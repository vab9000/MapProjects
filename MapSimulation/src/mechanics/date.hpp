#pragma once
#include <string>

namespace mechanics {
    struct date {
        uint_fast32_t year;
        uint_fast8_t month;
        uint_fast8_t day;

        date();

        date(uint_fast32_t year, uint_fast8_t month,
             uint_fast8_t day);

        explicit date(const date *other);

        // Returns the absolute number of days since the tracking date (1/1/0)
        [[nodiscard]] auto absolute_days() const -> int_fast64_t;

        // Gets a string representation of the date in the format "day/month/year"
        [[nodiscard]] auto to_string() const -> std::string;

        // Gets a string representation of the date in the format "day/month/year AE/BE" with the epoch year being the year 0
        [[nodiscard]] auto to_string(uint_fast32_t epoch_year) const -> std::string;

        auto operator==(const date &other) const -> bool;

        auto operator<(const date &other) const -> bool;

        auto operator+(int_fast64_t days) const -> date;

        auto operator-(int_fast64_t days) const -> date;

        auto operator-(const date &other) const -> int_fast64_t;
    };
}
