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

	std::vector<std::string> args;
	for (int i = 0; i < argc; i++) {
		args.emplace_back(argv[i]);
	}

	const auto initFilePath = std::filesystem::absolute(
		std::filesystem::path("example/cpp/bouncingBall.txt"));

	std::filesystem::path rootPath(
		std::filesystem::absolute(std::filesystem::path(argv[0]))
			.parent_path());

	Simulator sim(1, 0.9f, 0.9f);

	initialize(initFilePath, sim);

	sim.simulate(0.01, 1);

	return 0;
}