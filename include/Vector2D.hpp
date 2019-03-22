#ifndef VECTOR2D_H
#define VECTOR2D_H

#define DataType double

#include <cmath>
#include <string>

class Vector2D {
   public:
	////////////////////////////////////////////////////////////
	/// \brief Default constructor
	///
	/// Creates a Vector2D(0, 0).
	///
	////////////////////////////////////////////////////////////
	Vector2D() : x(0), y(0) {}

	////////////////////////////////////////////////////////////
	/// \brief Construct the vector from its coordinates
	///
	/// \param X X coordinate
	/// \param Y Y coordinate
	///
	////////////////////////////////////////////////////////////
	Vector2D(DataType X, DataType Y) : x(X), y(Y) {}

	////////////////////////////////////////////////////////////
	// Member data
	////////////////////////////////////////////////////////////
	DataType x;  ///< X coordinate of the vector
	DataType y;  ///< Y coordinate of the vector
};

////////////////////////////////////////////////////////////
/// \relates Vector2D
/// \brief Overload of unary operator -
///
/// \param right Vector to negate
///
/// \return Memberwise opposite of the vector
///
////////////////////////////////////////////////////////////
inline Vector2D operator-(const Vector2D &right) {
	return Vector2D(-right.x, -right.y);
}

////////////////////////////////////////////////////////////
/// \relates Vector2D
/// \brief Overload of binary operator +=
///
/// This operator performs a memberwise addition of both vectors,
/// and assigns the result to \a left.
///
/// \param left Left operand (a vector)
/// \param right Right operand (a vector)
///
/// \return Reference to \a left
///
////////////////////////////////////////////////////////////
inline Vector2D &operator+=(Vector2D &left, const Vector2D &right) {
	left.x += right.x;
	left.y += right.y;
	return left;
}

////////////////////////////////////////////////////////////
/// \relates Vector2D
/// \brief Overload of binary operator -=
///
/// This operator performs a memberwise subtraction of both vectors,
/// and assigns the result to \a left.
///
/// \param left Left operand (a vector)
/// \param right Right operand (a vector)
///
/// \return Reference to \a left
///
////////////////////////////////////////////////////////////
inline Vector2D &operator-=(Vector2D &left, const Vector2D &right) {
	left.x -= right.x;
	left.y -= right.y;
	return left;
}

////////////////////////////////////////////////////////////
/// \relates Vector2D
/// \brief Overload of binary operator +
///
/// \param left Left operand (a vector)
/// \param right Right operand (a vector)
///
/// \return Memberwise addition of both vectors
///
////////////////////////////////////////////////////////////
inline Vector2D operator+(const Vector2D &left, const Vector2D &right) {
	return Vector2D(left.x + right.x, left.y + right.y);
}

////////////////////////////////////////////////////////////
/// \relates Vector2D
/// \brief Overload of binary operator -
///
/// \param left Left operand (a vector)
/// \param right Right operand (a vector)
///
/// \return Memberwise subtraction of both vectors
///
////////////////////////////////////////////////////////////
inline Vector2D operator-(const Vector2D &left, const Vector2D &right) {
	return Vector2D(left.x - right.x, left.y - right.y);
}

////////////////////////////////////////////////////////////
/// \relates Vector2D
/// \brief Overload of binary operator *
///
/// \param left Left operand (a vector)
/// \param right Right operand (a scalar value)
///
/// \return Memberwise multiplication by \a right
///
////////////////////////////////////////////////////////////
inline Vector2D operator*(const Vector2D &left, DataType right) {
	return Vector2D(left.x * right, left.y * right);
}

////////////////////////////////////////////////////////////
/// \relates Vector2D
/// \brief Overload of binary operator *
///
/// \param left Left operand (a scalar value)
/// \param right Right operand (a vector)
///
/// \return Memberwise multiplication by \a left
///
////////////////////////////////////////////////////////////
inline Vector2D operator*(DataType left, const Vector2D &right) {
	return Vector2D(right.x * left, right.y * left);
}

////////////////////////////////////////////////////////////
/// \relates Vector2D
/// \brief Overload of binary operator *=
///
/// This operator performs a memberwise multiplication by \a right,
/// and assigns the result to \a left.
///
/// \param left Left operand (a vector)
/// \param right Right operand (a scalar value)
///
/// \return Reference to \a left
///
////////////////////////////////////////////////////////////
inline Vector2D &operator*=(Vector2D &left, DataType right) {
	left.x *= right;
	left.y *= right;
	return left;
}

////////////////////////////////////////////////////////////
/// \relates Vector2
/// \brief Overload of binary operator /
///
/// \param left Left operand (a vector)
/// \param right Right operand (a scalar value)
///
/// \return Memberwise division by \a right
///
////////////////////////////////////////////////////////////
inline Vector2D operator/(const Vector2D &left, DataType right) {
	return Vector2D(left.x / right, left.y / right);
}

////////////////////////////////////////////////////////////
/// \relates Vector2D
/// \brief Overload of binary operator /=
///
/// This operator performs a memberwise division by \a right,
/// and assigns the result to \a left.
///
/// \param left Left operand (a vector)
/// \param right Right operand (a scalar value)
///
/// \return Reference to \a left
///
////////////////////////////////////////////////////////////
inline Vector2D &operator/=(Vector2D &left, DataType right) {
	left.x /= right;
	left.y /= right;

	return left;
}

////////////////////////////////////////////////////////////
/// \relates Vector2D
/// \brief Overload of binary operator ==
///
/// This operator compares strict equality between two vectors.
///
/// \param left Left operand (a vector)
/// \param right Right operand (a vector)
///
/// \return True if \a left is equal to \a right
///
////////////////////////////////////////////////////////////
inline bool operator==(const Vector2D &left, const Vector2D &right) {
	return (left.x == right.x) && (left.y == right.y);
}

////////////////////////////////////////////////////////////
/// \relates Vector2D
/// \brief Overload of binary operator !=
///
/// This operator compares strict difference between two vectors.
///
/// \param left Left operand (a vector)
/// \param right Right operand (a vector)
///
/// \return True if \a left is not equal to \a right
///
////////////////////////////////////////////////////////////
inline bool operator!=(const Vector2D &left, const Vector2D &right) {
	return (left.x != right.x) || (left.y != right.y);
}

////////////////////////////////////////////////////////////
///	\relates Vector2D
///	\brief Squared Distance between two vectors
///	This function returns the Squared Distance between two Points
///
///	\param	a (first Point)
///	\param	b (second Point)
///
///	\return Squared Distance
////////////////////////////////////////////////////////////
inline DataType disSq(const Vector2D &a, const Vector2D &b) {
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

////////////////////////////////////////////////////////////
///	\relates Vector2D
///	\brief Distance between two vectors
///	This function returns the Distance between two Points
///
///	\param	a (first Point)
///	\param	b (second Point)
///
///	\return Distance
////////////////////////////////////////////////////////////
inline DataType dis(const Vector2D &a, const Vector2D &b) {
	return std::sqrt(disSq(a, b));
}

////////////////////////////////////////////////////////////
///	\relates Vector2D
///	\brief Squared Length of a vector
///	This function returns the Squared Length of a vector
///
///	\param	a
///
///	\return Squared Length
////////////////////////////////////////////////////////////
inline DataType lenSq(const Vector2D &a) {
	return (a.x * a.x + a.y * a.y);
}

////////////////////////////////////////////////////////////
///	\relates Vector2D
///	\brief Length of a vector
///	This function returns the Length of a vector
///
///	\param	a
///
///	\return Length
////////////////////////////////////////////////////////////
inline DataType len(const Vector2D &a) {
	return std::sqrt(lenSq(a));
}

////////////////////////////////////////////////////////////
///	\relates Vector2D
///	\brief Normalized vector
///	This function returns a unit vector in the direction of the vector
///
///	\param	a
///
///	\return unit Vector
////////////////////////////////////////////////////////////
inline Vector2D unit(const Vector2D &a) {
	return a / len(a);
}

////////////////////////////////////////////////////////////
///	\relates Vector2D
///	\brief Dot Product of two Vectors
///	This function returns Dot Product of two Vectors
///
///	\param	a
///	\param	b
///
///	\return Dot Product
////////////////////////////////////////////////////////////
inline DataType dot(const Vector2D &a, const Vector2D &b) {
	return a.x * b.x + a.y * b.y;
}

////////////////////////////////////////////////////////////
///	\relates Vector2D
///	\brief Cross Product of two Vectors
///	This function returns cross Product of two Vectors
///
///	\param	a
///	\param	b
///
///	\return Dot Product
////////////////////////////////////////////////////////////
inline DataType cross(const Vector2D &a, const Vector2D &b) {
	return a.x * b.y - a.y * b.x;
}

////////////////////////////////////////////////////////////
///	\relates Vector2D
///	\brief Projection of Vector on another Vector
///	This function returns Projection of First Vector on the Second Vector
///
///	\param	a
///	\param	b
///
///	\return Projection
////////////////////////////////////////////////////////////
inline Vector2D proj(const Vector2D &a, const Vector2D &b) {
	return (b * dot(a, b)) / lenSq(b);
}

////////////////////////////////////////////////////////////
///	\relates Vector2D
///	\brief Projection of Vector on another unit Vector
///	This function returns Projection of First Vector on the Second Vector
///
///	\param	a
///	\param	b
///
///	\return Projection
////////////////////////////////////////////////////////////
inline Vector2D projOnUnit(const Vector2D &a, const Vector2D &b) {
	return b * dot(a, b);
}

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
inline DataType distFromLine(const Vector2D &start, const Vector2D &end, const Vector2D &pt) {
	DataType param = dot(pt - start, end - start) / disSq(end, start);
	if (param < 0)
		return disSq(start, pt);
	if (param > 1)
		return disSq(end, pt);
	else
		return disSq(pt, start + param * (end - start));
}

inline Vector2D rotate(const Vector2D &v, double angle) {
	double s = std::sin(angle), c = std::cos(angle);
	return {v.x * c - v.y * s, v.x * s + v.y * c};
}
inline Vector2D rotate(const Vector2D &v, double s, double c) {
	return {v.x * c - v.y * s, v.x * s + v.y * c};
}

////////////////////////////////////////////////////////////
///	\relates Vector2D
///	\brief Returns a String representation of the Vector
///
///	This Functions returns the String (<em>std::string</em>) representation of the Vector
///	in the form (V.x , V.y)
///
///	\param a (a vector)
///
///	\return std::string
////////////////////////////////////////////////////////////

#endif  // VECTOR2D_H
