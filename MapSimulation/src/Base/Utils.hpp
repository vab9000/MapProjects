#pragma once

#include <string>

enum class map_mode {
    provinces,
    owner,
};

struct date {
    enum class month_t {
        january = 1,
        february = 2,
        march = 3,
        april = 4,
        may = 5,
        june = 6,
        july = 7,
        august = 8,
        september = 9,
        october = 10,
        november = 11,
        december = 12
    };

    static unsigned int days_in_month(const month_t month, const unsigned int year) {
        switch (month) {
            case month_t::january:
            case month_t::march:
            case month_t::may:
            case month_t::july:
            case month_t::august:
            case month_t::october:
            case month_t::december:
                return 31;
            case month_t::april:
            case month_t::june:
            case month_t::september:
            case month_t::november:
                return 30;
            case month_t::february:
                if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
                    return 29;
                }
                return 28;
        }
        return 0;
    }

    unsigned int year;
    unsigned int month;
    unsigned int day;

    date() : date(0, 1, 1) {
    }

    date(const unsigned int year, const unsigned int month,
         const unsigned int day) : year(year), month(month), day(day) {
    }

    explicit date(const date *other) : date(other->year, other->month, other->day) {
    }

    [[nodiscard]] date clone() const { return date(this); }

    [[nodiscard]] unsigned int absolute_days() const {
        unsigned int total_days = 0;

        for (unsigned int y = 0; y < year; ++y) {
            total_days += (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? 366 : 365;
        }

        for (unsigned int m = 1; m < month; ++m) {
            total_days += days_in_month(static_cast<month_t>(m), year);
        }

        total_days += day;

        return total_days;
    }

    [[nodiscard]] std::string to_string() const {
        return std::to_string(day) + '/' + std::to_string(month) + '/' + std::to_string(year);
    }

    bool operator==(const date &other) const { return year == other.year && month == other.month && day == other.day; }

    date operator+(const int days) const {
        unsigned int new_day = day;
        unsigned int new_month = month;
        unsigned int new_year = year;
        if (days < 0) {
            new_day -= days;
            while (new_day < 1) {
                new_month -= 1;
                if (new_month < 1) {
                    new_year -= 1;
                    new_month = 12;
                }
                new_day += days_in_month(static_cast<month_t>(new_month), new_year);
            }
        } else if (days > 0) {
            new_day += days;
            while (new_day > days_in_month(static_cast<month_t>(new_month), new_year)) {
                new_day -= days_in_month(static_cast<month_t>(new_month), new_year);
                new_month += 1;
                if (new_month > 12) {
                    new_year += 1;
                    new_month = 1;
                }
            }
        }
        return {new_year, new_month, new_day};
    }

    date operator-(const int days) const { return *this + -days; }

    int operator-(const date &other) const {
        return static_cast<int>(absolute_days()) - static_cast<int>(other.absolute_days());
    }
};
