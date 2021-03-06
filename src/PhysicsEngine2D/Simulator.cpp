#include <PhysicsEngine2D/Simulator.hpp>
#include <PhysicsEngine2D/util.hpp>
#include <functional>
#include <memory>
#include <vector>

#include "IntervalTree.hpp"

Simulator::Simulator(unsigned subStep, float restitutionCoeff, float frictionCoeff)
	: subStep(subStep), restitutionCoeff(restitutionCoeff), frictionCoeff(frictionCoeff) {
}

void Simulator::addObject(BaseShape *object) {
	objects.push_back(std::unique_ptr<BaseShape>(object));
}

void Simulator::addForceField(const ForceField forceField) {
	forceFields.push_back(forceField);
}

std::string str(const Vector2D &a) {
	return "(" + std::to_string(a.x) + "," + std::to_string(a.y) + ")";
}

struct Event {
	double xCoord;
	bool isStart;
	int index;
};
inline bool operator<(const Event &a, const Event &b) {
	return std::tie(a.xCoord, a.isStart) < std::tie(b.xCoord, b.isStart);
}

std::vector<std::pair<int, int>> Simulator::getCollisions() {
	std::vector<std::pair<int, int>> collisions;
	AVL<double, int> st;
	std::vector<Event> xEvents(2 * objects.size());
	for (size_t i = 0, j = 0; i < objects.size(); ++i, j += 2) {
		xEvents[j].xCoord = objects[i]->left;
		xEvents[j].isStart = true;
		xEvents[j].index = i;
		xEvents[j | 1].xCoord = objects[i]->right;
		xEvents[j | 1].isStart = false;
		xEvents[j | 1].index = i;
	}
	std::sort(xEvents.begin(), xEvents.end());
	for (auto &event : xEvents) {
		if (event.isStart) {
			auto list = st.searchAll(objects[event.index]->bottom, objects[event.index]->top);
			for (auto &j : list) {
				if (objects[event.index]->intersects(*objects[j]))
					collisions.push_back({event.index, j});
			}
			st.insert(objects[event.index]->bottom, objects[event.index]->top, event.index);
		}
		else {
			st.remove(objects[event.index]->bottom, objects[event.index]->top, event.index);
		}
	}
	return collisions;
}

bool Simulator::manageCollision(Particle &first, Particle &second, float delTime) {
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
			auto Impulse = (1 + restitutionCoeff) * vRel.projOnUnit(n) / (first.invMass + second.invMass);
			first.vel += Impulse * first.invMass;
			second.vel -= Impulse * second.invMass;
		}
		return true;
	}
	return false;
}
bool Simulator::manageCollision(Ball &b, Line &l, float delTime) {
	float dist = distFromLine(l.start, l.end, b.pos);
	if (dist <= b.rad * b.rad) {
		dist = sqrt(dist) - b.rad;
		if (dist < 0.0f)
			b.pos -= dist * l.normal;
		if (b.vel.dot(l.normal) < 0.0) {
			const auto normalComp = (b.vel * b.mass).projOnUnit(l.normal), normalImpulse = -(1 + restitutionCoeff) * normalComp;
			const auto [tangentialCompMag, tangentialCompDir] = (b.vel * b.mass - normalComp - b.angVel * b.inertia * b.rad * l.normal.rotate(1, 0)).getMagnitudeAndDirection();
			const auto frictionImpulse = -frictionCoeff * std::min(normalComp.len(), tangentialCompMag) * tangentialCompDir;
			b.applyImpulse(normalImpulse + frictionImpulse, b.pos - l.normal * b.rad);
		}
		// b.acc += -projOnUnit(b.acc, l.normal);
		return true;
	}
	return false;
}
bool Simulator::manageCollision(Box &b, Line &l, float delTime) {
	float dist = distFromLine(l.start, l.end, b.pos);
	if (dist <= b.w * b.h) {
		dist = sqrt(dist) - sqrt(b.w * b.h);
		if (dist < 0.0f)
			b.pos -= dist * l.normal;
		if (b.vel.dot(l.normal) < 0.0)
			b.applyImpulse(
				-(1 + restitutionCoeff) * b.mass * b.vel.projOnUnit(l.normal),
				b.pos +
					0.5 * sqrt(b.w * b.h) *
						Vector2D(std::cos(b.angle), std::sin(b.angle)));  // commands
		// b.acc += -projOnUnit(b.acc, l.normal);
		return true;
	}
	return false;
}
bool Simulator::manageCollision(Particle &b, Line &l, float delTime) {
	float dist = distFromLine(l.start, l.end, b.pos);
	if (dist <= b.rad * b.rad) {
		dist = sqrt(dist) - b.rad;
		if (dist < 0.0f)
			b.pos -= dist * l.normal;
		if (b.vel.dot(l.normal) < 0.0) {
			const auto normalComp = b.vel.projOnUnit(l.normal), normalImpulse = -(1 + restitutionCoeff) * normalComp;
			const auto [tangentialCompMag, tangentialCompDir] = (b.vel - normalComp).getMagnitudeAndDirection();
			const auto frictionImpulse = -frictionCoeff * std::min(normalComp.len(), tangentialCompMag) * tangentialCompDir;
			b.applyImpulse(normalImpulse + frictionImpulse, b.pos);
		}
		// b.acc += -projOnUnit(b.acc, l.normal);
		return true;
	}
	return false;
}
void Simulator::simulate(float seconds) {
	const float delta = seconds / subStep;
	for (unsigned step = 0; step < subStep; ++step) {
		for (auto &object : objects) {
			// switch (object->getClass()) {
			// 	case PARTICLE: {
			// 		auto obj = static_cast<Particle *>(object.get());
			// 		// obj->applyImpulse(gravity.getForce(*obj) * delta, obj->pos);
			// 		obj->move(delta);
			// 		break;
			// 	}
			// 	case BALL: {
			// 		auto obj = static_cast<Ball *>(object.get());
			// 		// obj->applyImpulse(gravity.getForce(*obj) * delta, obj->pos);
			// 		obj->move(delta);
			// 		break;
			// 	}
			// 	case BOX: {
			// 		auto obj = static_cast<Box *>(object.get());
			// 		// obj->applyImpulse(gravity.getForce(*obj) * delta, obj->pos);
			// 		obj->move(delta);
			// 		break;
			// 	}
			// 	case BASESHAPE:
			// 	case DYNAMICSHAPE:
			// 	case LINE:
			// 	case RIGIDSHAPE:
			// 		break;
			// }
			if (isTypeof(DYNAMICSHAPE, object->getClass())) {
				auto obj = static_cast<DynamicShape *>(object.get());
				obj->move(delta);
				for (auto &forceField : forceFields) {
					obj->applyImpulse(forceField.getForce(*obj) * delta, obj->pos);
				}
			}
		}
		auto possibleCollisions = getCollisions();

		for (auto &p : possibleCollisions) {
			int i = p.first, j = p.second;
			Type first = objects[i]->getClass(), second = objects[j]->getClass();
			if (first == LINE && second == PARTICLE)
				manageCollision(*static_cast<Particle *>(objects[j].get()),
								*static_cast<Line *>(objects[i].get()), delta);
			if (first == LINE && second == BALL)
				manageCollision(*static_cast<Ball *>(objects[j].get()),
								*static_cast<Line *>(objects[i].get()), delta);
			else if (second == LINE && first == BALL)
				manageCollision(*static_cast<Ball *>(objects[i].get()),
								*static_cast<Line *>(objects[j].get()), delta);
			else if (second == LINE && first == PARTICLE)
				manageCollision(*static_cast<Particle *>(objects[i].get()),
								*static_cast<Line *>(objects[j].get()), delta);
			else if (first == PARTICLE && second == PARTICLE)
				manageCollision(*static_cast<Particle *>(objects[j].get()),
								*static_cast<Particle *>(objects[i].get()), delta);
		}
	}
}

void Simulator::clear() {
	this->forceFields.clear();
	this->objects.clear();
}
