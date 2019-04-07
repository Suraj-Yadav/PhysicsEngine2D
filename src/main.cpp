#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>

#include <TGUI/TGUI.hpp>

#include "Simulator.hpp"
#include "drawUtil.hpp"

// clang-format off
using std::cout;
using std::cin;
#define newline '\n'
inline void print() {}
template <typename T> void print(T t) { cout<<t; }
template <typename T, typename... Args> void print(T t, Args... args) { cout << t << " "; print(args...); }
template <typename... Args> void println(Args... args) { print(args...); cout<<newline; }
inline void printF(const char *&format) { cout << format; }
template <typename T, typename... Args> void printF(const char *format, T t, Args... args) {
while (*format != '%' && *format) {cout.put(*format++);} if (*format++ == '\0') {return;} cout << t, printF( format, args...);}
template <typename T> inline void printC(T t) { for (auto &elem : t) print(elem, ""); println(); }
#define NORMAL_IO_SPEEDUP std::ios_base::sync_with_stdio(false),std::cin.tie(NULL);
// clang-format on

// Vector2D gravity(const DynamicShape &a, const ForceField &f) {
// 	return 6.67408e-11 * unit(f.pos - a.pos) * a.mass / lenSq(a.pos - f.pos);
// }

void initialize(const std::string filePath, sf::RenderWindow &window, tgui::Gui &gui, Simulator &sim) {
	sim.clear();
	std::ifstream file(filePath);
	std::string line;
	std::string type;
	for (size_t lineNumber = 1; std::getline(file, line); lineNumber++) {
		std::istringstream iss(line);
		iss >> type;
		try {
			if (type.front() == '#') {
			}
			else if (type == "SIZE") {
				const unsigned extraWidth = 250;
				unsigned W, H;
				double left, top, right, bottom;
				if (!(iss >> W >> H >> left >> top >> right >> bottom)) {
					throw std::invalid_argument("Invalid 'SIZE' input");
				}
				window.setSize({W + extraWidth, H});
				sf::View view(sf::FloatRect(left, top, right - left, bottom - top));
				view.setViewport({0.0f, 0.0f, W / float(W + extraWidth), 1.0f});
				window.setView(view);
				view.setCenter(extraWidth / 2, H / 2);
				view.setSize(extraWidth, H);
				view.setViewport({W / float(W + extraWidth), 0.0f, extraWidth / float(W + extraWidth), 1.0f});
				gui.setView(view);
			}
			else if (type == "TITLE") {
				std::string title;
				if (!(iss >> title)) {
					throw std::invalid_argument("Invalid 'TITLE' input");
				}
				window.setTitle(title);
			}
			else if (type == "LINE") {
				Vector2D a, b, p;
				if (!(iss >> a.x >> a.y >> b.x >> b.y >> p.x >> p.y)) {
					throw std::invalid_argument("Invalid 'LINE' input");
				}
				sim.addObject(new Line(a, b, p));
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
			else if (type == "GRAVITY") {
				sim.addForceField(ForceField([](const DynamicShape &a, const ForceField &f) {
					return Vector2D(0.0f, -9.8f) * a.mass;
				}));
			}
			else if (type == "END") {
				return;
			}
		}
		catch (const std::exception &e) {
			printF("%:%: %", filePath, lineNumber, e.what());
			throw;
		}
	}
}

int main() {
	// NORMAL_IO_SPEEDUP;

	Simulator sim(10, 0.9f);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(800, 800), "2131321", sf::Style::Close, settings);
	tgui::Gui gui(window);
	initialize("G:\\work\\PhysicsEngine2D\\resources\\init.txt", window, gui, sim);

	bool showBox = false;

	gui.loadWidgetsFromFile("G:\\work\\PhysicsEngine2D\\resources\\form.txt");

	auto resetButton = gui.get<tgui::Button>("resetButton");
	auto checkbox = gui.get<tgui::CheckBox>("checkbox");
	auto slider = gui.get<tgui::Slider>("slider");
	auto coeffLabel = gui.get<tgui::Label>("coeffLabel");
	auto timeLabel = gui.get<tgui::Label>("timeLabel");

	checkbox->setChecked(showBox);
	checkbox->connect({"Checked", "Unchecked"}, [&showBox](bool value) { showBox = value; });

	resetButton->connect("pressed", [&]() { initialize("G:\\work\\PhysicsEngine2D\\resources\\init.txt", window, gui, sim); });

	slider->setValue(sim.restitutionCoeff * 100);
	slider->connect("ValueChanged", [&coeffLabel, &sim](float value) {
		coeffLabel->setText(std::to_string(sim.restitutionCoeff = value / 100.0f));
	});

	double time = 0;
	bool pauseSimulation = true;

	sf::Clock FPSClock;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyReleased: {
					if (event.key.code == sf::Keyboard::Escape)
						window.close();
					if (event.key.code == sf::Keyboard::A) {
						auto pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
						sim.addObject(new Particle({pos.x, pos.y}, {0.0, 0.0}, 1, 1));
					}
					if (event.key.code == sf::Keyboard::Space) {
						pauseSimulation = !pauseSimulation;
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
		{
			const auto view = window.getView();
			const double left = view.getCenter().x - view.getSize().x / 2, right = view.getCenter().x + view.getSize().x / 2;
			const double top = view.getCenter().y - view.getSize().y / 2, bottom = view.getCenter().y + view.getSize().y / 2;
			AABB(left, bottom, right, top, sf::Color::Red, window);
			// for (int i = -50; i < 50; i += 5) {
			// 	line({i, -100}, {i, 100}, sf::Color::Yellow, window);
			// 	line({-100, i}, {100, i}, sf::Color::Yellow, window);
			// }
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
					line(obj->start, obj->end, sf::Color::Green, window);
					break;
				}
				case PARTICLE: {
					auto obj = static_cast<Particle *>(object.get());
					circle(obj->pos, obj->rad, sf::Color::Blue, window);
					if (showBox) {
						line(obj->pos, obj->pos + obj->vel, sf::Color::White, window);
					}
					break;
				}
				case BALL: {
					auto obj = static_cast<Ball *>(object.get());
					circle(obj->pos, obj->rad, sf::Color::Blue, window);
					line(obj->pos,
						 obj->pos +
							 obj->rad *
								 Vector2D(std::cos(obj->angle), std::sin(obj->angle)),
						 sf::Color::Yellow, window);
					break;
				}
				case BOX: {
					auto obj = static_cast<Box *>(object.get());
					quad(obj->corner, sf::Color::Blue, window);
					break;
				}
			}
			if (showBox)
				AABB(object->left, object->bottom, object->right, object->top,
					 sf::Color::Red, window);
		}
		gui.draw();
		window.display();
	}
	window.close();
	return 0;
}
