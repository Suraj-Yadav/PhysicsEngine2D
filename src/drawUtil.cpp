#include "drawUtil.hpp"

// #define CATCH_CONFIG_MAIN

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <sstream>

template <typename T>
std::string to_string_stream(const T &n) {
	std::ostringstream stm;
	stm << n;
	return stm.str();
}

inline sf::Color invert(const sf::Color &c) { return sf::Color(((255 - c.r) << 24) + ((255 - c.g) << 16) + ((255 - c.b) << 8) + 255); }

void AABB(double left, double bottom, double right, double top, const sf::Color &col, sf::RenderTarget &window) {
	static sf::VertexArray lines(sf::LinesStrip, 5);
	lines[0].position.x = lines[1].position.x = left;
	lines[2].position.x = lines[3].position.x = right;

	lines[0].position.y = lines[3].position.y = bottom;
	lines[1].position.y = lines[2].position.y = top;

	lines[4].position = lines[0].position;

	lines[0].color = lines[1].color = lines[2].color = lines[3].color = lines[4].color = col;

	window.draw(lines);
}

void quad(const std::array<Vector2D, 4> &pts,
		  const sf::Color &col,
		  sf::RenderTarget &window) {
	static sf::ConvexShape q(4);
	q.setPoint(0, {(float)pts[0].x, (float)pts[0].y});
	q.setPoint(1, {(float)pts[1].x, (float)pts[1].y});
	q.setPoint(2, {(float)pts[2].x, (float)pts[2].y});
	q.setPoint(3, {(float)pts[3].x, (float)pts[3].y});
	q.setFillColor(col);
	q.setOutlineColor(invert(col));
	q.setOutlineThickness(-0.2);
	window.draw(q);
}

void circle(const Vector2D &cen, float rad, const sf::Color &c, sf::RenderTarget &window) {
	static sf::CircleShape shape(50);
	shape.setRadius(rad);
	shape.setFillColor(c);
	shape.setOutlineColor(invert(c));
	shape.setOutlineThickness(-0.2);
	shape.setPosition(cen.x, cen.y);
	shape.setOrigin(rad, rad);
	window.draw(shape);
}
void line(const Vector2D &a, const Vector2D &b, const sf::Color &c1, const sf::Color &c2, sf::RenderTarget &window) {
	static sf::VertexArray lines(sf::LinesStrip, 2);
	lines[0].position = {(float)a.x, (float)a.y};
	lines[0].color = c1;
	lines[1].color = c2;
	lines[1].position = {(float)b.x, (float)b.y};
	window.draw(lines);
}
void line(const Vector2D &a, const Vector2D &b, const sf::Color &c, sf::RenderTarget &window) {
	line(a, b, c, c, window);
}
int text(std::string str, const Vector2D &pos, float size, const sf::Color &c, sf::RenderTarget &window) {
	static sf::Font font;
	if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
		return -1;
	static sf::Text text("", font, 30);
	text.setString(str);
	text.setScale(size, -size);
	text.setColor(c);
	text.setPosition(pos.x, pos.y);
	window.draw(text);
	return 0;
}

/**
 * Returns a suitable division size of the Scale
 * depending on the viewSize and the windowSize
 */
float getScaleDivision(float viewSize, float windowSize) {
	// Values to decide a "Good" size of unit cell
	const float vals[] = {5.0, 2.5, 2.0, 1.0, 0.5, 0.25, 0.2, 0.1},
				maxPix = 140.0f,  // Range of actual pixel size(for unit cell) allowed
		minPix = 120.0f;		  //

	bool isPositive = viewSize >= 0;

	if (!isPositive)
		viewSize = -viewSize;

	float val = vals[0],
		  sep = pow(10, floor(log10(viewSize))),
		  pix = windowSize * sep / viewSize,
		  dist = std::min(fabs(vals[0] * pix - maxPix), fabs(vals[0] * pix - minPix));

	for (size_t i = 1; i < 8; i++) {
		float tdist = std::min(fabs(vals[i] * pix - maxPix), fabs(vals[i] * pix - minPix));
		if (dist > tdist) {
			val = vals[i];
			dist = tdist;
		}
	}
	return (isPositive ? 1.0 : -1.0) * val * sep;
}
std::vector<std::pair<int, float>> getXPoints(const sf::RenderTarget &window) {
	sf::View view = window.getView();
	std::vector<std::pair<int, float>> markings;
	float Left = view.getCenter().x - view.getSize().x / 2,
		  Right = view.getCenter().x + view.getSize().x / 2,
		  xSep = getScaleDivision(view.getSize().x, window.getSize().x),
		  actualX = Left - fmod(Left, xSep),
		  pixelX = window.mapCoordsToPixel({actualX, 0}).x;
	int noOfLinesX = Right / xSep - (Left - fmod(Left, xSep)) / xSep + 1;

	for (int i = 0; i < noOfLinesX; i++) {
		pixelX = window.mapCoordsToPixel({actualX, 0}).x;
		markings.emplace_back(pixelX, actualX);
		actualX += xSep;
	}
	return markings;
}
std::vector<std::pair<int, float>> getYPoints(const sf::RenderTarget &window) {
	sf::View view = window.getView();
	std::vector<std::pair<int, float>> markings;
	float Bottom = view.getCenter().y - view.getSize().y / 2,
		  Top = view.getCenter().y + view.getSize().y / 2,
		  ySep = getScaleDivision(view.getSize().y, window.getSize().y),
		  actualY = Bottom - fmod(Bottom, ySep),
		  pixelY = window.mapCoordsToPixel({0, actualY}).y;
	int noOfLinesY = Top / ySep - (Bottom - fmod(Bottom, ySep)) / ySep + 1;

	for (int i = 0; i < noOfLinesY; i++) {
		pixelY = window.mapCoordsToPixel({0, actualY}).y;
		markings.emplace_back(pixelY, actualY);
		actualY += ySep;
	}
	return markings;
}
void addGridMarks(std::vector<sf::Vertex> &Line,
				  std::vector<std::pair<int, float>> &xMarkings,
				  std::vector<std::pair<int, float>> &yMarkings,
				  int length = 10,
				  int offset = 0) {
	float start = offset,
		  end = start + length;
	for (auto p : xMarkings) {
		Line.emplace_back(sf::Vector2f(p.first, start), sf::Color::Red);
		Line.emplace_back(sf::Vector2f(p.first, end), sf::Color::Red);
	}
	start = offset;
	end = start + length;
	for (auto p : yMarkings) {
		Line.emplace_back(sf::Vector2f(start, p.first), sf::Color::Red);
		Line.emplace_back(sf::Vector2f(end, p.first), sf::Color::Red);
	}
}

void gridLable(sf::RenderTarget &window,
			   std::vector<std::pair<int, float>> &xMarkings,
			   std::vector<std::pair<int, float>> &yMarkings,
			   int offset = 10) {
	static sf::Font font;
	if (font.getInfo().family.size() == 0)
		if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
			return;
	static sf::Text text("", font, 15);
	for (auto p : xMarkings) {
		text.setString(to_string_stream(p.second));
		text.setPosition(p.first, offset);
		sf::Vector2f point(text.getGlobalBounds().left + text.getGlobalBounds().width, text.getGlobalBounds().top + text.getGlobalBounds().height);
		if (point.x > window.getSize().x)
			text.move(window.getSize().x - point.x, 0);
		if (point.y > window.getSize().y)
			text.move(0, window.getSize().y - point.y);
		if (text.getPosition().y < 0)
			text.move(0, -text.getPosition().y);
		window.draw(text);
	}
	for (auto p : yMarkings) {
		text.setString(to_string_stream(p.second));
		text.setPosition(offset, p.first);
		sf::Vector2f point(text.getGlobalBounds().left + text.getGlobalBounds().width, text.getGlobalBounds().top + text.getGlobalBounds().height);
		if (point.x > window.getSize().x)
			text.move(window.getSize().x - point.x, 0);
		if (text.getPosition().x < 0)
			text.move(-text.getPosition().x, 0);
		if (point.y > window.getSize().y)
			text.move(0, window.getSize().y - point.y);
		window.draw(text);
	}
}

void drawGrid(sf::RenderTarget &window, bool changed) {
	sf::View view = window.getView(), uiView;
	static std::vector<sf::Vertex> Line;
	static std::vector<std::pair<int, float>> xMarkings, yMarkings;
	if (changed) {
		Line.clear();
		xMarkings = getXPoints(window);
		yMarkings = getYPoints(window);
		addGridMarks(Line, xMarkings, yMarkings);
	}
	window.setView(window.getDefaultView());
	window.draw(&Line[0], Line.size(), sf::Lines);
	gridLable(window, xMarkings, yMarkings);
	window.setView(view);
}