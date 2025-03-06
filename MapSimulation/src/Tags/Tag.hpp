#ifndef TAG_HPP
#define TAG_HPP

#include "../Base/AI.hpp"

class Tag : public HasAI {
public:
	std::string name;
	unsigned int color;
	std::vector<std::unique_ptr<Army>> armies;
	int gold;

	Tag() {
		ai = AI();
		name = "";
		color = 0;
		armies = std::vector<std::unique_ptr<Army>>();
		gold = 0;
	}

	Tag(const std::string &name, const unsigned int color) {
		ai = AI();
		this->name = name;
		this->color = color;
		armies = std::vector<std::unique_ptr<Army>>();
		gold = 0;
	}

	[[nodiscard]] virtual bool hasArmyAccess(const Province &province) const {
		return true;
	}

	[[nodiscard]] Army *newArmy() {
		auto army = std::make_unique<Army>(*this);
		armies.emplace_back(std::move(army));
		return armies.back().get();
	}

	void updateAI() override {

	}
};

#endif // TAG_HPP
