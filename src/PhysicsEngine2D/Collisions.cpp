#include <PhysicsEngine2D/Collisions.hpp>
#include <PhysicsEngine2D/IntervalTree.hpp>

std::vector<std::pair<int, int>> getCollisionBruteForce(
	const std::vector<std::reference_wrapper<BaseShape>> &objects) {
	std::vector<std::pair<int, int>> collisions;
	for (size_t i = 0; i < objects.size(); i++) {
		for (int j = i + 1; j < objects.size(); ++j) {
			if (objects[i].get().intersects(objects[j])) {
				collisions.emplace_back(i, j);
			}
		}
	}
	return collisions;
}

struct Temp {
	int index;
	dataType left;

	bool operator<(const Temp &that) { return this->left < that.left; }
};

std::vector<std::pair<int, int>> getCollisionBruteForceSAT(
	const std::vector<std::reference_wrapper<BaseShape>> &objects) {
	std::vector<std::pair<int, int>> collisions;

	std::vector<Temp> sortedObj(objects.size());
	for (size_t i = 0; i < objects.size(); i++) {
		sortedObj[i].index = i;
		sortedObj[i].left = objects[i].get().left;
	}

	std::sort(sortedObj.begin(), sortedObj.end());

	for (size_t i = 0; i < sortedObj.size(); i++) {
		auto &firstObj = objects[sortedObj[i].index].get();
		for (int j = i + 1; j < sortedObj.size(); ++j) {
			auto &secondObj = objects[sortedObj[j].index].get();
			if (firstObj.intersects(secondObj)) {
				collisions.emplace_back(
					std::minmax(sortedObj[i].index, sortedObj[j].index));
			}
			if (secondObj.left > firstObj.right) {
				break;
			}
		}
	}
	// Sort
	std::sort(collisions.begin(), collisions.end());

	return collisions;
}

struct Event {
	dataType xCoord;
	bool isStart;
	int index;
};

inline bool operator<(const Event &a, const Event &b) {
	return a.xCoord < b.xCoord ||
		   (a.xCoord == b.xCoord && a.isStart < b.isStart);
}

std::vector<std::pair<int, int>> getCollisionIntervalTree(
	const std::vector<std::reference_wrapper<BaseShape>> &objects) {
	std::vector<std::pair<int, int>> collisions;
	AVL<double, int> st;
	std::vector<Event> xEvents;

	st.reserve(objects.size());
	xEvents.resize(2 * objects.size());

	for (size_t i = 0, j = 0; i < objects.size(); ++i, j += 2) {
		xEvents[j].xCoord = objects[i].get().left;
		xEvents[j].isStart = true;
		xEvents[j].index = i;
		xEvents[j | 1].xCoord = objects[i].get().right;
		xEvents[j | 1].isStart = false;
		xEvents[j | 1].index = i;
	}

	std::sort(xEvents.begin(), xEvents.end());

	for (auto &event : xEvents) {
		if (event.isStart) {
			auto list = st.searchAll(
				objects[event.index].get().bottom,
				objects[event.index].get().top);
			for (auto &j : list) {
				if (objects[event.index].get().intersects(objects[j]) &&
					(objects[event.index].get().getClass() != LINE ||
					 objects[j].get().getClass() != LINE)) {
					if (event.index > j) {
						collisions.emplace_back(j, event.index);
					}
					else {
						collisions.emplace_back(event.index, j);
					}
				}
			}
			st.insert(
				objects[event.index].get().bottom,
				objects[event.index].get().top, event.index);
		}
		else {
			st.remove(
				objects[event.index].get().bottom,
				objects[event.index].get().top, event.index);
		}
		// st.printTree();
	}
	return collisions;
}