#ifndef TAG_HPP
#define TAG_HPP

#include "../Base/AI.hpp"

class Tag : public HasAI {
public:
	std::string name;
	unsigned int color;
	std::vector<Army *> armies;
	int gold;

	Tag() {
		ai = AI();
		name = "";
		color = 0;
		armies = std::vector<Army *>();
		gold = 0;
	}

	Tag(const std::string &name, const unsigned int color) {
		ai = AI();
		this->name = name;
		this->color = color;
		armies = std::vector<Army *>();
		gold = 0;
	}

	~Tag() override {
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

	void updateAI() override {

	}
};

#endif // TAG_HPP
