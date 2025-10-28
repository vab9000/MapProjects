#ifndef MECHANICS_DATE
#define MECHANICS_DATE
#include <chrono>
#include <string>

namespace mechanics {
    enum class tick_t : unsigned char {
        day, month, year
    };

    class date {
        std::chrono::sys_days time_;

    public:
        date();

        date(const date &other) = default;

        [[nodiscard]] auto to_string() const -> std::string;

        auto operator<=>(const date &other) const -> std::strong_ordering;

        auto operator+(int n_days) const -> date;

        auto operator-(const date &other) const -> int;

        auto operator+=(unsigned int n_days) -> date &;

        auto advance() -> tick_t;

        auto operator==(const date &date) const -> bool;
    };
}
#endif
