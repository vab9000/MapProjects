#include "date.hpp"

namespace {
    enum class month_t : uint_fast8_t {
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

    auto days_in_month(const month_t month, const uint_fast32_t year) -> uint_fast8_t {
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
                if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) { return 29; }
                return 28;
        }
        return 0;
    }
}

namespace mechanics {
    date::date() : date(0, 1, 1) {}

    date::date(const uint_fast32_t year, const uint_fast8_t month,
               const uint_fast8_t day) : year(year), month(month), day(day) {}

    date::date(const date *other) : date(other->year, other->month, other->day) {}

    auto date::absolute_days() const -> int_fast64_t {
        int_fast64_t total_days = 0;

        for (uint_fast32_t y = 0; y < year; ++y) {
            total_days += y % 4 == 0 && (y % 100 != 0 || y % 400 == 0) ? 366 : 365;
        }

        for (uint_fast8_t m = 1; m < month; ++m) { total_days += days_in_month(static_cast<month_t>(m), year); }

        total_days += day;

        return total_days;
    }

    auto date::to_string() const -> std::string {
        return std::to_string(day) + '/' + std::to_string(month) + '/' + std::to_string(year);
    }

    auto date::to_string(const uint_fast32_t epoch_year) const -> std::string {
        if (year >= epoch_year) {
            return std::to_string(day) + '/' + std::to_string(month) + '/' + std::to_string(year - epoch_year) + " AE";
        }
        return std::to_string(day) + '/' + std::to_string(month) + '/' + std::to_string(epoch_year - year) + " BE";
    }

    auto date::operator==(const date &other) const -> bool {
        return year == other.year && month == other.month && day == other.day;
    }

    auto date::operator<(const date &other) const -> bool {
        if (year < other.year) {
            return true;
        }
        if (year > other.year) {
            return false;
        }
        if (month < other.month) {
            return true;
        }
        if (month > other.month) {
            return false;
        }
        return day < other.day;
    }

    auto date::operator+(const int64_t days) const -> date {
        uint_fast8_t new_day = day;
        uint_fast8_t new_month = month;
        uint_fast32_t new_year = year;

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

    auto date::operator-(const int64_t days) const -> date { return *this + -days; }

    auto date::operator-(const date &other) const -> int64_t { return absolute_days() - other.absolute_days(); }
}
