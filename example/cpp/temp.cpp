#include <PhysicsEngine2D/Range.hpp>
#include <PhysicsEngine2D/RangeTree2D.hpp>
#include <PhysicsEngine2D/Simulator.hpp>
#include <PhysicsEngine2D/util.hpp>
#include <filesystem>
#include <fstream>
#include <random>

#include "drawUtil.hpp"

void print_exception(const std::exception &e, int level = 0) {
	std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
	try {
		std::rethrow_if_nested(e);
	}
	catch (const std::exception &e) {
		print_exception(e, level + 1);
	}
	catch (...) {
	}
}

void initialize(const std::filesystem::path filePath, Simulator &sim) {
	sim.clear();
	std::ifstream file(filePath.string());
	std::string line;
	std::string type;

	// Will be used to obtain a seed for the random number engine
	std::random_device rd;

	// Standard mersenne_twister_engine seeded with rd()
	std::mt19937 gen(rd());

	for (size_t lineNumber = 1; std::getline(file, line); lineNumber++) {
		std::istringstream iss(line);
		iss >> type;
		try {
			if (type.front() == '#') {
			}
			else if (type == "SIZE") {
				unsigned W, H;
				double left, top, right, bottom;
				if (!(iss >> W >> H >> left >> top >> right >> bottom)) {
					throw std::invalid_argument("Invalid 'SIZE' input");
				}
				// printLn(left, right, bottom, top);
			}
			else if (type == "TITLE") {
				std::string title;
				if (!(iss >> title)) {
					throw std::invalid_argument("Invalid 'TITLE' input");
				}
			}
			else if (type == "LINE") {
				Vector2D a, b;
				if (!(iss >> a.x >> a.y >> b.x >> b.y)) {
					throw std::invalid_argument("Invalid 'LINE' input");
				}
				sim.addObject(new Line(a, b));
			}
			else if (type == "PARTICLE") {
				double mass = 1, radius = 1;
				Vector2D position, velocity;
				if (!(iss >> mass >> radius >> position.x >> position.y)) {
					throw std::invalid_argument("Invalid 'PARTICLE' input");
				}
				if (iss >> velocity.x) {
					if (!(iss >> velocity.y)) {
						throw std::invalid_argument("Invalid 'PARTICLE' input");
					}
				}

				sim.addObject(new Particle(position, velocity, mass, radius));
			}
			else if (type == "BALL") {
				double mass = 1, radius = 1, angle = 0, angularVelocity = 0;
				Vector2D position, velocity;
				if (!(iss >> mass >> radius >> position.x >> position.y)) {
					throw std::invalid_argument("Invalid 'BALL' input");
				}
				if (iss >> velocity.x) {
					if (!(iss >> velocity.y)) {
						throw std::invalid_argument("Invalid 'BALL' input");
					}
				}
				iss >> angle;
				iss >> angularVelocity;

				sim.addObject(new Ball(
					position, velocity, mass, radius, angle, angularVelocity));
			}
			else if (type == "GRAVITY") {
				dataType x, y;
				if (!(iss >> x >> y)) {
					throw std::invalid_argument("Invalid 'GRAVITY' input");
				}
				sim.addForceField(ForceField(
					[x, y](const DynamicShape &a, const ForceField &) {
						return Vector2D(x, y) * a.mass;
					}));
			}
			else if (type == "REPEAT") {
				int repeatCount = 0;
				std::string itemType = "";
				if (!(iss >> repeatCount >> itemType)) {
				}
				if (itemType == "PARTICLE") {
					double massMin, massMax, radMin, radMax, xMin, xMax, yMin,
						yMax;
					if (!(iss >> massMin >> massMax >> radMin >> radMax >>
						  xMin >> xMax >> yMin >> yMax)) {
						throw std::invalid_argument("Invalid 'REPEAT' input");
					}
					std::uniform_real_distribution<> mass(massMin, massMax);
					std::uniform_real_distribution<> rad(radMin, radMax);
					std::uniform_real_distribution<> x(xMin, xMax);
					std::uniform_real_distribution<> y(yMin, yMax);
					for (int i = 0; i < std::max(0, repeatCount); i++) {
						sim.addObject(new Particle(
							{x(gen), y(gen)}, {0, 0}, mass(gen), rad(gen)));
					}
				}
			}
			else if (type == "END") {
				return;
			}
			else {
				throw std::invalid_argument("Unknown type '" + type + "'");
			}
		}
		catch (const std::exception &e) {
			print_exception(e);
			throw;
		}
	}
}

int main(int argc, char **argv) {
	NORMAL_IO_SPEEDUP;

	// const auto SIZE = 10;

	// const Range2D<dataType> range2d(-40.0, -10.0, -20.0, 20.0);

	// write(std::cout, SRC, debug(SIZE), "\n");

	// std::cout << "ASY import unicode;\n";
	// std::cout << "ASY unitsize(2mm);\n";
	// std::cout << "ASY draw(box((-40,-40), (40,40)));\n";
	// writeF(
	// 	std::cout, "ASY draw(box((%,%), (%,%)));\n", range2d.rangeX.start,
	// 	range2d.rangeY.start, range2d.rangeX.end, range2d.rangeY.end);

	// // std::cout << "size(100.5,100.5);";

	// std::vector<std::string> args;
	// for (int i = 0; i < argc; i++) {
	// 	args.emplace_back(argv[i]);
	// }

	// std::random_device rd;
	// std::mt19937 gen(rd());

	// const auto left = -40 * 1, right = 40 * 1, bottom = -40 * 1, top = 40 *
	// 1; std::uniform_real_distribution<> x(left, right);
	// std::uniform_real_distribution<> y(bottom, top);

	// std::vector<Vector2D> points;
	// std::vector<int> values;

	// for (size_t i = 0; i < SIZE; i++) {
	// 	points.emplace_back(x(gen), y(gen));
	// 	values.emplace_back(i);
	// }

	// RangeTree2D<int> rangeTree(points, values);

	// auto p = rangeTree.rangeQuery(range2d);
	// writeC(std::cout, p);

	Particle p({-1, -2}, {-3, -4}, 5, 6);
	auto &d = static_cast<DynamicShape &>(p);
	auto &p1 = static_cast<Particle &>(d);

	printLn("p", &p, p.getClass(), p.pos, p.vel, p.rad, p.mass);
	printLn("p1", &p1, p1.getClass(), p1.pos, p1.vel, p1.rad, p1.mass);

	p1.applyImpulse({7, 8}, {0, 0});

	printLn("p", &p, p.getClass(), p.pos, p.vel, p.rad, p.mass);
	printLn("p1", &p1, p1.getClass(), p1.pos, p1.vel, p1.rad, p1.mass);

	return 0;
}