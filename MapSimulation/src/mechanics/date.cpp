#include "date.hpp"

namespace mechanics {
    using namespace std::chrono;

    date::date() : date{0, 1U, 1U} {}

    date::date(year y, month m, day d) : date{{y, m, d}} {}

    date::date(const year_month_day ymd) : time_{ymd} {}

    date::date(const int_fast32_t y, const uint_fast32_t m, const uint_fast32_t d) : date{year{y}, month{m}, day{d}} {}

    auto date::to_string() const -> std::string {
        return std::to_string(static_cast<uint_fast32_t>(time_.day())) + "/" +
               std::to_string(static_cast<uint_fast32_t>(time_.month())) + "/" + std::to_string(
                   static_cast<int_fast32_t>(time_.year()));
    }

    auto date::operator<=>(const date &other) const -> std::strong_ordering { return time_ <=> other.time_; }

    auto date::operator+(const int_fast64_t n_days) const -> date {
        return date{year_month_day{sys_days{time_} + days{n_days}}};
    }

    auto date::operator-(const date &other) const -> int_fast64_t {
        return (sys_days{time_} - sys_days{other.time_}).count();
    }

    auto date::advance() -> tick_t {
        time_ = sys_days{time_} + days{1};
        if (time_.day() != day{1U}) { return tick_t::day; }
        if (time_.month() != month{0U}) { return tick_t::month; }
        return tick_t::year;
    }

    auto date::operator==(const date &date) const -> bool { return time_ == date.time_; }
}
