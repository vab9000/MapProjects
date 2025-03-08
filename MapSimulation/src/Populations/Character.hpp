#pragma once

#include <unordered_set>
#include "../Base/Utils.hpp"
#include "../Base/AI.hpp"

enum class PersonalityTrait {
	AMBITIOUS,
};

class Army;

class Character : public HasAI {
	Date birthday{};
	bool commander{};
	Army* army;

public:
	std::unordered_set<PersonalityTrait> traits;

	explicit Character(Date birthday);

	[[nodiscard]] int age(Date currentDate) const;

	void updateAI() override;

	void makeCommander(Army *army);

	void removeCommander();

	[[nodiscard]] bool isCommander() const;
};
