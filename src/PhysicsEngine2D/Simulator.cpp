#include <PhysicsEngine2D/Collisions.hpp>
#include <PhysicsEngine2D/Simulator.hpp>
#include <PhysicsEngine2D/util.hpp>
#include <chrono>
#include <functional>
#include <memory>
#include <set>
#include <vector>

inline std::ostream &operator<<(std::ostream &out, const ShapeType &type) {
	return out << getShapeTypeName(type);
}

inline bool operator<(
	const std::reference_wrapper<BaseShape> &a,
	const std::reference_wrapper<BaseShape> &b) {
	return a.get().left < b.get().left;
}

Simulator::Simulator(
	unsigned subStep, float restitutionCoeff, float frictionCoeff,
	float nBodyGravity)
	: subStep(subStep),
	  restitutionCoeff(restitutionCoeff),
	  frictionCoeff(frictionCoeff),
	  nBodyGravity(nBodyGravity) {}

void Simulator::addForceField(const ForceField forceField) {
	forceFields.emplace_back(forceField);
}

void Simulator::invalidateReferences() { areReferencesValid = false; }

const std::vector<Line> &Simulator::getLines() const { return lines; }

const std::vector<Particle> &Simulator::getParticles() const {
	return particles;
}

const std::vector<std::reference_wrapper<BaseShape>> &Simulator::getBaseShapes()
	const {
	return baseShapes;
}

const std::vector<Ball> &Simulator::getBalls() const { return balls; }

const std::vector<Box> &Simulator::getBoxes() const { return boxes; }

void Simulator::updateReferences() {
	if (areReferencesValid) {
		return;
	}

	dynamicShapes.clear();
	baseShapes.clear();
	dynamicShapes.reserve(balls.size() + particles.size() + boxes.size());
	baseShapes.reserve(
		balls.size() + particles.size() + boxes.size() + lines.size());

	for (auto &elem : balls) {
		dynamicShapes.emplace_back(elem);
		baseShapes.emplace_back(elem);
	}

	for (auto &elem : particles) {
		dynamicShapes.emplace_back(elem);
		baseShapes.emplace_back(elem);
	}

	for (auto &elem : boxes) {
		dynamicShapes.emplace_back(elem);
		baseShapes.emplace_back(elem);
	}

	for (auto &elem : lines) {
		baseShapes.emplace_back(elem);
	}

	std::sort(baseShapes.begin(), baseShapes.end());
}

template <> bool Simulator::manageCollision(Ball &b, Line &l, float) {
	float dist = distFromLine(l.start, l.end, b.pos);
	if (dist <= b.rad * b.rad) {
		dist = sqrt(dist) - b.rad;
		if (dist < 0.0f) b.pos -= dist * l.normal;
		if (b.vel.dot(l.normal) < 0.0) {
			const auto normalComp = (b.vel * b.mass).projOnUnit(l.normal),
					   normalImpulse = -(1 + restitutionCoeff) * normalComp;
			const auto &&[tangentialCompMag, tangentialCompDir] =
				(b.vel * b.mass - normalComp -
				 b.angVel * b.inertia * b.rad * l.normal.rotate(1, 0))
					.getMagnitudeAndDirection();
			const auto frictionImpulse =
				-frictionCoeff * std::min(normalComp.len(), tangentialCompMag) *
				tangentialCompDir;
			b.applyImpulse(
				normalImpulse + frictionImpulse, b.pos - l.normal * b.rad);
		}
		// b.acc += -projOnUnit(b.acc, l.normal);
		return true;
	}
	return false;
}

template <>
bool Simulator::manageCollision(
	Particle &first, Particle &second, float delTime) {
	Vector2D n = second.pos - first.pos;
	float dist = n.lenSq();
	if (dist <= (first.rad + second.rad) * (first.rad + second.rad)) {
		auto vRel = second.vel - first.vel;
		dist = sqrt(dist);
		n = n / dist;
		dist -= first.rad + second.rad;
		if (dist < 0.0f) {
			first.vel += dist * n;
			second.vel -= dist * n;
		}
		if (first.vel.dot(n) > second.vel.dot(n)) {
			auto Impulse = (1 + restitutionCoeff) * vRel.projOnUnit(n) /
						   (first.invMass + second.invMass);
			first.vel += Impulse * first.invMass;
			second.vel -= Impulse * second.invMass;
		}
		return true;
	}
	return false;
}

template <> bool Simulator::manageCollision(Box &b, Line &l, float) {
	float dist = distFromLine(l.start, l.end, b.pos);
	if (dist <= b.w * b.h) {
		dist = sqrt(dist) - sqrt(b.w * b.h);
		if (dist < 0.0f) b.pos -= dist * l.normal;
		if (b.vel.dot(l.normal) < 0.0)
			b.applyImpulse(
				-(1 + restitutionCoeff) * b.mass * b.vel.projOnUnit(l.normal),
				b.pos + 0.5 * sqrt(b.w * b.h) *
							Vector2D(std::cos(b.angle), std::sin(b.angle)));
		// commands
		// b.acc += -projOnUnit(b.acc, l.normal);
		return true;
	}
	return false;
}

template <> bool Simulator::manageCollision(Particle &b, Line &l, float) {
	float dist = distFromLine(l.start, l.end, b.pos);
	if (dist <= b.rad * b.rad) {
		dist = sqrt(dist) - b.rad;
		if (dist < 0.0f) b.pos -= dist * l.normal;
		if (b.vel.dot(l.normal) < 0.0) {
			const auto normalComp = b.vel.projOnUnit(l.normal),
					   normalImpulse = -(1 + restitutionCoeff) * normalComp;
			const auto [tangentialCompMag, tangentialCompDir] =
				(b.vel - normalComp).getMagnitudeAndDirection();
			const auto frictionImpulse =
				-frictionCoeff * std::min(normalComp.len(), tangentialCompMag) *
				tangentialCompDir;
			b.applyImpulse(normalImpulse + frictionImpulse, b.pos);
		}
		// b.acc += -projOnUnit(b.acc, l.normal);
		return true;
	}
	return false;
}

void Simulator::simulate(float seconds) {
	updateReferences();
	const float delta = seconds / subStep;
	for (unsigned step = 0; step < subStep; ++step) {
		for (auto &objRef : dynamicShapes) {
			auto &obj = objRef.get();
			obj.move(delta);
			for (const auto &forceField : forceFields) {
				obj.applyImpulse(forceField.getForce(obj) * delta, obj.pos);
			}
		}
		if (nBodyGravity > 0) {
			for (size_t i = 0; i < dynamicShapes.size(); i++) {
				auto &a = dynamicShapes[i].get();
				for (size_t j = i + 1; j < dynamicShapes.size(); j++) {
					auto &b = dynamicShapes[j].get();
					auto const [mag, dir] =
						(b.pos - a.pos).getMagnitudeAndDirection();
					auto impulse =
						nBodyGravity * a.mass * b.mass * dir / (mag * mag);
					a.applyImpulse(impulse, a.pos);
					b.applyImpulse(-impulse, b.pos);
				}
			}
		}

		const auto possibleCollisions = getCollisionBruteForceSAT(baseShapes);

		for (auto &p : possibleCollisions) {
			auto &firstObj = baseShapes[p.first].get(),
				 &secondObj = baseShapes[p.second].get();
			ShapeType firstType = firstObj.getClass(),
					  secondType = secondObj.getClass();

			if (firstType == LINE && secondType == PARTICLE) {
				manageCollision(
					static_cast<Particle &>(secondObj),
					static_cast<Line &>(firstObj), seconds);
			}
			else if (firstType == PARTICLE && secondType == LINE) {
				manageCollision(
					static_cast<Particle &>(firstObj),
					static_cast<Line &>(secondObj), seconds);
			}
			else if (firstType == PARTICLE && secondType == PARTICLE) {
				manageCollision(
					static_cast<Particle &>(firstObj),
					static_cast<Particle &>(secondObj), seconds);
			}
		}
	}
}

void Simulator::clear() {
	forceFields.clear();
	balls.clear();
	boxes.clear();
	particles.clear();
	lines.clear();
	dynamicShapes.clear();
	baseShapes.clear();
}
