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
	std::vector<Province *> path;
	double travelProgress;
	Character *general;
	bool retreating;
	double speed;

	Unit(const Army &army, const Province &location);

	void setDestination(const Province &destination);

	void move();
};

class Army : public HasAI {
	ArmyDirective directive{};

public:
	std::vector<std::unique_ptr<Unit>> units;
	Tag *tag;

	explicit Army(const Tag &tag);

	void updateAI() override;

	[[nodiscard]] Unit *newUnit(const Province &location);
};


#endif // ARMY_HPP
