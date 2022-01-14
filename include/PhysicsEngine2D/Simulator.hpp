#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include "IntervalTree.hpp"
#include "KdTree.hpp"
#include "Shapes.hpp"

class ForceField {
	std::function<Vector2D(const DynamicShape &, const ForceField &)> func;

   public:
	Vector2D pos;
	ForceField(
		const std::function<Vector2D(const DynamicShape &, const ForceField &)>
			&f,
		const Vector2D &p = Vector2D())
		: func(f), pos(p) {}
	Vector2D getForce(const DynamicShape &obj) const {
		return func(obj, *this);
	}
	void setPos(const Vector2D &p) { pos = p; }
};

class Simulator {
	bool areReferencesValid = false;
	unsigned subStep;

	std::vector<ForceField> forceFields;

	std::vector<Line> lines;
	std::vector<Particle> particles;
	std::vector<Ball> balls;
	std::vector<Box> boxes;

	std::vector<std::reference_wrapper<DynamicShape>> dynamicShapes;
	std::vector<std::reference_wrapper<BaseShape>> baseShapes;

	void invalidateReferences();
	void updateReferences();

	template <typename T1, typename T2>
	bool manageCollision(T1 &t1, T2 &t2, float);

	template <typename T, typename... Args>
	void addObject(std::vector<T> &vec, Args &&...args) {
		vec.emplace_back(args...);
		invalidateReferences();
	}

   public:
	float restitutionCoeff;
	float frictionCoeff;
	float nBodyGravity;
	Simulator(
		unsigned subStep = 10, float restitutionCoeff = 1.0f,
		float frictionCoeff = 0.5f, float nBodyGravity = false);

	const std::vector<Line> &getLines() const;
	const std::vector<Particle> &getParticles() const;
	const std::vector<Ball> &getBalls() const;
	const std::vector<Box> &getBoxes() const;
	const std::vector<std::reference_wrapper<BaseShape>> &getBaseShapes() const;

	template <typename... Args> inline void addLine(Args &&...args) {
		addObject(lines, args...);
	}
	template <typename... Args> inline void addParticle(Args &&...args) {
		addObject(particles, args...);
	}
	template <typename... Args> inline void addBall(Args &&...args) {
		addObject(balls, args...);
	}
	template <typename... Args> inline void addBox(Args &&...args) {
		addObject(balls, args...);
	}

	void addForceField(const ForceField forceField);

	void simulate(float delta);

	void clear();
};

#endif	// SIMULATION_H_INCLUDED
