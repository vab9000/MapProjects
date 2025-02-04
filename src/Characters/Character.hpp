//
// Created by varun on 2/4/2025.
//

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

	int age(const Date currentDate) const {
		return currentDate.year - birthday.year;
	}
};


#endif // CHARACTER_HPP
