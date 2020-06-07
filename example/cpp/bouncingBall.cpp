#define _USE_MATH_DEFINES
#include <PhysicsEngine2D/Simulator.hpp>
#include <PhysicsEngine2D/util.hpp>
#include <TGUI/TGUI.hpp>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>

#include "drawUtil.hpp"

Vector2D gravity(const DynamicShape &a, const ForceField &f) {
	return 6.67408e-11 * (f.pos - a.pos).unit() * a.mass / (a.pos - f.pos).lenSq();
}

void initialize(const std::filesystem::path filePath, sf::RenderWindow &window, Simulator &sim) {
	sim.clear();
	std::ifstream file(filePath.string());
	std::string line;
	std::string type;
	const float scale = std::max(sf::VideoMode::getDesktopMode().width / 1920.0, 1.0);
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
				window.setSize({unsigned(W * scale), unsigned(H * scale)});
				sf::View view(sf::FloatRect(left, top, right - left, bottom - top));
				view.setViewport({0.0f, 0.0f, 1.0f, 1.0f});
				window.setView(view);
			}
			else if (type == "TITLE") {
				std::string title;
				if (!(iss >> title)) {
					throw std::invalid_argument("Invalid 'TITLE' input");
				}
				window.setTitle(title);
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
				sim.addObject(new Ball(position, velocity, mass, radius, angle, angularVelocity));
			}
			else if (type == "GRAVITY") {
				sim.addForceField(ForceField([](const DynamicShape &a, const ForceField &f) {
					return Vector2D(0.0f, -9.8f) * a.mass;
				}));
			}
			else if (type == "END") {
				return;
			}
			else {
				throw std::invalid_argument("Unknown type '" + type + "'");
			}
		}
		catch (const std::exception &e) {
			printF("%:%: %", filePath, lineNumber, e.what());
			throw;
		}
	}
}

int main(int argc, char **argv) {
	// NORMAL_IO_SPEEDUP;

	std::filesystem::path rootPath(std::filesystem::absolute(std::filesystem::path(argv[0])).parent_path());

	Simulator sim(10, 0.9f);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(800, 800), "Drawing Area", sf::Style::Close, settings);
	sf::RenderWindow controllerWindow(sf::VideoMode(500, 500), "Controls", sf::Style::Close, settings);
	tgui::Gui gui(controllerWindow);

	printLn(std::filesystem::absolute(std::filesystem::path(argv[0]).parent_path()));

	initialize(rootPath / "bouncingBall.txt", window, sim);

	DrawUtil drawUtil(window, "/Users/surajyadav/Documents/PhysicsEngine2D/fonts/Sunda_Prada.ttf");

	bool showBox = false;

	gui.loadWidgetsFromFile(rootPath / "controller-hd.form");

	auto resetButton = gui.get<tgui::Button>("resetButton");
	auto checkbox = gui.get<tgui::CheckBox>("checkbox");
	auto restitutionSlider = gui.get<tgui::Slider>("restitutionSlider");
	auto restitutionCoeffLabel = gui.get<tgui::Label>("restitutionCoeffLabel");
	auto frictionSlider = gui.get<tgui::Slider>("frictionSlider");
	auto frictionCoeffLabel = gui.get<tgui::Label>("frictionCoeffLabel");
	auto timeLabel = gui.get<tgui::Label>("timeLabel");

	checkbox->setChecked(showBox);
	checkbox->connect({"Checked", "Unchecked"}, [&showBox](bool value) { showBox = value; });

	resetButton->connect("pressed", [&]() { initialize(rootPath / "bouncingBall.txt", window, sim); });

	restitutionSlider->setValue(sim.restitutionCoeff * 100);
	restitutionSlider->connect("ValueChanged", [&restitutionCoeffLabel, &sim](float value) {
		restitutionCoeffLabel->setText(std::to_string(sim.restitutionCoeff = value / 100.0f));
	});

	frictionSlider->setValue(sim.frictionCoeff * 100);
	frictionSlider->connect("ValueChanged", [&frictionCoeffLabel, &sim](float value) {
		frictionCoeffLabel->setText(std::to_string(sim.frictionCoeff = value / 100.0f));
	});

	double time = 0;
	bool pauseSimulation = true;

	sf::Clock FPSClock;
	while (window.isOpen() && controllerWindow.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					window.close();
					controllerWindow.close();
					break;
				case sf::Event::KeyReleased: {
					if (event.key.code == sf::Keyboard::Escape) {
						window.close();
						controllerWindow.close();
					}
					else if (event.key.code == sf::Keyboard::A) {
						auto pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
						sim.addObject(new Particle({pos.x, pos.y}, {0.0, 0.0}, 1, 1));
					}
					else if (event.key.code == sf::Keyboard::Space) {
						pauseSimulation = !pauseSimulation;
					}
					else if (event.key.code == sf::Keyboard::P) {
						for (const auto &object : sim.objects) {
							switch (object->getClass()) {
								case BASESHAPE:
									break;
								case DYNAMICSHAPE:
									break;
								case RIGIDSHAPE:
									break;
								case LINE: {
									auto obj = static_cast<Line *>(object.get());
									printLn("LINE", obj->start.x, obj->start.y, obj->start.x, obj->end.y);
									break;
								}
								case PARTICLE: {
									auto obj = static_cast<Particle *>(object.get());
									printLn("PARTICLE", obj->pos.x, obj->pos.y);
									break;
								}
								case BALL: {
									// auto obj = static_cast<Ball *>(object.get());
									break;
								}
								case BOX: {
									// auto obj = static_cast<Box *>(object.get());
									break;
								}
							}
						}
					}

					break;
				}
				default:
					break;
			}
		}
		while (controllerWindow.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					window.close();
					controllerWindow.close();
					break;
				case sf::Event::KeyReleased: {
					if (event.key.code == sf::Keyboard::Escape) {
						window.close();
						controllerWindow.close();
					}
					break;
				}
				default:
					gui.handleEvent(event);
					break;
			}
		}

		if (!pauseSimulation) {
			auto timeLapse = std::min(FPSClock.restart(), sf::seconds(0.1));
			time += timeLapse.asSeconds();
			sim.simulate(timeLapse.asSeconds());
			timeLabel->setText("Time: " + std::to_string(time) + " s");
		}

		window.clear();
		controllerWindow.clear();
		{
			const auto view = window.getView();
			const double left = view.getCenter().x - view.getSize().x / 2, right = view.getCenter().x + view.getSize().x / 2;
			const double top = view.getCenter().y - view.getSize().y / 2, bottom = view.getCenter().y + view.getSize().y / 2;
			drawUtil.quad({Vector2D(left, top), Vector2D(right, top),
						   Vector2D(right, bottom), Vector2D(left, bottom)},
						  sf::Color::Red, 5);
		}

		for (const auto &object : sim.objects) {
			switch (object->getClass()) {
				case BASESHAPE:
					break;
				case DYNAMICSHAPE:
					break;
				case RIGIDSHAPE:
					break;
				case LINE: {
					auto obj = static_cast<Line *>(object.get());
					drawUtil.line(obj->start, obj->end, sf::Color::Green);
					drawUtil.line(0.5 * obj->start + 0.5 * obj->end, 0.5 * obj->start + 0.5 * obj->end + obj->normal, sf::Color::Magenta);
					break;
				}
				case PARTICLE: {
					auto obj = static_cast<Particle *>(object.get());
					drawUtil.drawCircle(obj->pos, obj->rad, sf::Color::Blue);
					if (showBox) {
						drawUtil.line(obj->pos, obj->pos + obj->vel, sf::Color::White);
					}
					break;
				}
				case BALL: {
					const auto obj = static_cast<Ball *>(object.get());
					drawUtil.drawCircle(obj->pos, obj->rad, sf::Color::Blue);
					auto radiusVec = Vector2D(std::cos(obj->angle), std::sin(obj->angle));
					drawUtil.line(obj->pos, obj->pos + obj->rad * radiusVec, sf::Color::Yellow);
					drawUtil.line(obj->pos + radiusVec, obj->pos + obj->rad * radiusVec + obj->angVel * radiusVec.rotate(1, 0), sf::Color::Yellow);
					break;
				}
				case BOX: {
					auto obj = static_cast<Box *>(object.get());
					drawUtil.quad(obj->corner, sf::Color::Blue);
					break;
				}
			}
			if (showBox)
				drawUtil.quad({Vector2D(object->left, object->top),
							   Vector2D(object->right, object->top),
							   Vector2D(object->right, object->bottom),
							   Vector2D(object->left, object->bottom)},
							  sf::Color::Red);
		}
		drawUtil.finally();
		gui.draw();
		window.display();
		controllerWindow.display();
	}
	window.close();
	controllerWindow.close();
	return 0;
}
