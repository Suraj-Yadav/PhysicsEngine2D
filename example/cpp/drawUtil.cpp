#include "drawUtil.hpp"

#include <PhysicsEngine2D/util.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/OpenGL.hpp>
#include <sstream>

#include "fontManager.hpp"

inline sf::Color invert(const sf::Color &c) {
	return sf::Color(
		((255 - c.r) << 24) + ((255 - c.g) << 16) + ((255 - c.b) << 8) + 255);
}
inline sf::Vector2f toVec(const Vector2D &v) {
	return sf::Vector2f({float(v.x), float(v.y)});
}

DrawUtil::DrawUtil(sf::RenderTarget &window, const std::string &fontName)
	: window(window), quads(sf::Quads, 0), lines(sf::Lines, 0), circle(50) {
	if (!font.loadFromFile(fontManager::findFont(fontName).path.string())) {
		throw std::runtime_error("Font File not found: " + fontName);
	}

	text = sf::Text("", font, 30);
}

DrawUtil::~DrawUtil() {}

void DrawUtil::line(
	const Vector2D &a,
	const Vector2D &b,
	const sf::Color &c1,
	const sf::Color &c2,
	int width) {
	if (width == 0) {
		static sf::Vertex vertices[2];
		vertices[0].position = toVec(a);
		vertices[1].position = toVec(b);
		vertices[0].color = c1;
		vertices[1].color = c2;
		lines.append(vertices[0]);
		lines.append(vertices[1]);
	}
	else {
		static sf::Vertex vertices[4];
		const double actualWidth =
			-(window.getView().getSize().y / window.getSize().y) * width;
		const auto normal = (b - a).unit().rotate(1, 0);
		vertices[0].position = toVec(a + actualWidth * normal / 2);
		vertices[1].position = toVec(a - actualWidth * normal / 2);
		vertices[2].position = toVec(b - actualWidth * normal / 2);
		vertices[3].position = toVec(b + actualWidth * normal / 2);
		vertices[0].color = vertices[1].color = c1;
		vertices[2].color = vertices[3].color = c2;
		quads.append(vertices[0]);
		quads.append(vertices[1]);
		quads.append(vertices[2]);
		quads.append(vertices[3]);
	}
}

template <typename T> std::string to_string_stream(const T &n) {
	std::ostringstream stm;
	stm << n;
	return stm.str();
}

void DrawUtil::drawCircle(const Vector2D &cen, double rad, const sf::Color &c) {
	circle.setRadius(rad);
	circle.setFillColor(c);
	// circle.setOutlineColor(invert(c));
	// circle.setOutlineThickness(-std::min(0.2, 0.1 * rad));
	circle.setPosition(cen.x, cen.y);
	circle.setOrigin(rad, rad);
	window.draw(circle);
}

void DrawUtil::drawText(
	std::string str, const Vector2D &pos, int size, const sf::Color &c) {
	const double actualSize =
		-(window.getView().getSize().y / window.getSize().y) * size;
	text.setString(str);
	text.setScale(actualSize / 30, -actualSize / 30);
	text.setFillColor(c);
	text.setPosition(pos.x, pos.y);
	window.draw(text);
}

/**
 * Returns a suitable division size of the Scale
 * depending on the viewSize and the windowSize
 */
float getScaleDivision(float viewSize, float windowSize) {
	// Values to decide a "Good" size of unit cell
	const float
		vals[] = {5.0, 2.5, 2.0, 1.0, 0.5, 0.25, 0.2, 0.1},
		maxPix = 140.0f,  // Range of actual pixel size(for unit cell) allowed
		minPix = 120.0f;  //

	bool isPositive = viewSize >= 0;

	if (!isPositive) viewSize = -viewSize;

	float val = vals[0], sep = pow(10, floor(log10(viewSize))),
		  pix = windowSize * sep / viewSize,
		  dist = std::min(
			  fabs(vals[0] * pix - maxPix), fabs(vals[0] * pix - minPix));

	for (size_t i = 1; i < 8; i++) {
		float tdist = std::min(
			fabs(vals[i] * pix - maxPix), fabs(vals[i] * pix - minPix));
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
void addGridMarks(
	std::vector<sf::Vertex> &Line,
	std::vector<std::pair<int, float>> &xMarkings,
	std::vector<std::pair<int, float>> &yMarkings,
	int length = 10,
	int offset = 0) {
	float start = offset, end = start + length;
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

void gridLable(
	sf::RenderTarget &window,
	std::vector<std::pair<int, float>> &xMarkings,
	std::vector<std::pair<int, float>> &yMarkings,
	int offset = 10) {
	static sf::Font font;
	if (font.getInfo().family.size() == 0)
		if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) return;
	static sf::Text text("", font, 15);
	for (auto p : xMarkings) {
		text.setString(to_string_stream(p.second));
		text.setPosition(p.first, offset);
		sf::Vector2f point(
			text.getGlobalBounds().left + text.getGlobalBounds().width,
			text.getGlobalBounds().top + text.getGlobalBounds().height);
		if (point.x > window.getSize().x)
			text.move(window.getSize().x - point.x, 0);
		if (point.y > window.getSize().y)
			text.move(0, window.getSize().y - point.y);
		if (text.getPosition().y < 0) text.move(0, -text.getPosition().y);
		window.draw(text);
	}
	for (auto p : yMarkings) {
		text.setString(to_string_stream(p.second));
		text.setPosition(offset, p.first);
		sf::Vector2f point(
			text.getGlobalBounds().left + text.getGlobalBounds().width,
			text.getGlobalBounds().top + text.getGlobalBounds().height);
		if (point.x > window.getSize().x)
			text.move(window.getSize().x - point.x, 0);
		if (text.getPosition().x < 0) text.move(-text.getPosition().x, 0);
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
