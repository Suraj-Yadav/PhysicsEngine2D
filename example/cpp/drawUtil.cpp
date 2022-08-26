#include "drawUtil.hpp"

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <PhysicsEngine2D/Simulator.hpp>
#include <PhysicsEngine2D/util.hpp>
#include <fstream>
#include <random>
#include <sstream>

#include "fontManager.hpp"

DrawUtil::DrawUtil(const std::filesystem::path filePath, Simulator& sim) {
	title = "Viewer:" + filePath.filename().string();
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
			if (type == "SIZE") {
				unsigned W, H;
				double left, top, right, bottom;
				if (!(iss >> W >> H >> left >> top >> right >> bottom)) {
					throw std::invalid_argument("Invalid 'SIZE' input");
				}
				view.windowSize = ImVec2(W, H);
				view.position = Vector2D(left, top);
				view.scale = ImVec2(W / (right - left), H / (bottom - top));
			}
			else if (type == "TITLE") {
				std::getline(iss, title);
			}
			else if (type == "LINE") {
				Vector2D a, b;
				if (!(iss >> a.x >> a.y >> b.x >> b.y)) {
					throw std::invalid_argument("Invalid 'LINE' input");
				}
				sim.addLine(a, b);
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
				sim.addParticle(position, velocity, mass, radius);
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
				sim.addBall(
					position, velocity, mass, radius, angle, angularVelocity);
			}
			else if (type == "GRAVITY") {
				dataType x, y;
				if (!(iss >> x >> y)) {
					throw std::invalid_argument("Invalid 'GRAVITY' input");
				}
				sim.addForceField(ForceField(
					[x, y](const DynamicShape& a, const ForceField&) {
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
						sim.addParticle(
							Vector2D(x(gen), y(gen)), Vector2D(), mass(gen),
							rad(gen));
					}
				}
			}
			else if (type == "END") {
				break;
			}
		}
		catch (const std::exception& e) {
			print_exception(e);
			throw;
		}
	}
}

DrawUtil::~DrawUtil() {}

void DrawUtil::line(
	ImDrawList* draw_list, const Vector2D& a, const Vector2D& b, const Color& c,
	int width) {
	draw_list->AddLine(view.abs(a), view.abs(b), c, width);
}

void DrawUtil::rect(
	ImDrawList* draw_list, const Vector2D& a, const Vector2D& b,
	const Color& col, int width) {
	draw_list->AddRect(view.abs(a), view.abs(b), col, 0, 0, width);
}

void DrawUtil::drawCircle(
	ImDrawList* draw_list, const Vector2D& cen, double rad, const Color& c) {
	draw_list->AddCircleFilled(view.abs(cen), view.abs(rad), c);
}

void DrawUtil::drawText(
	ImDrawList* draw_list, std::string str, const Vector2D& pos, int size,
	const Color& c) {
	draw_list->AddText(view.abs(pos), c, str.c_str(), str.c_str() + str.size());
}

void DrawUtil::finally(GLFWwindow* window) {
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we
	// save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call
	//  glfwMakeContextCurrent(window) directly)
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}

	glfwSwapBuffers(window);
}

void print_exception(const std::exception& e, int level) {
	std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
	try {
		std::rethrow_if_nested(e);
	}
	catch (const std::exception& e) {
		print_exception(e, level + 1);
	}
	catch (...) {
	}
}

ImVec2 ViewPort::abs(Vector2D v) {
	v -= position;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const ImVec2 origin = viewport->Pos;
	return ImVec2(v.x * scale.x + origin.x, v.y * scale.y + origin.y);
}
Vector2D ViewPort::rel(const ImVec2&) { return Vector2D(); }
dataType ViewPort::abs(dataType v) { return v * scale.x; }
dataType ViewPort::rel(dataType v) { return v / scale.x; }
