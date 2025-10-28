#include "date.hpp"

namespace mechanics {
    using namespace std::chrono;

    date::date() : time_{year_month_day{year{0U}, month{1U}, day{1U}}} {}

    auto date::to_string() const -> std::string {
        const year_month_day ymd = time_;
        return std::to_string(static_cast<unsigned int>(ymd.day())) + "/" +
               std::to_string(static_cast<unsigned int>(ymd.month())) + "/" + std::to_string(
                   static_cast<int>(ymd.year()));
    }

    auto date::operator<=>(const date &other) const -> std::strong_ordering { return time_ <=> other.time_; }

    auto date::operator+(const int n_days) const -> date { return date{*this} += n_days; }

    auto date::operator-(const date &other) const -> int { return (time_ - other.time_).count(); }

    auto date::operator+=(const unsigned int n_days) -> date & {
        time_ += days{n_days};
        return *this;
    }

    auto date::advance() -> tick_t {
        ++time_;
        const year_month_day ymd = time_;
        if (ymd.day() != day{1U}) { return tick_t::day; }
        if (ymd.month() != month{0U}) { return tick_t::month; }
        return tick_t::year;
    }

    auto date::operator==(const date &date) const -> bool { return time_ == date.time_; }
}
