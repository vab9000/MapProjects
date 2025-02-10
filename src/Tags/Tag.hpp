#ifndef TAG_HPP
#define TAG_HPP

#include "../Base/AI.hpp"

class Tag {
public:
	std::string name;
	unsigned int color;
	std::vector<Army *> armies;
	AI ai;

	Tag() {
		name = "";
		color = 0;
		armies = std::vector<Army *>();
		ai = AI();
	}

	Tag(const std::string &name, const unsigned int color) {
		this->name = name;
		this->color = color;
		armies = std::vector<Army *>();
		ai = AI();
	}

	virtual ~Tag() {
		for (const auto &army: armies) {
            delete army;
        }
	}

	[[nodiscard]] virtual bool hasArmyAccess(const Province &province) const {
		return true;
	}

	[[nodiscard]] Army *newArmy() {
		auto army = new Army(*this);
		armies.emplace_back(army);
		return army;
	}
};

#endif // TAG_HPP
