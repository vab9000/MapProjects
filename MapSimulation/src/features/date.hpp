#pragma once
#include <string>

struct date {
    uint_fast32_t year;
    uint_fast8_t month;
    uint_fast8_t day;

    date();

    date(uint_fast32_t year, uint_fast8_t month,
         uint_fast8_t day);

    explicit date(const date *other);

    // Returns the absolute number of days since the tracking date (1/1/0)
    [[nodiscard]] int_fast64_t absolute_days() const;

    // Gets a string representation of the date in the format "day/month/year"
    [[nodiscard]] std::string to_string() const;

    // Gets a string representation of the date in the format "day/month/year AE/BE" with the epoch year being the year 0
    [[nodiscard]] std::string to_string(uint_fast32_t epoch_year) const;

    bool operator==(const date &other) const;

    bool operator<(const date &other) const;

    date operator+(int_fast64_t days) const;

    date operator-(int_fast64_t days) const;

    int_fast64_t operator-(const date &other) const;
};
