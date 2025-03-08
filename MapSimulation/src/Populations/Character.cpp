#include "Character.hpp"

Character::Character(const Date birthday) {
	traits = std::unordered_set<PersonalityTrait>();
	this->birthday = birthday;
	commander = false;
	army = nullptr;
	ai = AI();
}

[[nodiscard]] int Character::age(const Date currentDate) const {
	return static_cast<int>(currentDate.year) - static_cast<int>(birthday.year);
}

void Character::updateAI() {

}

void Character::makeCommander(Army *army) {
	commander = true;
	this->army = army;
}

void Character::removeCommander() {
	commander = false;
	army = nullptr;
}

[[nodiscard]] bool Character::isCommander() const {
	return commander;
}
