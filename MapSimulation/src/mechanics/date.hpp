#pragma once
#include <chrono>
#include <string>
#include "tickable.hpp"

namespace mechanics {
    class date {
        std::chrono::year_month_day time_;

        date(std::chrono::year y, std::chrono::month m, std::chrono::day d);

        explicit date(std::chrono::year_month_day ymd);

    public:
        date();

        explicit date(int y, unsigned int m, unsigned int d);

        [[nodiscard]] auto to_string() const -> std::string;

        auto operator<=>(const date &other) const -> std::strong_ordering;

        auto operator+(int_fast64_t n_days) const -> date;

        auto operator-(const date &other) const -> int_fast64_t;

        auto advance() -> tick_t;

        auto operator==(const date & date) const -> bool;
    };
}
