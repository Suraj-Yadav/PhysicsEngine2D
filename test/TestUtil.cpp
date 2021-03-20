#include "TestUtil.hpp"

#include <algorithm>
#include <random>

std::random_device rd;
static std::mt19937 gen(rd());

std::vector<Vector2D> getRandomPoints(
	const Range2D<dataType>& range2D, size_t N) {
	std::uniform_real_distribution<> x(
		range2D.rangeX.start, range2D.rangeX.end);
	std::uniform_real_distribution<> y(
		range2D.rangeY.start, range2D.rangeY.end);
	std::vector<Vector2D> points;
	points.reserve(N);
	for (size_t i = 0; i < N; i++) {
		points.emplace_back(x(gen), y(gen));
	}
	return points;
}

std::vector<int> getShuffledArrayOf1ToN(size_t N) {
	std::vector<int> arr(N);
	std::iota(arr.begin(), arr.end(), 1);
	std::shuffle(arr.begin(), arr.end(), gen);
	return arr;
}

Range2D<dataType> getRandom2DRange(
	const Range2D<dataType>& posRange, const Range2D<dataType>& sizeRange) {
	const auto width = std::uniform_real_distribution<>(
		sizeRange.rangeX.start, sizeRange.rangeX.end)(gen);
	const auto height = std::uniform_real_distribution<>(
		sizeRange.rangeY.start, sizeRange.rangeY.end)(gen);

	const auto left = std::uniform_real_distribution<>(
		posRange.rangeX.start,
		std::max(posRange.rangeX.start, posRange.rangeX.end - width))(gen);

	const auto bottom = std::uniform_real_distribution<>(
		posRange.rangeY.start,
		std::max(posRange.rangeY.start, posRange.rangeY.end - height))(gen);

	return Range2D<dataType>(left, left + width, bottom, bottom + height);
}

std::vector<Particle> getRandomParticles(
	const Range2D<dataType>& posRange, const Range<dataType>& radiusRange,
	const Range<dataType>& massRange, size_t N) {
	auto positions = getRandomPoints(posRange, N);

	std::uniform_real_distribution<> rad(radiusRange.start, radiusRange.end);
	std::uniform_real_distribution<> mass(massRange.start, massRange.end);

	std::vector<Particle> particles;
	for (int i = 0; i < N; ++i) {
		particles.emplace_back(positions[i], Vector2D(), mass(gen), rad(gen));
	}
	return particles;
}