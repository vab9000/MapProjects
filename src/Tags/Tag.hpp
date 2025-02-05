#ifndef TAG_HPP
#define TAG_HPP


class Tag {
public:
	std::string name;
	unsigned int color;

	Tag() {
		name = "";
		color = 0;
	}

	Tag(const std::string &name, const unsigned int color) {
		this->name = name;
		this->color = color;
	}
};

#endif // TAG_HPP
