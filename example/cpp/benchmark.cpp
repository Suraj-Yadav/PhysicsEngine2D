#include <benchmark/benchmark.h>

#include <PhysicsEngine2D/Simulator.hpp>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());

Simulator sim(10, 0.9f, 0.9f);
int64_t dataFilled = 0;

static void BM_IntervalTree(benchmark::State& state) {
	for (auto _ : state) {
		state.PauseTiming();
		if (dataFilled != state.range(0)) {
			const auto left = -40 * state.range(0), right = 40 * state.range(0),
					   bottom = -40 * state.range(0), top = 40 * state.range(0);
			std::uniform_real_distribution<> x(left, right);
			std::uniform_real_distribution<> y(bottom, top);
			sim.clear();

			sim.addObject(new Line({left, bottom}, {right, bottom}));
			sim.addObject(new Line({left, top}, {left, bottom}));
			sim.addObject(new Line({right, top}, {left, top}));
			sim.addObject(new Line({right, bottom}, {right, top}));

			for (int i = 0; i < state.range(0); ++i) {
				sim.addObject(new Particle({x(gen), y(gen)}, {0, 0}, 1, 1));
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

			sim.addObject(new Line({left, bottom}, {right, bottom}));
			sim.addObject(new Line({left, top}, {left, bottom}));
			sim.addObject(new Line({right, top}, {left, top}));
			sim.addObject(new Line({right, bottom}, {right, top}));

			for (int i = 0; i < state.range(0); ++i) {
				sim.addObject(new Particle({x(gen), y(gen)}, {0, 0}, 1, 1));
			}
			dataFilled = state.range(0);
		}
		state.ResumeTiming();
		sim.simulate(0.01, 1);
	}
	state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_KdTree)->RangeMultiplier(2)->Range(1 << 10, 1 << 20)->Complexity();

BENCHMARK(BM_IntervalTree)
	->RangeMultiplier(2)
	->Range(1 << 10, 1 << 20)
	->Complexity();

BENCHMARK_MAIN();