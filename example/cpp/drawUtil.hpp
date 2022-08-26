#ifndef DRAW_UTIL_H
#define DRAW_UTIL_H

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <PhysicsEngine2D/Simulator.hpp>
#include <PhysicsEngine2D/Vector2D.hpp>
#include <array>
#include <filesystem>
#include <string>

struct ViewPort {
	ImVec2 windowSize;
	Vector2D position;
	ImVec2 scale;

   public:
	ImVec2 abs(Vector2D);
	Vector2D rel(const ImVec2&);
	dataType abs(dataType);
	dataType rel(dataType);
};

void print_exception(const std::exception& e, int level = 0);

using Color = ImColor;

class DrawUtil {
   public:
	ViewPort view;
	GLFWwindow* window;
	std::string title;

	DrawUtil(const std::filesystem::path filePath, Simulator& sim);
	~DrawUtil();

	void drawCircle(
		ImDrawList* draw_list, const Vector2D& cen, double rad, const Color& c);
	void drawText(
		ImDrawList* draw_list, std::string str, const Vector2D& pos, int size,
		const Color& c);
	void rect(
		ImDrawList* draw_list, const Vector2D& a, const Vector2D& b,
		const Color& col, int width = 1);
	void line(
		ImDrawList* draw_list, const Vector2D& a, const Vector2D& b,
		const Color& c, int width = 1);
	void finally(GLFWwindow* window);
};

#endif	// DRAW_UTIL_H
