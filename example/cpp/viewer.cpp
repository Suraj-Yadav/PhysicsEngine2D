#define _USE_MATH_DEFINES
#include <GLFW/glfw3.h>	 // Will drag system OpenGL headers
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#ifdef __APPLE__
#include <gperftools/profiler.h>
#endif

#include <PhysicsEngine2D/Simulator.hpp>
#include <PhysicsEngine2D/util.hpp>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <set>

#include "drawUtil.hpp"

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* setupWindow(const ViewPort& view, const std::string& title) {
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) return nullptr;

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// 3.2+
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			// 3.0+ only

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(
		view.windowSize.x, view.windowSize.y, title.c_str(), NULL, NULL);
	if (window == NULL) return nullptr;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);  // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |=
		ImGuiConfigFlags_NavEnableKeyboard;	 // Enable Keyboard Controls
	io.ConfigFlags |=
		ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
	io.ConfigFlags |=
		ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform
										   // Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	return window;
}

int main(int argc, char** argv) {
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

	DrawUtil drawUtil(initFilePath, sim);

	bool showBox = false;
	bool pauseSimulation = false;
	double time = 0, lastTime = glfwGetTime();

	auto window = setupWindow(drawUtil.view, drawUtil.title);
	if (!window) {
		return -1;
	}

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (!pauseSimulation) {
			auto now = glfwGetTime();
			auto timeLapsed = std::min(now - lastTime, 0.1);
			time += timeLapsed;
			lastTime = now;
			sim.simulate(timeLapsed);
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text(
			"Time: %.2f secs, %.3f ms/frame (%.1f FPS)", time,
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		if (ImGui::Button("Reset")) {
			time = 0;
			drawUtil = DrawUtil(initFilePath, sim);
		}
		ImGui::Checkbox("Show Bounding Boxes", &showBox);
		ImGui::Checkbox("Pause Simulation", &pauseSimulation);
		ImGui::SliderFloat(
			"N Body Gravity", &sim.nBodyGravity, 0, 100, nullptr,
			ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Coefficient of Friction", &sim.frictionCoeff, 0, 1);
		ImGui::SliderFloat(
			"Coefficient of Restitution", &sim.restitutionCoeff, 0, 1);

		ImGui::End();

		auto dl = ImGui::GetBackgroundDrawList();
		for (auto& elem : sim.getBaseShapes()) {
			if (showBox)
				drawUtil.rect(
					dl, {elem.get().left, elem.get().top},
					{elem.get().right, elem.get().bottom},
					ImColor(1.0f, 0.0f, 0.0f));
		}

		for (auto& line : sim.getLines()) {
			drawUtil.line(dl, line.start, line.end, ImColor(0.0f, 1.0f, 0.0f));
			if (showBox) {
				drawUtil.line(
					dl, 0.5 * line.start + 0.5 * line.end,
					0.5 * line.start + 0.5 * line.end + line.normal,
					ImColor(0.0f, 1.0f, 0.0f));
			}
		}

		for (auto& particle : sim.getParticles()) {
			drawUtil.drawCircle(
				dl, particle.pos, particle.rad, ImColor(0.0f, 0.0f, 1.0f));
			if (showBox) {
				drawUtil.line(
					dl, particle.pos, particle.pos + particle.vel,
					ImColor(1.0f, 1.0f, 1.0f));
			}
		}
		for (auto& ball : sim.getBalls()) {
			drawUtil.drawCircle(
				dl, ball.pos, ball.rad, ImColor(0.0f, 0.0f, 1.0f));
			auto radiusVec =
				Vector2D(std::cos(ball.angle), std::sin(ball.angle));
			drawUtil.line(
				dl, ball.pos, ball.pos + ball.rad * radiusVec,
				ImColor(1.0f, 1.0f, 0.0f));
			drawUtil.line(
				dl, ball.pos + radiusVec,
				ball.pos + ball.rad * radiusVec +
					ball.angVel * radiusVec.rotate(1, 0),
				ImColor(1.0f, 1.0f, 0.0f));
		}
		// Rendering
		drawUtil.finally(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
