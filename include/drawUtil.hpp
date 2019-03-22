#ifndef UTIL_H
#define UTIL_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <array>
#include <string>

#include "Vector2D.hpp"

// #define ballToBallDamp 0.9
// #define ballToWallDamp 0.9

#define RADIUS 0.05f

void AABB(double left, double bottom, double right, double top, const sf::Color &col, sf::RenderTarget &window);
void quad(const std::array<Vector2D, 4> &pts, const sf::Color &col, sf::RenderTarget &window);

void circle(const Vector2D &cen, float rad, const sf::Color &c, sf::RenderTarget &window);

void line(const Vector2D &a, const Vector2D &b, const sf::Color &c1, const sf::Color &c2, sf::RenderTarget &window);
void line(const Vector2D &a, const Vector2D &b, const sf::Color &c, sf::RenderTarget &window);

int text(std::string str, const Vector2D &pos, float size, const sf::Color &c, sf::RenderTarget &window);

void drawGrid(sf::RenderTarget &window, bool change);

// std::string str(const Vector2D &a);

#endif  // UTIL_H
