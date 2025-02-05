#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <unordered_set>
#include "../Base/Utils.hpp"

enum class PersonalityTraits {
	AMBITIOUS,
};

class Character {
public:
	std::unordered_set<PersonalityTraits> *traits;
	Date birthday{};

	explicit Character() {
		traits = new std::unordered_set<PersonalityTraits>();
		birthday = Date(0, 0, 0);
	}

	~Character() {
		delete traits;
	}

	[[nodiscard]] int age(const Date currentDate) const {
		return static_cast<int>(currentDate.year) - static_cast<int>(birthday.year);
	}
};


#endif // CHARACTER_HPP
