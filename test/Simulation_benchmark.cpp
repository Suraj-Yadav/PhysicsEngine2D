#include <benchmark/benchmark.h>

#include <PhysicsEngine2D/Simulator.hpp>
#include <random>

#include "TestUtil.hpp"

Simulator sim(10, 0.9f, 0.9f);
int64_t dataFilled = 0;

extern std::mt19937 gen;

static void BM_IntervalTree(benchmark::State& state) {
	for (auto _ : state) {
		state.PauseTiming();
		if (dataFilled != state.range(0)) {
			const auto left = -40 * state.range(0), right = 40 * state.range(0),
					   bottom = -40 * state.range(0), top = 40 * state.range(0);
			std::uniform_real_distribution<> x(left, right);
			std::uniform_real_distribution<> y(bottom, top);
			sim.clear();

			sim.addLine(Vector2D({left, bottom}), Vector2D({right, bottom}));
			sim.addLine(Vector2D({left, top}), Vector2D({left, bottom}));
			sim.addLine(Vector2D({right, top}), Vector2D({left, top}));
			sim.addLine(Vector2D({right, bottom}), Vector2D({right, top}));

			for (int i = 0; i < state.range(0); ++i) {
				sim.addParticle(Vector2D(x(gen), y(gen)), Vector2D(0, 0), 1, 1);
			}
			dataFilled = state.range(0);
		}
		state.ResumeTiming();
		sim.simulate(0.01);
	}
	state.SetComplexityN(state.range(0));
}

static void BM_KdTree(benchmark::State& state) {
	for (auto _ : state) {
		state.PauseTiming();
		if (dataFilled != state.range(0)) {
			const auto left = -40 * state.range(0), right = 40 * state.range(0),
					   bottom = -40 * state.range(0), top = 40 * state.range(0);
			std::uniform_real_distribution<> x(left, right);
			std::uniform_real_distribution<> y(bottom, top);
			sim.clear();

			sim.addLine(Vector2D({left, bottom}), Vector2D({right, bottom}));
			sim.addLine(Vector2D({left, top}), Vector2D({left, bottom}));
			sim.addLine(Vector2D({right, top}), Vector2D({left, top}));
			sim.addLine(Vector2D({right, bottom}), Vector2D({right, top}));

			for (int i = 0; i < state.range(0); ++i) {
				sim.addParticle(Vector2D(x(gen), y(gen)), Vector2D(0, 0), 1, 1);
			}
			dataFilled = state.range(0);
		}
		state.ResumeTiming();
		sim.simulate(0.01);
	}
	state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_KdTree)->RangeMultiplier(2)->Range(1 << 10, 1 << 20)->Complexity();

BENCHMARK(BM_IntervalTree)
	->RangeMultiplier(2)
	->Range(1 << 10, 1 << 20)
	->Complexity();