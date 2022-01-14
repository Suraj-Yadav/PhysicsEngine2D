#include <benchmark/benchmark.h>

#include <algorithm>
#include <random>

#include "TestUtil.hpp"

template <class Collision> void BM_GetCollision(benchmark::State& state) {
	const size_t length = state.range();
	const auto areaMultiplier = std::sqrt(length / (1 << 5));
	const auto areaBounds =
		400 *
		areaMultiplier;	 // This is needed so that density of particles
						 // remain the same irrespective of number of points.
	auto particles = getRandomParticles(
		{-areaBounds, areaBounds, -areaBounds, areaBounds}, {0.1, 0.1}, {1, 2},
		length);
	std::vector<std::reference_wrapper<BaseShape>> objects(
		particles.begin(), particles.end());
	for (auto _ : state) {
		Collision::getCollisions(objects);
	}
	state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(BM_GetCollision, BruteForceCollision)
	->RangeMultiplier(2)
	->Range(1 << 5, 1 << 15)
	->Complexity();

BENCHMARK_TEMPLATE(BM_GetCollision, BruteForceSATCollision)
	->RangeMultiplier(2)
	->Range(1 << 5, 1 << 16)
	->Complexity();

BENCHMARK_TEMPLATE(BM_GetCollision, IntervalTreeCollision)
	->RangeMultiplier(2)
	->Range(1 << 5, 1 << 16)
	->Complexity();
