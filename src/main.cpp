#define _USE_MATH_DEFINES
#include <cmath>
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

Vector2D gravity(const DynamicShape &a, const ForceField &f) {
	return 100 * unit(f.pos - a.pos) * a.mass / lenSq(a.pos - f.pos);
}

int main() {
	NORMAL_IO_SPEEDUP;

	auto sim = Simulator(10, 0.9f);

	const float BOTTOM = -300, LEFT = -400, RIGHT = 400, TOP = 300;
	const float PAD = 10;
	const int W = 1000, H = 800;

	sim.addObject(new Line({-400, -200}, {400, -200}, {0, 0}));
	sim.addObject(new Line({-400, -200}, {-400, 300}, {0, 0}));
	sim.addObject(new Line({400, -200}, {400, 300}, {0, 0}));
	sim.addObject(new Line({-400, 300}, {400, 300}, {0, 0}));

	std::set<std::pair<int, int>> gravPairs;

	for (int j = -300; j < -290; j += 5) {
		for (int i = -180; i < 280; i += 5) {
			gravPairs.insert({sim.objects.size(), sim.forceFields.size()});
			sim.addObject(new Particle({j, i}, {0.0, 0.0}, 1, 2));
			sim.addForceField(ForceField(gravity, {j, i}));
		}
	}

	for (int j = -300; j < -295; j += 5) {
		for (int i = -180; i < 280; i += 5) {
			gravPairs.insert({sim.objects.size(), sim.forceFields.size()});
			sim.addObject(new Particle({-j, i}, {0.0, 0.0}, 1, 2));
			sim.addForceField(ForceField(gravity, {-j, i}));
		}
	}

	// sim.addForceField(ForceField([](const DynamicShape &a, const ForceField &f) {
	// 	return Vector2D(0.0f, -9.8f) * a.mass;
	// }));

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(W, H), std::string(__FILE__).find_last_of('\\') + __FILE__ + 1, sf::Style::Close, settings);
	window.setPosition({0, 0});
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f((RIGHT - LEFT + 2 * PAD),
												   (BOTTOM - TOP - 2 * PAD)));
	window.setView(view);

	bool showBox = false;

	tgui::Gui gui{window};

	gui.loadWidgetsFromFile("G:\\work\\PhysicsEngine2D\\resources\\form.txt");

	auto resetButton = gui.get<tgui::Button>("resetButton");
	auto checkbox = gui.get<tgui::CheckBox>("checkbox");
	auto slider = gui.get<tgui::Slider>("slider");
	auto coeffLabel = gui.get<tgui::Label>("coeffLabel");

	if (showBox) checkbox->check();

	slider->setValue(sim.restitutionCoeff * 100);
	slider->connect("ValueChanged", [&coeffLabel, &sim](float value) {
		coeffLabel->setText(std::to_string(sim.restitutionCoeff = value / 100.0f));
	});
	checkbox->connect({"Checked", "Unchecked"}, [&showBox](bool value) { showBox = value; });

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
						gravPairs.insert({sim.objects.size(), sim.forceFields.size()});
						sim.addObject(new Particle({pos.x, pos.y}, {0.0, 0.0}, 1, 2));
						sim.addForceField(ForceField(gravity, {pos.x, pos.y}));
					}
					break;
				}
				default:
					gui.handleEvent(event);
			}
		}

		auto timeLapse = std::min(FPSClock.restart(), sf::seconds(0.1));
		for (auto &[i, j] : gravPairs) {
			sim.forceFields[j].pos = static_cast<DynamicShape *>(sim.objects[i].get())->pos;
		}
		sim.simulate(timeLapse.asSeconds(), gravPairs);

		window.clear();
		// AABB(LEFT, BOTTOM, RIGHT, TOP, sf::Color::White, window);

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
						line({0, 0}, obj->pos, sf::Color::Green, window);
					}
					// energy += obj->mass * lenSq(obj->vel) / 2.0 - obj->mass * gravity.y *
					// obj->pos.y;
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
					if (showBox) {
						// line(obj->pos, obj->pos + obj->vel * delta, sf::Color::White,
						// window);
						line({0, 0}, obj->pos, sf::Color::Green, window);
					}
					// energy += obj->mass * lenSq(obj->vel) / 2.0 - obj->mass * gravity.y *
					// obj->pos.y;
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
		// window.capture().saveToFile("screenShot.png");
	}
	window.close();
	gui.removeAllWidgets();
	return 0;
}
