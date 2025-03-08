#pragma once

#include <string>

enum class MapMode {
	PROVINCES,
	OWNER,
};

struct Date {
	enum class Month {
		JANUARY = 1,
		FEBRUARY = 2,
		MARCH = 3,
		APRIL = 4,
		MAY = 5,
		JUNE = 6,
		JULY = 7,
		AUGUST = 8,
		SEPTEMBER = 9,
		OCTOBER = 10,
		NOVEMBER = 11,
		DECEMBER = 12
	};

	static unsigned int daysInMonth(const Month month, const unsigned int year) {
		switch (month) {
			case Month::JANUARY:
			case Month::MARCH:
			case Month::MAY:
			case Month::JULY:
			case Month::AUGUST:
			case Month::OCTOBER:
			case Month::DECEMBER:
				return 31;
			case Month::APRIL:
			case Month::JUNE:
			case Month::SEPTEMBER:
			case Month::NOVEMBER:
				return 30;
			case Month::FEBRUARY:
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

	Date() : Date(0, 1, 1) {}

	Date(const unsigned int year, const unsigned int month, const unsigned int day) : year(year), month(month), day(day) {}

	explicit Date(const Date *other) : Date(other->year, other->month, other->day) {}

	[[nodiscard]] Date clone() const { return Date(this); }

	[[nodiscard]] unsigned int absoluteDays() const {
		unsigned int totalDays = 0;

		for (unsigned int y = 0; y < year; ++y) {
			totalDays += (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? 366 : 365;
		}

		for (unsigned int m = 1; m < month; ++m) {
			totalDays += daysInMonth(static_cast<Month>(m), year);
		}

		totalDays += day;

		return totalDays;
	}

	[[nodiscard]] std::string toString() const {
		return std::to_string(day) + '/' + std::to_string(month) + '/' + std::to_string(year);
	}

	bool operator==(const Date &other) const { return year == other.year && month == other.month && day == other.day; }

	Date operator+(const int days) const {
		unsigned int newDay = day;
		unsigned int newMonth = month;
		unsigned int newYear = year;
		if (days < 0) {
			newDay -= days;
			while (newDay < 1) {
				newMonth -= 1;
				if (newMonth < 1) {
					newYear -= 1;
					newMonth = 12;
				}
				newDay += daysInMonth(static_cast<Month>(newMonth), newYear);
			}
		} else if (days > 0) {
			newDay += days;
			while (newDay > daysInMonth(static_cast<Month>(newMonth), newYear)) {
				newDay -= daysInMonth(static_cast<Month>(newMonth), newYear);
				newMonth += 1;
				if (newMonth > 12) {
					newYear += 1;
					newMonth = 1;
				}
			}
		}
		return { newYear, newMonth, newDay };
	}

	Date operator-(const int days) const { return *this + -days; }

	int operator-(const Date &other) const {
		return static_cast<int>(absoluteDays()) - static_cast<int>(other.absoluteDays());
	}
};
