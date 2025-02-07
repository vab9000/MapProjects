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
	double speed;

	Unit(const Army &army, const Province &location);

	~Unit();

	void setDestination(const Province &destination);

	void move();
};

class Army {
	AI *ai;
	ArmyDirective directive{};

public:
	std::vector<Unit *> *units;
	Tag *tag;

	explicit Army(const Tag &tag);

	~Army();

	[[nodiscard]] Unit *newUnit(const Province &location) const;
};


#endif // ARMY_HPP
