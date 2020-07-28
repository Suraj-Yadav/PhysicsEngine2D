#ifndef DRAW_UTIL_H
#define DRAW_UTIL_H

#include <PhysicsEngine2D/Vector2D.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <array>
#include <string>

class DrawUtil {
	sf::RenderTarget &window;
	sf::VertexArray quads, lines;
	sf::Font font;
	sf::Text text;
	sf::CircleShape circle;

   public:
	DrawUtil(sf::RenderTarget &window, const std::string &fontName);
	~DrawUtil();

	void line(
		const Vector2D &a,
		const Vector2D &b,
		const sf::Color &c1,
		const sf::Color &c2,
		int width = 0);
	void drawCircle(const Vector2D &cen, double rad, const sf::Color &c);
	void drawText(
		std::string str, const Vector2D &pos, int size, const sf::Color &c);
	void quad(
		const std::array<Vector2D, 4> &points,
		const sf::Color &col,
		int width = 0) {
		line(points[0], points[1], col, col, width);
		line(points[1], points[2], col, col, width);
		line(points[2], points[3], col, col, width);
		line(points[3], points[0], col, col, width);
	}
	void line(
		const Vector2D &a,
		const Vector2D &b,
		const sf::Color &c,
		int width = 0) {
		line(a, b, c, c, width);
	}
	void finally() {
		window.draw(lines);
		window.draw(quads);
		lines.clear();
		quads.clear();
	}
};

void drawGrid(sf::RenderTarget &window, bool change);

#endif	// DRAW_UTIL_H
