#include <benchmark/benchmark.h>

#include <PhysicsEngine2D/KdTree.hpp>
#include <PhysicsEngine2D/RangeTree2D.hpp>
#include <random>

#include "TestUtil.hpp"

template <class Tree> void BM_BuildTree(benchmark::State& state) {
	auto points = getRandomPoints({-400, 400, -400, 400}, state.range());
	auto values = getShuffledArrayOf1ToN(state.range());

	for (auto _ : state) {
		Tree tree(points, values);
	}
	state.SetComplexityN(state.range(0));
}

template <class Tree> void BM_RangeQuery(benchmark::State& state) {
	Tree tree = getRandomRangeTree<Tree>({-512, 512, -512, 512}, state.range());
	auto range2D = getRandom2DRange({-512, 512, -512, 512}, {10, 10, 10, 10});

	for (auto _ : state) {
		tree.rangeQuery(range2D);
	}
	state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(BM_BuildTree, KdTree<int>)
	->Range(1 << 5, 1 << 18)
	->Complexity();

BENCHMARK_TEMPLATE(BM_BuildTree, RangeTree2D<int>)
	->Range(1 << 5, 1 << 18)
	->Complexity();

BENCHMARK_TEMPLATE(BM_RangeQuery, KdTree<int>)
	->RangeMultiplier(2)
	->Range(1 << 5, 1 << 18)
	->Complexity();

BENCHMARK_TEMPLATE(BM_RangeQuery, RangeTree2D<int>)
	->RangeMultiplier(2)
	->Range(1 << 5, 1 << 18)
	->Complexity();
