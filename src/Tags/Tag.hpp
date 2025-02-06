#ifndef TAG_HPP
#define TAG_HPP


class Tag {
public:
	std::string name;
	unsigned int color;
	std::vector<Army *> *armies;
	AI *ai;

	Tag() {
		name = "";
		color = 0;
		armies = new std::vector<Army *>();
		ai = new AI();
	}

	Tag(const std::string &name, const unsigned int color) {
		this->name = name;
		this->color = color;
		armies = new std::vector<Army *>();
		ai = new AI();
	}

	virtual ~Tag() {
		for (const auto &army: *armies) {
            delete army;
        }

		delete ai;
		delete armies;
	}

	[[nodiscard]] virtual bool hasArmyAccess(const Province &province) const = 0;
};

#endif // TAG_HPP
