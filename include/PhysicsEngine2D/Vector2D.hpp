#ifndef VECTOR2D_H
#define VECTOR2D_H

#define dataType	float
#define VECTOR_SIZE 2

#define _USE_MATH_DEFINES

#include <cmath>
#include <limits>
#include <ostream>
#include <string>

template <class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
almost_equal(T x, T y) {
	// the machine epsilon has to be scaled to the magnitude of the values used
	// and multiplied by the desired precision in ULPs (units in the last place)
	return std::abs(x - y) <=
			   std::numeric_limits<T>::epsilon() * std::abs(x + y)
		   // unless the result is subnormal
		   || std::abs(x - y) < std::numeric_limits<T>::min();
}

class Vector2D {
   public:
	Vector2D() : x(0), y(0) {}

	Vector2D(dataType X, dataType Y) : x(X), y(Y) {}

	dataType x;
	dataType y;

	inline dataType operator[](size_t i) const {
		i = i >= VECTOR_SIZE ? i % VECTOR_SIZE : i;
		if (i == 0) return x;
		return y;
	}

	inline Vector2D& operator+=(const Vector2D& that) {
		this->x += that.x;
		this->y += that.y;
		return *this;
	}

	inline Vector2D& operator-=(const Vector2D& that) {
		this->x -= that.x;
		this->y -= that.y;
		return *this;
	}

	inline Vector2D& operator*=(const dataType& factor) {
		this->x *= factor;
		this->y *= factor;
		return *this;
	}
	inline Vector2D& operator/=(const dataType& factor) {
		this->x /= factor;
		this->y /= factor;
		return *this;
	}

	inline Vector2D operator+(const Vector2D& that) const {
		return {this->x + that.x, this->y + that.y};
	}
	inline Vector2D operator-(const Vector2D& that) const {
		return {this->x - that.x, this->y - that.y};
	}

	inline Vector2D operator*(const dataType& factor) const {
		return {this->x * factor, this->y * factor};
	}
	inline Vector2D operator/(const dataType& factor) const {
		return {this->x / factor, this->y / factor};
	}

	inline bool operator==(const Vector2D& that) const {
		return this->x == that.x && this->y == that.y;
	}
	inline bool operator!=(const Vector2D& that) const {
		return this->x != that.x || this->y != that.y;
	}

	friend Vector2D operator-(const Vector2D& right) {
		return Vector2D(-right.x, -right.y);
	}

	friend Vector2D operator*(dataType left, const Vector2D& right) {
		return Vector2D(right.x * left, right.y * left);
	}

	inline dataType lenSq() const {
		return this->x * this->x + this->y * this->y;
	}
	inline dataType len() const { return std::sqrt(this->lenSq()); }

	inline Vector2D unit() const { return *this / this->len(); };

	inline dataType dot(const Vector2D& that) const {
		return this->x * that.x + this->y * that.y;
	}
	inline dataType cross(const Vector2D& that) const {
		return this->x * that.y - this->y * that.x;
	}

	inline Vector2D proj(const Vector2D& that) const {
		return that.unit() * this->dot(that);
	}
	inline Vector2D projOnUnit(const Vector2D& that) const {
		return that * this->dot(that);
	}

	inline Vector2D rotate(dataType s, dataType c) const {
		return {this->x * c - this->y * s, this->x * s + this->y * c};
	}
	inline Vector2D rotate(dataType angle) const {
		return this->rotate(std::sin(angle), std::cos(angle));
	}

	inline auto getMagnitudeAndDirection() const {
		const auto mag = this->len();
		if (almost_equal(mag, 0.0f))
			return std::make_pair(0.0f, Vector2D(0.0f, 0.0f));
		return std::make_pair(mag, this->operator/(mag));
	}
	friend std::ostream& operator<<(std::ostream& out, Vector2D const& v) {
		out.precision(std::numeric_limits<double>::max_digits10);
		return out << "(" << v.x << ", " << v.y << ")";
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
inline dataType distFromLine(
	const Vector2D& start, const Vector2D& end, const Vector2D& pt) {
	dataType param = (pt - start).dot(end - start) / (end - start).lenSq();
	if (param < 0) return (start - pt).lenSq();
	if (param > 1)
		return (end - pt).lenSq();
	else
		return (pt - (start + param * (end - start))).lenSq();
}

#define comparePair(a1, b1, comparisonOfA2B2) \
	((a1 < b1) || (!(b1 < a1) && (comparisonOfA2B2)))

// struct AABB {
// 	Vector2D min, max;
// 	AABB(dataType left, dataType right, dataType bottom, dataType top)
// 		: min(left, bottom), max(right, top) {}
// 	bool contains(const Vector2D &p) const {
// 		return min.x <= p.x && p.x <= max.x && min.y <= p.y && p.y <= max.y;
// 	}
// };

// template <class Type> struct Interval {
// 	Type low, high;
// 	inline Interval()
// 		: low(std::numeric_limits<Type>::max()),
// 		  high(std::numeric_limits<Type>::lowest()) {}
// 	inline Interval(Type start, Type end) : low(start), high(end) {}

// 	friend std::ostream &operator<<(std::ostream &out, Interval const &b) {
// 		return out << "Interval (" << b.low << "," << b.high << ")";
// 	}
// };

// template <typename T>
// bool operator<(const Interval<T> &x, const Interval<T> &y) {
// 	return comparePair(x.low, x.high, y.low, y.high);
// }

#endif	// VECTOR2D_H
