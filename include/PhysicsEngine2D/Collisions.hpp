#ifndef COLLISION_H
#define COLLISION_H

#include <PhysicsEngine2D/util.hpp>
#include <memory>
#include <vector>

#include "Shapes.hpp"

std::vector<std::pair<int, int>> getCollisionBruteForce(
	const std::vector<std::reference_wrapper<BaseShape>> &objects);

std::vector<std::pair<int, int>> getCollisionBruteForceSAT(
	const std::vector<std::reference_wrapper<BaseShape>> &objects);

std::vector<std::pair<int, int>> getCollisionIntervalTree(
	const std::vector<std::reference_wrapper<BaseShape>> &objects);

#endif	// COLLISION_H