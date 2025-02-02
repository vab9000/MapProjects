#ifndef PROVINCE_HPP
#define PROVINCE_HPP

#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include "../Tags/Country.hpp"
#include "Utils.hpp"

class Province {
	std::vector<std::array<int, 2>> *pixels;
	std::vector<std::pair<Province *, std::array<int, 2>>> *outline;
	std::array<int, 2> *lockedPixels{};
	std::pair<Province *, std::array<int, 2>> *lockedOutline{};
	bool locked{};
	Country* owner;

public:
	std::string name;
	unsigned int color;
	unsigned int baseColor;
	unsigned int numPixels;
	unsigned int numOutline;
	int bounds[4]{};
	int center[2]{};

	Province(const std::string &name, const unsigned int color, const int i, const int j) {
		locked = false;
		this->name = name;
		this->color = color;
		baseColor = color;
		bounds[0] = i;
		bounds[1] = j;
		bounds[2] = i;
		bounds[3] = j;
		numPixels = 0;
		numOutline = 0;
		pixels = new std::vector<std::array<int, 2>>();
		outline = new std::vector<std::pair<Province *, std::array<int, 2>>>();
		center[0] = -1;
		center[1] = -1;
		owner = nullptr;
	}

	~Province() {
		if (locked) {
			delete[] lockedPixels;
			delete[] lockedOutline;
		} else {
			delete pixels;
			delete outline;
		}
	}

	void lock() {
		if (locked == true) {
			return;
		}

		const auto totalPixels = numPixels + numOutline;

		const auto x = new int[totalPixels];
		const auto y = new int[totalPixels];

		locked = true;

		lockedPixels = new std::array<int, 2>[numPixels];
		for (int i = 0; i < numPixels; i++) {
			lockedPixels[i][0] = pixels->at(i)[0];
			lockedPixels[i][1] = pixels->at(i)[1];
			x[i] = lockedPixels[i][0];
			y[i] = lockedPixels[i][1];
		}
		delete pixels;

		lockedOutline = new std::pair<Province *, std::array<int, 2>>[numOutline];
		for (int i = 0; i < numOutline; i++) {
			lockedOutline[i].first = outline->at(i).first;
			lockedOutline[i].second[0] = outline->at(i).second[0];
			lockedOutline[i].second[1] = outline->at(i).second[1];
			x[i + numPixels] = lockedOutline[i].second[0];
			y[i + numPixels] = lockedOutline[i].second[1];
		}
		delete outline;

		std::sort(x, x + totalPixels);
		std::sort(y, y + totalPixels);

		int testCenter[2] = {0, 0};

		if (totalPixels % 2 == 0) {
			testCenter[0] = (x[totalPixels / 2] + x[totalPixels / 2 - 1]) / 2;
			testCenter[1] = (y[totalPixels / 2] + y[totalPixels / 2 - 1]) / 2;
		} else {
			testCenter[0] = x[totalPixels / 2];
			testCenter[1] = y[totalPixels / 2];
		}

		for (int i = 0; i < numPixels; i++) {
			if (lockedPixels[i][0] == testCenter[0] && lockedPixels[i][1] == testCenter[1]) {
				center[0] = testCenter[0];
				center[1] = testCenter[1];
				break;
			}
		}

		for (int i = 0; i < numOutline; i++) {
			if (lockedOutline[i].second[0] == testCenter[0] && lockedOutline[i].second[1] == testCenter[1]) {
				center[0] = testCenter[0];
				center[1] = testCenter[1];
				break;
			}
		}

		auto distance = [](const std::array<int, 2> &a, const std::array<int, 2> &b) {
			return sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]));
		};

		if (center[0] == -1) {
			double minDistance = (std::numeric_limits<double>::max)();
			for (int i = 0; i < numPixels; i++) {
				if (const auto dist = distance(lockedPixels[i], {testCenter[0], testCenter[1]});
					dist < minDistance) {
					minDistance = dist;
					center[0] = lockedPixels[i][0];
					center[1] = lockedPixels[i][1];
				}
			}
			for (int i = 0; i < numOutline; i++) {
				if (const auto dist = distance(lockedOutline[i].second, {testCenter[0], testCenter[1]});
				    dist < minDistance) {
					minDistance = dist;
					center[0] = lockedOutline[i].second[0];
					center[1] = lockedOutline[i].second[1];
				}
			}
		}

		delete[] x;
		delete[] y;
	}

	void setOwner(Country* newOwner) {
		if (owner != nullptr && owner->hasProvince(this)) {
			owner->removeProvince(this);
		}
		owner = newOwner;
	}

	[[nodiscard]] Country* getOwner() const {
		return owner;
	}

	void addPixel(const int x, const int y) {
		if (locked) {
			return;
		}
		pixels->push_back({x, y});
		numPixels += 1;
	}

	void addOutline(const int x, const int y, Province *other) {
		if (locked) {
			return;
		}
		outline->emplace_back(other, std::array{x, y});
		numOutline += 1;
	}

	[[nodiscard]] std::array<int, 2> *getPixels() const {
		if (locked) {
			return lockedPixels;
		}
		return pixels->data();
	}

	[[nodiscard]] std::pair<Province *, std::array<int, 2>> *getOutline() const {
		if (locked) {
			return lockedOutline;
		}
		return outline->data();
	}

	void expandBounds(const int x, const int y) {
		if (x < bounds[0]) {
			bounds[0] = x;
		} else if (x > bounds[2]) {
			bounds[2] = x;
		} else if (y < bounds[1]) {
			bounds[1] = y;
		}
		if (y > bounds[3]) {
			bounds[3] = y;
		}
	}

	void recolor(const MapMode mode) {
		if (mode == MapMode::PROVINCES) {
			color = baseColor;
		} else if (mode == MapMode::OWNER) {
			if (owner != nullptr) {
				color = owner->color;
			}
			else {
				color = 0xFFFFFFFF;
			}
		}
	}
};

#endif // PROVINCE_HPP
