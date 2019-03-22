
#include <cmath>
#include <functional>
#include <random>
#include <vector>

#include <TGUI/TGUI.hpp>

#include "IntervalTree.hpp"
#include "Shapes.hpp"
#include "drawUtil.hpp"
#include "util.hpp"

class ForceField {
	std::function<Vector2D(const DynamicShape &, const ForceField &)> func;

   public:
	Vector2D pos;
	ForceField(const std::function<Vector2D(const DynamicShape &,
											const ForceField &)> &f,
			   const Vector2D &p = Vector2D())
		: func(f), pos(p) {}
	Vector2D getForce(const DynamicShape &obj) { return func(obj, *this); }
};

// ForceField gravity([](const DynamicShape &a, const ForceField &f) { return
// Vector2D(5.8, -9.8) * a.mass; });
ForceField gravity(
	[](const DynamicShape &a, const ForceField &f) {
		return -10000 * unit(a.pos) * a.mass / lenSq(a.pos - f.pos);
	});

float restitutionCoeff = 0.9f;

std::vector<std::unique_ptr<BaseShape>> objects;

class Simulator {
	std::vector<std::shared_ptr<BaseShape>> objects;

   public:
	template <typename T, typename... Args>
	std::shared_ptr<T> addShape(Args... args) {
		objects.push_back(std::make_shared<BaseShape>(new T(std::forward<Args>(args)...)));
		return objects.back();
	}
};

// void initialization() {
// 	std::random_device rd;
// 	std::mt19937 gen(rd());
// 	gen.seed(time(nullptr));
// 	std::uniform_real_distribution<> posX(LEFT, RIGHT), posY(0, TOP);

// 	// objects.clear();

// 	// int count = 1;
// 	// objects.resize(count);
// 	// for (int i = 0; i < count; ++i) objects[i] = std::unique_ptr<Shape>(new
// 	// Ball({posX(gen), posY(gen)}, {0, 0}, 1, 1));
// 	objects.push_back(
// 		std::unique_ptr<BaseShape>(new Ball({-10, 0}, {0, 0}, 1.0, 1.0, M_PI_4)));
// 	// objects.push_back(
// 	// 	std::unique_ptr<BaseShape>(new Ball({10, 0}, {0, 0}, 100, 1.0, -M_PI_4)));
// 	objects.push_back(
// 		std::unique_ptr<BaseShape>(new Box({-30, 30}, {0, 0}, 1, 4, 2, -M_PI_4)));

// 	// auto p = static_cast<Box *>(objects.back().get());
// 	// println(str(p->pos), str(p->vel), p->mass, p->invMass, p->inertia,
// 	// p->invInertia, p->angle, p->angVel, p->w, p->h);
// 	// for(auto elem:p->corner)println(str(elem));

// 	objects.push_back(
// 		std::unique_ptr<BaseShape>(new Line({-5, -5}, {5, -5}, {0, -10})));
// 	objects.push_back(std::unique_ptr<BaseShape>(new Line({-5, 5}, {5, 5}, {0, 10})));
// 	objects.push_back(
// 		std::unique_ptr<BaseShape>(new Line({-5, -5}, {-5, 5}, {-10, 0})));
// 	objects.push_back(std::unique_ptr<BaseShape>(new Line({5, -5}, {5, 5}, {10, 0})));

// int collisionSim(unsigned subStep) {
// 	std::ios_base::sync_with_stdio(false);
// 	std::cin.tie(NULL);
// 	sf::ContextSettings settings;
// 	settings.antialiasingLevel = 8;
// 	sf::RenderWindow window(sf::VideoMode(W, H), std::string(__FILE__).find_last_of('\\') + __FILE__ + 1, sf::Style::Close, settings);
// 	settings = window.getSettings();
// 	println(settings.majorVersion, ":", settings.minorVersion);
// 	window.setPosition({0, 0});
// 	sf::View view(sf::Vector2f(0, 0), sf::Vector2f((RIGHT - LEFT + 2 * PAD),
// 												   (BOTTOM - TOP - 2 * PAD)));
// 	window.setView(view);

// 	initialization();

// 	bool showBox = false;

// 	tgui::Gui gui{window};  // Create the gui and attach it to the window

// 	gui.loadWidgetsFromFile("G:\\work\\PhysicsEngine2D\\resources\\form.txt");

// 	auto resetButton = gui.get<tgui::Button>("resetButton");
// 	auto checkbox = gui.get<tgui::CheckBox>("checkbox");
// 	auto slider = gui.get<tgui::Slider>("slider");
// 	auto coeffLabel = gui.get<tgui::Label>("coeffLabel");

// 	resetButton->connect("pressed", initialization);
// 	if (showBox) checkbox->check();
// 	slider->setValue(restitutionCoeff * 100);
// 	slider->connect("ValueChanged", [&coeffLabel](float value) {
// 		coeffLabel->setText(std::to_string(restitutionCoeff = value / 100.0f));
// 	});
// 	checkbox->connect({"Checked", "Unchecked"}, [&](bool value) { showBox = value; });

// 	sf::Clock FPSClock;
// 	for (int i = 0; /* i < 1000 && */ window.isOpen(); ++i) {
// 		sf::Event event;
// 		while (window.pollEvent(event)) {
// 			switch (event.type) {
// 				case sf::Event::Closed:
// 					window.close();
// 					break;
// 				case sf::Event::KeyReleased: {
// 					if (event.key.code == sf::Keyboard::Escape)
// 						window.close();
// 					if (event.key.code == sf::Keyboard::A) {
// 						auto pos = window.mapPixelToCoords(sf::Mouse::getPosition());
// 						objects.push_back(std::unique_ptr<BaseShape>(
// 							new Particle({pos.x, pos.y}, {0, 0}, 1, 1)));
// 					}
// 					break;
// 				}
// 				default:
// 					gui.handleEvent(event);
// 			}
// 		}

// 		auto timeLapse = std::min(FPSClock.restart(), sf::seconds(0.1));
// 		float delta = timeLapse.asSeconds() / subStep;
// 		// float fps = 1000000.0f / timeLapse.asMicroseconds();

// 		//
// 		//	Physics Part
// 		//
// 		for (unsigned step = 0; step < subStep; ++step) {
// 			for (auto &object : objects) {
// 				switch (object->getClass()) {
// 					case PARTICLE: {
// 						auto obj = static_cast<Particle *>(object.get());
// 						// print(str(obj->vel));
// 						obj->applyImpulse(gravity.getForce(*obj) * delta, obj->pos);
// 						// println(str(obj->vel));
// 						obj->move(delta);
// 						break;
// 					}
// 					case BALL: {
// 						auto obj = static_cast<Ball *>(object.get());
// 						obj->applyImpulse(gravity.getForce(*obj) * delta, obj->pos);
// 						obj->move(delta);
// 						break;
// 					}
// 					case BOX: {
// 						auto obj = static_cast<Box *>(object.get());
// 						obj->applyImpulse(gravity.getForce(*obj) * delta, obj->pos);
// 						obj->move(delta);
// 						break;
// 					}
// 					// case BOX: {
// 					// 	auto obj = static_cast<Box *>(object.get());
// 					// 	// obj->applyImpulse(gravity * delta * obj->mass);
// 					// 	obj->move(delta);
// 					// 	break;
// 					// }
// 					case BASESHAPE:
// 					case DYNAMICSHAPE:
// 					case LINE:
// 					case RIGIDSHAPE:
// 						break;
// 				}
// 			}
// 			auto possibleCollisions = getCollisions(objects);
// 			for (auto &p : possibleCollisions) {
// 				int i = p.first, j = p.second;
// 				Type first = objects[i]->getClass(), second = objects[j]->getClass();
// 				if (first == LINE && second == PARTICLE)
// 					manageCollision(*static_cast<Particle *>(objects[j].get()),
// 									*static_cast<Line *>(objects[i].get()), delta);
// 				if (first == LINE && second == BALL)
// 					manageCollision(*static_cast<Ball *>(objects[j].get()),
// 									*static_cast<Line *>(objects[i].get()), delta);
// 				else if (second == LINE && first == BALL)
// 					manageCollision(*static_cast<Ball *>(objects[i].get()),
// 									*static_cast<Line *>(objects[j].get()), delta);
// 				else if (second == LINE && first == PARTICLE)
// 					manageCollision(*static_cast<Particle *>(objects[i].get()),
// 									*static_cast<Line *>(objects[j].get()), delta);
// 				else if (first == PARTICLE && second == PARTICLE)
// 					manageCollision(*static_cast<Particle *>(objects[j].get()),
// 									*static_cast<Particle *>(objects[i].get()), delta);
// 			}
// 		}

// 		window.clear(sf::Color::Black);
// 		AABB(LEFT, BOTTOM, RIGHT, TOP, sf::Color::White, window);
// 		double energy = 0.0;
// 		for (auto &object : objects) {
// 			switch (object->getClass()) {
// 				case BASESHAPE:
// 					break;
// 				case DYNAMICSHAPE:
// 					break;
// 				case RIGIDSHAPE:
// 					break;
// 				case LINE: {
// 					auto obj = static_cast<Line *>(object.get());
// 					line(obj->start, obj->end, sf::Color::Green, window);
// 					break;
// 				}
// 				case PARTICLE: {
// 					auto obj = static_cast<Particle *>(object.get());
// 					circle(obj->pos, obj->rad, sf::Color::Blue, window);
// 					if (showBox) {
// 						line(obj->pos, obj->pos + obj->vel * delta, sf::Color::White, window);
// 						line({0, 0}, obj->pos, sf::Color::Green, window);
// 					}
// 					// energy += obj->mass * lenSq(obj->vel) / 2.0 - obj->mass * gravity.y *
// 					// obj->pos.y;
// 					break;
// 				}
// 				case BALL: {
// 					auto obj = static_cast<Ball *>(object.get());
// 					circle(obj->pos, obj->rad, sf::Color::Blue, window);
// 					line(obj->pos,
// 						 obj->pos +
// 							 obj->rad *
// 								 Vector2D(std::cos(obj->angle), std::sin(obj->angle)),
// 						 sf::Color::Yellow, window);
// 					if (showBox) {
// 						// line(obj->pos, obj->pos + obj->vel * delta, sf::Color::White,
// 						// window);
// 						line({0, 0}, obj->pos, sf::Color::Green, window);
// 					}
// 					// energy += obj->mass * lenSq(obj->vel) / 2.0 - obj->mass * gravity.y *
// 					// obj->pos.y;
// 					break;
// 				}
// 				case BOX: {
// 					auto obj = static_cast<Box *>(object.get());
// 					quad(obj->corner, sf::Color::Blue, window);
// 					break;
// 				}
// 			}
// 			if (showBox)
// 				AABB(object->left, object->bottom, object->right, object->top,
// 					 sf::Color::Red, window);
// 		}
// 		// label->setText(str(energy));
// 		// coeffLabel->setText(str(slider->getValue() / 100.0));
// 		// drawGrid(window, true);
// 		gui.draw();
// 		window.display();
// 		// window.capture().saveToFile("image.png");
// 	}
// 	window.close();
// 	gui.removeAllWidgets();
// 	return 0;
// }
