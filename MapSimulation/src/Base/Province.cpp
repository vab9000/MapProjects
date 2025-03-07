#include "Province.hpp"
#include <algorithm>
#include <array>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "../Populations/Pop.hpp"
#include "../Tags/Country.hpp"
#include "Utils.hpp"

Province::Province(const std::string &name, const unsigned int color, const int i, const int j) {
	distancesProcessed = false;
	this->name = name;
	this->color = color;
	baseColor = color;
	bounds[0] = i;
	bounds[1] = j;
	bounds[2] = i;
	bounds[3] = j;
	numPixels = 0;
	numOutline = 0;
	pixels = std::vector<std::array<int, 2>>();
	outline = std::vector<std::pair<Province *, std::array<int, 2>>>();
	center[0] = -1;
	center[1] = -1;
	owner = nullptr;
	pops = std::vector<Pop *>();
	neighbors = std::unordered_map<Province *, double>();
}

void Province::finalize() {
	const auto totalPixels = numPixels + numOutline;

	auto x = std::vector<int>(totalPixels);
	auto y = std::vector<int>(totalPixels);

	std::ranges::sort(x);
	std::ranges::sort(y);

	int testCenter[2] = {0, 0};

	if (totalPixels % 2 == 0) {
		testCenter[0] = (x[totalPixels / 2] + x[totalPixels / 2 - 1]) / 2;
		testCenter[1] = (y[totalPixels / 2] + y[totalPixels / 2 - 1]) / 2;
	} else {
		testCenter[0] = x[totalPixels / 2];
		testCenter[1] = y[totalPixels / 2];
	}

	for (int i = 0; i < numPixels; ++i) {
		if (pixels[i][0] == testCenter[0] && pixels[i][1] == testCenter[1]) {
			center[0] = testCenter[0];
			center[1] = testCenter[1];
			break;
		}
	}

	for (int i = 0; i < numOutline; ++i) {
		if (outline[i].second[0] == testCenter[0] && outline[i].second[1] == testCenter[1]) {
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
		for (int i = 0; i < numPixels; ++i) {
			if (const auto dist = distance(pixels[i], {testCenter[0], testCenter[1]}); dist < minDistance) {
				minDistance = dist;
				center[0] = pixels[i][0];
				center[1] = pixels[i][1];
			}
		}
		for (int i = 0; i < numOutline; ++i) {
			if (const auto dist = distance(outline[i].second, {testCenter[0], testCenter[1]}); dist < minDistance) {
				minDistance = dist;
				center[0] = outline[i].second[0];
				center[1] = outline[i].second[1];
			}
		}
	}
}

void Province::processDistances() {
	for (const auto neighbor: neighbors | std::views::keys) {
		neighbors.operator[](neighbor) = this->distance(*neighbor);
	}
	distancesProcessed = true;
}

void Province::setOwner(const Country &newOwner) {
	if (owner != nullptr && owner->hasProvince(this)) {
		owner->removeProvince(this);
	}
	owner = &const_cast<Country &>(newOwner);
}

[[nodiscard]] Country *Province::getOwner() const { return owner; }

void Province::addPixel(const int x, const int y) {
	pixels.push_back({x, y});
	numPixels += 1;
}

void Province::addOutline(const int x, const int y, const Province &other) {
	outline.emplace_back(&const_cast<Province &>(other), std::array{x, y});
	numOutline += 1;
	neighbors.emplace(&const_cast<Province &>(other), 0.0);
}

[[nodiscard]] const std::array<int, 2> *Province::getPixels() const { return pixels.data(); }

[[nodiscard]] const std::pair<Province *, std::array<int, 2>> *Province::getOutline() const { return outline.data(); }

void Province::expandBounds(const int x, const int y) {
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

void Province::recolor(const MapMode mode) {
	if (mode == MapMode::PROVINCES) {
		color = baseColor;
	} else if (mode == MapMode::OWNER) {
		if (owner != nullptr) {
			color = owner->color;
		} else {
			color = 0xFFFFFFFF;
		}
	}
}

[[nodiscard]] double Province::distance(const Province &other) const {
	if (distancesProcessed && neighbors.contains(&const_cast<Province &>(other))) {
		return neighbors.at(&const_cast<Province &>(other));
	}
	return sqrt((center[0] - other.center[0]) * (center[0] - other.center[0]) +
	            (center[1] - other.center[1]) * (center[1] - other.center[1]));
}

[[nodiscard]] std::vector<Province *> Province::getPathTo(const Province &destination,
                                                          bool (*accessible)(const Province &, void *),
                                                          double (*costModifier)(const Province &, void *),
                                                          void *param) {
	std::unordered_map<Province *, double> distances;
	std::unordered_map<Province *, Province *> previous;
	std::priority_queue<std::pair<double, Province *>, std::vector<std::pair<double, Province *>>, std::greater<>>
	        queue;

	distances[this] = 0;
	queue.emplace(0, this);

	auto reached = false;

	while (!queue.empty()) {
		auto [currentDistance, currentProvince] = queue.top();
		queue.pop();

		if (currentProvince == &const_cast<Province &>(destination)) {
			reached = true;
			break;
		}

		for (const auto &[neighborProvince, neighborDistance]: currentProvince->neighbors) {
			if (neighborProvince == nullptr || !accessible(*neighborProvince, param) ||
			    costModifier(*neighborProvince, param) < 0) {
				continue;
			}

			if (double newDistance = currentDistance + costModifier(*neighborProvince, param) * neighborDistance;
			    !distances.contains(neighborProvince) || newDistance < distances[neighborProvince]) {
				distances[neighborProvince] = newDistance;
				previous[neighborProvince] = currentProvince;
				queue.emplace(newDistance, neighborProvince);
			}
		}
	}

	if (!reached) {
		return {};
	}

	std::vector<Province *> path;
	for (Province *at = &const_cast<Province &>(destination); at != nullptr; at = previous[at]) {
		path.insert(path.begin(), at);
	}

	return path;
}

void Province::tick() {
	if (owner != nullptr) {
		owner->gold += 1;
	}
}

[[nodiscard]] unsigned int Province::getNumPixels() const { return numPixels; }

[[nodiscard]] unsigned int Province::getNumOutline() const { return numOutline; }

[[nodiscard]] const int *Province::getBounds() const { return bounds; }

[[nodiscard]] const int *Province::getCenter() const { return center; }

[[nodiscard]] const std::vector<Pop *> &Province::getPops() const { return pops; }

[[nodiscard]] const std::unordered_map<Province *, double> &Province::getNeighbors() const { return neighbors; }
