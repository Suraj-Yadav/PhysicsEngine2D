#ifndef VECTOR2D_H
#define VECTOR2D_H

#define dataType double

#include <cmath>
#include <string>

template <class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
almost_equal(T x, T y) {
	// the machine epsilon has to be scaled to the magnitude of the values used
	// and multiplied by the desired precision in ULPs (units in the last place)
	return std::abs(x - y) <= std::numeric_limits<T>::epsilon() * std::abs(x + y)
		   // unless the result is subnormal
		   || std::abs(x - y) < std::numeric_limits<T>::min();
}

class Vector2D {
   public:
	Vector2D() : x(0), y(0) {}

	Vector2D(dataType X, dataType Y) : x(X), y(Y) {}

	dataType x;
	dataType y;

	inline Vector2D &operator+=(const Vector2D &that) {
		this->x += that.x;
		this->y += that.y;
		return *this;
	}

	inline Vector2D &operator-=(const Vector2D &that) {
		this->x -= that.x;
		this->y -= that.y;
		return *this;
	}

	inline Vector2D &operator*=(const dataType &factor) {
		this->x *= factor;
		this->y *= factor;
		return *this;
	}
	inline Vector2D &operator/=(const dataType &factor) {
		this->x /= factor;
		this->y /= factor;
		return *this;
	}

	inline Vector2D operator+(const Vector2D &that) const { return {this->x + that.x, this->y + that.y}; }
	inline Vector2D operator-(const Vector2D &that) const { return {this->x - that.x, this->y - that.y}; }

	inline Vector2D operator*(const dataType &factor) const { return {this->x * factor, this->y * factor}; }
	inline Vector2D operator/(const dataType &factor) const { return {this->x / factor, this->y / factor}; }

	inline bool operator==(const Vector2D &that) const { return this->x == that.x && this->y == that.y; }
	inline bool operator!=(const Vector2D &that) const { return this->x != that.x || this->y != that.y; }

	friend Vector2D operator-(const Vector2D &right) { return Vector2D(-right.x, -right.y); }

	friend Vector2D operator*(dataType left, const Vector2D &right) { return Vector2D(right.x * left, right.y * left); }

	inline dataType lenSq() const { return this->x * this->x + this->y * this->y; }
	inline dataType len() const { return std::sqrt(this->lenSq()); }

	inline Vector2D unit() const { return *this / this->len(); };

	inline dataType dot(const Vector2D &that) const { return this->x * that.x + this->y * that.y; }
	inline dataType cross(const Vector2D &that) const { return this->x * that.y - this->y * that.x; }

	inline Vector2D proj(const Vector2D &that) const { return this->dot(that) * that.unit(); }
	inline Vector2D projOnUnit(const Vector2D &that) const { return this->dot(that) * that; }

	inline Vector2D rotate(double s, double c) const { return {this->x * c - this->y * s, this->x * s + this->y * c}; }
	inline Vector2D rotate(double angle) const { return this->rotate(std::sin(angle), std::cos(angle)); }

	inline auto getMagnitudeAndDirection() const {
		const auto mag = this->len();
		if (almost_equal(mag, 0.0)) return std::make_pair(0.0, Vector2D(0.0, 0.0));
		return std::make_pair(mag, this->operator/(mag));
	}
};

////////////////////////////////////////////////////////////
///	\relates Vector2D
///	\brief Squared Distance between a Line Segment and a Point
///	This function returns between a Line Segment and a Point
///
///	\param	start	Starting Point of the Line Segment
///	\param	end		Ending Point of the Line Segment
///	\param	pt	Point
///
///	\return Squared Distance
////////////////////////////////////////////////////////////
inline dataType distFromLine(const Vector2D &start, const Vector2D &end, const Vector2D &pt) {
	dataType param = (pt - start).dot(end - start) / (end - start).lenSq();
	if (param < 0)
		return (start - pt).lenSq();
	if (param > 1)
		return (end - pt).lenSq();
	else
		return (pt - (start + param * (end - start))).lenSq();
}

#endif  // VECTOR2D_H
