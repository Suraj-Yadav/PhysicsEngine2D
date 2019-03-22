#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <functional>
#include <memory>
#include <random>
#include <set>
#include <vector>

#include "Shapes.hpp"

class ForceField {
	std::function<Vector2D(const DynamicShape &, const ForceField &)> func;

   public:
	Vector2D pos;
	ForceField(const std::function<Vector2D(const DynamicShape &,
											const ForceField &)> &f,
			   const Vector2D &p = Vector2D())
		: func(f), pos(p) {}
	Vector2D getForce(const DynamicShape &obj) { return func(obj, *this); }
	void setPos(const Vector2D &p) {
		pos = p;
	}
};

class Simulator {
	std::vector<std::pair<int, int>> getCollisions();
	bool manageCollision(Particle &first, Particle &second, float delTime);
	bool manageCollision(Ball &b, Line &l, float delTime);
	bool manageCollision(Box &b, Line &l, float delTime);
	bool manageCollision(Particle &b, Line &l, float delTime);
	unsigned subStep;

   public:
	float restitutionCoeff;
	Simulator(unsigned subStep, float restitutionCoeff);
	std::vector<std::shared_ptr<BaseShape>> objects;
	std::vector<ForceField> forceFields;

	void addObject(BaseShape *object);

	void addForceField(const ForceField forceField);

	void simulate(float delta, const std::set<std::pair<int, int>> &gravPairs);
};

#endif  // SIMULATION_H_INCLUDED
