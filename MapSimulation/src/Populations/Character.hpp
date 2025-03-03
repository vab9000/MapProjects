#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <unordered_set>
#include "../Base/Utils.hpp"
#include "../Base/AI.hpp"

enum class PersonalityTrait {
	AMBITIOUS,
};

class Character : public HasAI {
public:
	std::unordered_set<PersonalityTrait> traits;
	Date birthday{};

	explicit Character() {
		traits = std::unordered_set<PersonalityTrait>();
		birthday = Date(0, 0, 0);
		ai = AI();
	}

	[[nodiscard]] int age(const Date currentDate) const {
		return static_cast<int>(currentDate.year) - static_cast<int>(birthday.year);
	}
};


#endif // CHARACTER_HPP
