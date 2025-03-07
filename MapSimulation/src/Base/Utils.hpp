#pragma once

enum class MapMode {
	PROVINCES,
	OWNER,
};

class Date {
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

public:
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

	Date(const unsigned int year, const unsigned int month, const unsigned int day) {
		this->year = year;
		this->month = month;
		this->day = day;
	}

	explicit Date(const Date *other) {
		this->year = other->year;
		this->month = other->month;
		this->day = other->day;
	}

	[[nodiscard]] Date clone() const { return Date(this); }

	bool operator==(const Date &other) const { return year == other.year && month == other.month && day == other.day; }

	Date operator+(const int days) const {
		auto newDate = Date(this);
		if (days < 0) {
			newDate.day -= days;
			while (newDate.day < 1) {
				newDate.month -= 1;
				if (newDate.month < 1) {
					newDate.year -= 1;
					newDate.month = 12;
				}
				newDate.day += daysInMonth(static_cast<Month>(newDate.month), newDate.year);
			}
		} else if (days > 0) {
			newDate.day += days;
			while (newDate.day > daysInMonth(static_cast<Month>(newDate.month), newDate.year)) {
				newDate.day -= daysInMonth(static_cast<Month>(newDate.month), newDate.year);
				newDate.month += 1;
				if (newDate.month > 12) {
					newDate.year += 1;
					newDate.month = 1;
				}
			}
		}
		return newDate;
	}

	Date operator-(const int days) const { return *this + -days; }

	int operator-(const Date &other) const {
		return static_cast<int>(absoluteDays()) - static_cast<int>(other.absoluteDays());
	}

	[[nodiscard]] std::string toString() const {
		return std::to_string(day) + '/' + std::to_string(month) + '/' + std::to_string(year);
	}
};
