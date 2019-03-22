#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <vector>

#include "Shapes.hpp"

// class Simulator {
// 	std::vector<std::shared_ptr<BaseShape>> objects;

//    public:
// 	template <typename T, typename... Args>
// 	std::shared_ptr<T> addShape(Args... args) {
// 		objects.push_back(std::make_shared<BaseShape>(new T(std::forward<Args>(args)...)));
// 		return objects.back();
// 	}
// };

int collisionSim(unsigned subStep);
std::vector<std::pair<int, int>> getCollisions(const std::vector<std::unique_ptr<BaseShape>> &objects);

#endif  // SIMULATION_H_INCLUDED
