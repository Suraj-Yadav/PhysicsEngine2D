#include <benchmark/benchmark.h>

#include <PhysicsEngine2D/Simulator.hpp>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());

BENCHMARK_MAIN();