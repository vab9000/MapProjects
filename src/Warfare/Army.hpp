#ifndef ARMY_HPP
#define ARMY_HPP

#include <vector>
#include "../Base/AI.hpp"
#include "../Base/Province.hpp"
#include "../Populations/Character.hpp"


class Tag;
class Army;

enum class ArmyDirectiveType { ATTACK };

struct ArmyDirective {
	ArmyDirectiveType type;
	void *target;
};

class Unit {
public:
	unsigned int size;
	Province *location;
	Army *army;
	std::vector<Province *> *path;
	double travelProgress;
	Character *general;
	bool retreating;

	explicit Unit(Army *army, Province *location);

	~Unit();

	void setDestination(Province *destination);
};

class Army {
	AI *ai;
	ArmyDirective directive{};

public:
	std::vector<Unit *> *units;
	Tag *tag;

	explicit Army(Tag *tag);

	~Army();
};


#endif // ARMY_HPP
