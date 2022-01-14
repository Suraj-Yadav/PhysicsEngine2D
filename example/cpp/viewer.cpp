#define _USE_MATH_DEFINES

#ifdef __APPLE__
#include <gperftools/profiler.h>
#endif

#include <PhysicsEngine2D/Simulator.hpp>
#include <PhysicsEngine2D/util.hpp>
#include <TGUI/TGUI.hpp>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <set>

#include "drawUtil.hpp"

Vector2D gravity(const DynamicShape &a, const ForceField &f) {
	return 6.67408e-11 * (f.pos - a.pos).unit() * a.mass /
		   (a.pos - f.pos).lenSq();
}

void initialize(
	const std::filesystem::path filePath, sf::RenderWindow &window,
	sf::RenderWindow &controllerWindow, tgui::Gui &gui, Simulator &sim) {
	std::ifstream file(filePath.string());
	std::string line;
	std::string type;

	const float scale =
		std::max(sf::VideoMode::getDesktopMode().width / 1920.0, 1.0);

	controllerWindow.setSize({unsigned(300 * scale), unsigned(300 * scale)});
	controllerWindow.setPosition({200, 200});
	controllerWindow.setView(sf::View({0, 0, 600, 600}));
	gui.setView(controllerWindow.getView());

	window.setPosition({static_cast<int>(200 + 300 * scale), 200});

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
				sf::View view(
					sf::FloatRect(left, top, right - left, bottom - top));
				view.setViewport({0.0f, 0.0f, 1.0f, 1.0f});
				window.setView(view);
			}
			else if (type == "TITLE") {
				std::string title;
				if (!(std::getline(iss, title))) {
					throw std::invalid_argument("Invalid 'TITLE' input");
				}
				window.setTitle(title);
			}
			else if (type == "END") {
				break;
			}
		}
		catch (const std::exception &e) {
			print_exception(e);
			throw;
		}
	}
	initialize(filePath, sim);
}

int main(int argc, char **argv) {
	NORMAL_IO_SPEEDUP;

	std::vector<std::string> args;
	for (int i = 0; i < argc; i++) {
		args.emplace_back(argv[i]);
	}

	const auto initFilePath =
		std::filesystem::absolute(std::filesystem::path(args[1]));

	std::filesystem::path rootPath(
		std::filesystem::absolute(std::filesystem::path(argv[0]))
			.parent_path());

	Simulator sim(10, 0.9f, 0.9f);

	sf::RenderWindow controllerWindow(
		sf::VideoMode(400, 400), "Controls", sf::Style::Close,
		sf::ContextSettings(0, 0, 8));

	sf::RenderWindow window(
		sf::VideoMode(800, 800), "Drawing Area", sf::Style::Close,
		sf::ContextSettings(0, 0, 8));

	tgui::Gui gui(controllerWindow);

	initialize(initFilePath, window, controllerWindow, gui, sim);

	DrawUtil drawUtil(window, "2Dumb");

	bool showBox = false;

	gui.loadWidgetsFromFile((rootPath / "controller.form").string());

	auto resetButton = gui.get<tgui::Button>("resetButton");
	auto checkbox = gui.get<tgui::CheckBox>("checkbox");
	auto restitutionSlider = gui.get<tgui::Slider>("restitutionSlider");
	auto restitutionCoeffLabel = gui.get<tgui::Label>("restitutionCoeffLabel");
	auto frictionSlider = gui.get<tgui::Slider>("frictionSlider");
	auto frictionCoeffLabel = gui.get<tgui::Label>("frictionCoeffLabel");
	auto gravitySlider = gui.get<tgui::Slider>("gravitySlider");
	auto gravityLabel = gui.get<tgui::Label>("gravityLabel");
	auto timeLabel = gui.get<tgui::Label>("timeLabel");

	checkbox->setChecked(showBox);
	checkbox->connect(
		{"Checked", "Unchecked"}, [&showBox](bool value) { showBox = value; });

	restitutionSlider->setValue(sim.restitutionCoeff * 100);
	restitutionCoeffLabel->setText(std::to_string(sim.restitutionCoeff));
	restitutionSlider->connect(
		"ValueChanged", [&restitutionCoeffLabel, &sim](float value) {
			restitutionCoeffLabel->setText(
				std::to_string(sim.restitutionCoeff = value / 100.0f));
		});

	frictionSlider->setValue(sim.frictionCoeff * 100);
	frictionCoeffLabel->setText(std::to_string(sim.frictionCoeff));
	frictionSlider->connect(
		"ValueChanged", [&frictionCoeffLabel, &sim](float value) {
			frictionCoeffLabel->setText(
				std::to_string(sim.frictionCoeff = value / 100.0f));
		});

	gravitySlider->setValue(sim.nBodyGravity * 100.0f);
	gravityLabel->setText(std::to_string(sim.nBodyGravity));
	gravitySlider->connect("ValueChanged", [&gravityLabel, &sim](float value) {
		gravityLabel->setText(
			std::to_string(sim.nBodyGravity = value / 100.0f));
	});

	double time = 0;
	bool pauseSimulation = true;

	resetButton->connect("pressed", [&]() {
		initialize(initFilePath, window, controllerWindow, gui, sim);
		time = 0;
	});

#ifdef __APPLE__
	ProfilerStart("output.pprof");
#endif

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
					switch (event.key.code) {
						case sf::Keyboard::Escape:
							window.close(), controllerWindow.close();
							break;
						case sf::Keyboard::A: {
							auto pos = window.mapPixelToCoords(
								sf::Mouse::getPosition(window));
							sim.addParticle(
								Particle({pos.x, pos.y}, {0.0, 0.0}, 1, 1));
							break;
						}
						case sf::Keyboard::Space:
							pauseSimulation = !pauseSimulation;
							break;
						case sf::Keyboard::R:
							initialize(
								initFilePath, window, controllerWindow, gui,
								sim);
							time = 0;
							break;
						case sf::Keyboard::P:
							for (const auto &elem : sim.getLines()) {
								printLn("LINE", elem.start, elem.end);
							}
							for (const auto &elem : sim.getParticles()) {
								printLn("PARTICLE", elem.pos);
							}
							break;
						default:
							break;
					}
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
			const double left = view.getCenter().x - view.getSize().x / 2,
						 right = view.getCenter().x + view.getSize().x / 2;
			const double top = view.getCenter().y - view.getSize().y / 2,
						 bottom = view.getCenter().y + view.getSize().y / 2;
			drawUtil.quad(
				{Vector2D(left, top), Vector2D(right, top),
				 Vector2D(right, bottom), Vector2D(left, bottom)},
				sf::Color::Red, 5);
		}
		for (auto &elem : sim.getBaseShapes()) {
			if (showBox)
				drawUtil.quad(
					{Vector2D(elem.get().left, elem.get().top),
					 Vector2D(elem.get().right, elem.get().top),
					 Vector2D(elem.get().right, elem.get().bottom),
					 Vector2D(elem.get().left, elem.get().bottom)},
					sf::Color::Red);
		}

		for (auto &line : sim.getLines()) {
			drawUtil.line(line.start, line.end, sf::Color::Green);
			if (showBox) {
				drawUtil.line(
					0.5 * line.start + 0.5 * line.end,
					0.5 * line.start + 0.5 * line.end + line.normal,
					sf::Color::Green);
			}
		}
		for (auto &particle : sim.getParticles()) {
			drawUtil.drawCircle(particle.pos, particle.rad, sf::Color::Blue);
			if (showBox) {
				drawUtil.line(
					particle.pos, particle.pos + particle.vel,
					sf::Color::White);
			}
		}
		for (auto &ball : sim.getBalls()) {
			drawUtil.drawCircle(ball.pos, ball.rad, sf::Color::Blue);
			auto radiusVec =
				Vector2D(std::cos(ball.angle), std::sin(ball.angle));
			drawUtil.line(
				ball.pos, ball.pos + ball.rad * radiusVec, sf::Color::Yellow);
			drawUtil.line(
				ball.pos + radiusVec,
				ball.pos + ball.rad * radiusVec +
					ball.angVel * radiusVec.rotate(1, 0),
				sf::Color::Yellow);
		}

		drawUtil.finally();
		gui.draw();
		window.display();
		controllerWindow.display();
	}
#ifdef __APPLE__
	ProfilerStop();
#endif

	window.close();
	controllerWindow.close();
	return 0;
}
