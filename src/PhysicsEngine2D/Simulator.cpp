#include <PhysicsEngine2D/Simulator.hpp>
#include <PhysicsEngine2D/util.hpp>
#include <chrono>
#include <functional>
#include <memory>
#include <set>
#include <vector>

Simulator::Simulator(
	unsigned subStep, float restitutionCoeff, float frictionCoeff)
	: subStep(subStep),
	  restitutionCoeff(restitutionCoeff),
	  frictionCoeff(frictionCoeff) {}

void Simulator::addObject(BaseShape *object) {
	objects.emplace_back(std::unique_ptr<BaseShape>(object));
}

void Simulator::addForceField(const ForceField forceField) {
	forceFields.emplace_back(forceField);
}

std::string str(const Vector2D &a) {
	return "(" + std::to_string(a.x) + "," + std::to_string(a.y) + ")";
}

inline bool operator<(const Event &a, const Event &b) {
	return a.xCoord < b.xCoord ||
		   (a.xCoord == b.xCoord && a.isStart < b.isStart);
	// return std::tie(a.xCoord, a.isStart) < std::tie(b.xCoord, b.isStart);
}

std::vector<std::pair<int, int>> Simulator::getCollisions() {
	std::vector<std::pair<int, int>> collisions;

	st.reserve(objects.size());
	xEvents.resize(2 * objects.size());

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
			auto list = st.searchAll(
				objects[event.index]->bottom, objects[event.index]->top);
			for (auto &j : list) {
				if (objects[event.index]->intersects(*objects[j]) &&
					(objects[event.index]->getClass() != LINE ||
					 objects[j]->getClass() != LINE)) {
					if (event.index > j) {
						collisions.emplace_back(j, event.index);
					}
					else {
						collisions.emplace_back(event.index, j);
					}
				}
			}
			st.insert(
				objects[event.index]->bottom, objects[event.index]->top,
				event.index);
		}
		else {
			st.remove(
				objects[event.index]->bottom, objects[event.index]->top,
				event.index);
		}
		// st.printTree();
	}
	return collisions;
}

std::vector<std::pair<int, int>> Simulator::getCollisions1() {
	std::vector<std::pair<int, int>> collisions;
	std::vector<Vector2D> points;
	std::vector<size_t> values;
	points.reserve(4 * objects.size());
	values.reserve(4 * objects.size());

	for (size_t i = 0; i < objects.size(); i++) {
		const auto &obj = objects[i];
		values.emplace_back(i);
		points.emplace_back(obj->left, obj->bottom);

		values.emplace_back(i);
		points.emplace_back(obj->left, obj->top);

		values.emplace_back(i);
		points.emplace_back(obj->right, obj->bottom);

		values.emplace_back(i);
		points.emplace_back(obj->right, obj->top);
	}

	KdTree<size_t> kdTree(points, values);

	// kdTree.debugDraw(std::cerr);

	// for (size_t i = 0; i < objects.size(); i++) {
	// 	const auto &obj = objects[i];
	// 	const Range2D range2d(obj->left, obj->right, obj->bottom, obj->top);
	// 	// printLn(i, obj->left, obj->right, obj->bottom, obj->top);
	// 	auto inside = kdTree.rangeQuery<std::vector>(range2d);
	// 	for (auto &elem : inside) {
	// 		// if (i > elem) {
	// 		// 	printLn(elem, i);
	// 		// }
	// 		// else {
	// 		// 	printLn(i, elem);
	// 		// }
	// 		if (i != elem && objects[elem]->intersects(*objects[i]) &&
	// 			(objects[i]->getClass() != LINE ||
	// 			 objects[elem]->getClass() != LINE)) {
	// 			if (i > elem) {
	// 				collisions.emplace_back(elem, i);
	// 			}
	// 			else {
	// 				collisions.emplace_back(i, elem);
	// 			}
	// 		}
	// 	}
	// }

	return collisions;
}

bool Simulator::manageCollision(Particle &first, Particle &second, float) {
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
bool Simulator::manageCollision(Ball &b, Line &l, float) {
	float dist = distFromLine(l.start, l.end, b.pos);
	if (dist <= b.rad * b.rad) {
		dist = sqrt(dist) - b.rad;
		if (dist < 0.0f) b.pos -= dist * l.normal;
		if (b.vel.dot(l.normal) < 0.0) {
			const auto normalComp = (b.vel * b.mass).projOnUnit(l.normal),
					   normalImpulse = -(1 + restitutionCoeff) * normalComp;
			const auto [tangentialCompMag, tangentialCompDir] =
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
bool Simulator::manageCollision(Box &b, Line &l, float) {
	float dist = distFromLine(l.start, l.end, b.pos);
	if (dist <= b.w * b.h) {
		dist = sqrt(dist) - sqrt(b.w * b.h);
		if (dist < 0.0f) b.pos -= dist * l.normal;
		if (b.vel.dot(l.normal) < 0.0)
			b.applyImpulse(
				-(1 + restitutionCoeff) * b.mass * b.vel.projOnUnit(l.normal),
				b.pos +
					0.5 * sqrt(b.w * b.h) *
						Vector2D(
							std::cos(b.angle), std::sin(b.angle)));	 // commands
		// b.acc += -projOnUnit(b.acc, l.normal);
		return true;
	}
	return false;
}
bool Simulator::manageCollision(Particle &b, Line &l, float) {
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

void Simulator::debugDraw(std::ostream &out) {
	out << "unitsize(0.1cm);";
	for (size_t i = 0; i < objects.size(); i++) {
		const auto &object = objects.at(i);
		switch (object->getClass()) {
			case BASESHAPE:
				break;
			case DYNAMICSHAPE:
				break;
			case RIGIDSHAPE:
				break;
			case LINE: {
				auto obj = static_cast<Line *>(object.get());
				obj->debugDraw(out, std::to_string(i));
				break;
			}
			case PARTICLE: {
				auto obj = static_cast<Particle *>(object.get());
				obj->debugDraw(out, std::to_string(i));
				// printLn(
				// 	"PARTICLE", obj->mass, obj->rad, obj->pos.x, obj->pos.y);
				break;
			}
				// case BALL: {
				// 	const auto obj = static_cast<Ball *>(object.get());
				// 	drawUtil.drawCircle(obj->pos, obj->rad, sf::Color::Blue);
				// 	auto radiusVec =
				// 		Vector2D(std::cos(obj->angle), std::sin(obj->angle));
				// 	drawUtil.line(
				// 		obj->pos, obj->pos + obj->rad * radiusVec,
				// 		sf::Color::Yellow);
				// 	drawUtil.line(
				// 		obj->pos + radiusVec,
				// 		obj->pos + obj->rad * radiusVec +
				// 			obj->angVel * radiusVec.rotate(1, 0),
				// 		sf::Color::Yellow);
				// 	if (!pauseSimulation) {
				// 		auto KE = 0.5 * obj->mass * obj->vel.lenSq(),
				// 			 PE = 9.8 * obj->mass * obj->pos.y;
				// 		// printLn(time, ',', KE, ',', PE, ',', KE + PE);
				// 		// 	std::cout << time << ',' << obj->pos.x << ',' <<
				// 		// obj->pos.y << ',' << obj->vel.x << ',' << obj->vel.y
				// 		// << '\n';
				// 	}

				// 	break;
				// }
				// case BOX: {
				// 	auto obj = static_cast<Box *>(object.get());
				// 	drawUtil.quad(obj->corner, sf::Color::Blue);
				// 	break;
				// }
		}
		// if (showBox)
		// 	drawUtil.quad(
		// 		{Vector2D(object->left, object->top),
		// 		 Vector2D(object->right, object->top),
		// 		 Vector2D(object->right, object->bottom),
		// 		 Vector2D(object->left, object->bottom)},
		// 		sf::Color::Red);
	}
}

void Simulator::simulate(float seconds, int) {
	const float delta = seconds / subStep;
	for (unsigned step = 0; step < subStep; ++step) {
		for (auto &object : objects) {
			// switch (object->getClass()) {
			// 	case PARTICLE: {
			// 		auto obj = static_cast<Particle *>(object.get());
			// 		// obj->applyImpulse(gravity.getForce(*obj) * delta,
			// obj->pos); 		obj->move(delta); 		break;
			// 	}
			// 	case BALL: {
			// 		auto obj = static_cast<Ball *>(object.get());
			// 		// obj->applyImpulse(gravity.getForce(*obj) * delta,
			// obj->pos); 		obj->move(delta); 		break;
			// 	}
			// 	case BOX: {
			// 		auto obj = static_cast<Box *>(object.get());
			// 		// obj->applyImpulse(gravity.getForce(*obj) * delta,
			// obj->pos); 		obj->move(delta); 		break;
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
					obj->applyImpulse(
						forceField.getForce(*obj) * delta, obj->pos);
				}
			}
		}

		std::vector<std::pair<int, int>> possibleCollisions = getCollisions();
		std::vector<std::pair<int, int>> possibleCollisions1 = getCollisions1();

		// printLn(
		// 	debug(possibleCollisions.size()),
		// 	debug(possibleCollisions1.size()));

		std::sort(possibleCollisions.begin(), possibleCollisions.end());
		possibleCollisions.erase(
			std::unique(possibleCollisions.begin(), possibleCollisions.end()),
			possibleCollisions.end());

		std::sort(possibleCollisions1.begin(), possibleCollisions1.end());
		possibleCollisions1.erase(
			std::unique(possibleCollisions1.begin(), possibleCollisions1.end()),
			possibleCollisions1.end());

		std::vector<std::pair<int, int>> AMinusB, BMinusA;

		std::set_difference(
			possibleCollisions.begin(), possibleCollisions.end(),
			possibleCollisions1.begin(), possibleCollisions1.end(),
			std::inserter(AMinusB, AMinusB.begin()));

		std::set_difference(
			possibleCollisions1.begin(), possibleCollisions1.end(),
			possibleCollisions.begin(), possibleCollisions.end(),
			std::inserter(BMinusA, BMinusA.begin()));

		if (AMinusB.size() != 0 || BMinusA.size() != 0) {
			// printC(AMinusB);
			// printC(BMinusA);
			std::set<int> diffItems;
			for (auto &elem : AMinusB) {
				diffItems.insert(elem.first);
				diffItems.insert(elem.second);
			}
			for (auto &elem : BMinusA) {
				diffItems.insert(elem.first);
				diffItems.insert(elem.second);
			}

			// printLn(debug(AMinusB.size()), debug(BMinusA.size()));

			// for (auto &object : objects) {
			// 	const auto &object = objects.at(elem);
			// 	// if (object->getClass() == LINE) {
			// 	// 	auto obj = static_cast<Line *>(object.get());
			// 	// 	obj->debugDraw(std::cerr, std::to_string(elem));
			// 	// }
			// 	if (object->getClass() == PARTICLE) {
			// 		auto obj = static_cast<Particle *>(object.get());
			// 		obj->debugDraw(std::cerr, std::to_string(elem));
			// 		printLn(
			// 			"PARTICLE", obj->mass, obj->rad, obj->pos.x,
			// 			obj->pos.y);
			// 	}
			// }
			// debugDraw(std::cerr);
		}

		// if (possibleCollisions.size() != possibleCollisions1.size()) {
		// printLn(
		// 	debug(possibleCollisions.size()),
		// 	debug(possibleCollisions1.size()));

		// printC(possibleCollisions);
		// printC(possibleCollisions1);
		// debugDraw(std::cerr);
		// }

		// switch (collisionHandler) {
		// 	case 1:
		// 		/* code */
		// 		possibleCollisions = getCollisions1();
		// 		break;

		// 	default:
		// 		possibleCollisions = getCollisions();
		// 		break;
		// }

		for (auto &p : possibleCollisions) {
			int i = p.first, j = p.second;
			ShapeType first = objects[i]->getClass(),
					  second = objects[j]->getClass();
			if (first == LINE && second == PARTICLE)
				manageCollision(
					*static_cast<Particle *>(objects[j].get()),
					*static_cast<Line *>(objects[i].get()), delta);
			if (first == LINE && second == BALL)
				manageCollision(
					*static_cast<Ball *>(objects[j].get()),
					*static_cast<Line *>(objects[i].get()), delta);
			else if (second == LINE && first == BALL)
				manageCollision(
					*static_cast<Ball *>(objects[i].get()),
					*static_cast<Line *>(objects[j].get()), delta);
			else if (second == LINE && first == PARTICLE)
				manageCollision(
					*static_cast<Particle *>(objects[i].get()),
					*static_cast<Line *>(objects[j].get()), delta);
			else if (first == PARTICLE && second == PARTICLE)
				manageCollision(
					*static_cast<Particle *>(objects[j].get()),
					*static_cast<Particle *>(objects[i].get()), delta);
		}
	}
}

void Simulator::clear() {
	this->forceFields.clear();
	this->objects.clear();
}
