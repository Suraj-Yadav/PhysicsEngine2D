#ifndef TEST_UTIL_HPP
#define TEST_UTIL_HPP

#include <PhysicsEngine2D/Collisions.hpp>
#include <PhysicsEngine2D/KdTree.hpp>
#include <PhysicsEngine2D/Range.hpp>
#include <PhysicsEngine2D/RangeTree2D.hpp>
#include <PhysicsEngine2D/Shapes.hpp>
#include <PhysicsEngine2D/Vector2D.hpp>
#include <PhysicsEngine2D/util.hpp>

namespace std {
	template <typename T>
	ostream &operator<<(ostream &os, const vector<T> &in) {
		writeContainer(os, in);
		return os;
	}
	template <typename T1, typename T2>
	ostream &operator<<(ostream &os, const pair<T1, T2> &in) {
		return os << "(" << in.first << "," << in.second << ")";
	}

}  // namespace std

std::vector<Vector2D> getRandomPoints(
	const Range2D<dataType> &range2D, size_t N);

std::vector<int> getShuffledArrayOf1ToN(size_t N);
Range2D<dataType> getRandom2DRange(
	const Range2D<dataType> &positionRange, const Range2D<dataType> &sizeRange);

template <class Tree>
Tree getRandomRangeTree(const Range2D<dataType> &range2D, size_t N) {
	auto points = getRandomPoints(range2D, N);
	auto values = getShuffledArrayOf1ToN(N);
	return Tree(points, values);
}

std::vector<Particle> getRandomParticles(
	const Range2D<dataType> &posRange, const Range<dataType> &radiusRange,
	const Range<dataType> &massRange, size_t N);

struct BruteForceCollision {
	static auto getCollisions(
		const std::vector<std::reference_wrapper<BaseShape>> &objects) {
		return getCollisionBruteForce(objects);
	}
};

struct BruteForceSATCollision {
	static auto getCollisions(
		const std::vector<std::reference_wrapper<BaseShape>> &objects) {
		return getCollisionBruteForceSAT(objects);
	}
};

struct IntervalTreeCollision {
	static auto getCollisions(
		const std::vector<std::reference_wrapper<BaseShape>> &objects) {
		return getCollisionIntervalTree(objects);
	}
};

// template <typename Tree> struct RangeQueryCollision {
// 	static auto getCollisions(
// 		const std::vector<std::reference_wrapper<BaseShape>> &objects) {
// 		return getCollisionRangeQuery<Tree>(objects);
// 	}
// };
#endif	// TEST_UTIL_HPP