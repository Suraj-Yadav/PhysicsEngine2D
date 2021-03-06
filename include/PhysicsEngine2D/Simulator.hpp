#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <functional>
#include <memory>
#include <random>
#include <unordered_set>
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

struct pair_hasher {
	template <class T1, class T2>
	std::size_t operator()(std::pair<T1, T2> const &pair) const {
		std::size_t h1 = std::hash<T1>()(pair.first);
		std::size_t h2 = std::hash<T2>()(pair.second);

		return h1 * 10000 + h2;
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
	float frictionCoeff;
	Simulator(unsigned subStep = 10, float restitutionCoeff = 1.0f, float frictionCoeff = 0.5f);
	std::vector<std::shared_ptr<BaseShape>> objects;
	std::vector<ForceField> forceFields;

	void addObject(BaseShape *object);

	void addForceField(const ForceField forceField);

	void simulate(float delta);

	void clear();
};

#endif	// SIMULATION_H_INCLUDED
