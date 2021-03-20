#ifndef RANGE_HPP
#define RANGE_HPP

#include "Vector2D.hpp"

template <class Type> struct Range {
	Type start, end;
	inline Range(Type start, Type end)
		: start(std::min(start, end)), end(std::max(start, end)) {}

	inline bool contains(const Type &val) const {
		return start <= val && val <= end;
	}

	inline bool contains(const Range &that) const {
		return start <= that.start && that.end <= end;
	}

	inline bool intersects(const Range &that) const {
		return start <= that.end && that.start <= end;
	}

	bool operator<(const Range<Type> &that) const {
		return comparePair(start, that.start, end < that.end);
	}

	friend std::ostream &operator<<(std::ostream &out, Range<Type> const &b) {
		return out << "Interval [" << b.start << ":" << b.end << "]";
	}
};

template <class Type> struct Range2D {
	Range<Type> rangeX, rangeY;
	inline Range2D(Type startX, Type endX, Type startY, Type endY)
		: rangeX(startX, endX), rangeY(startY, endY) {}

	inline bool contains(const Vector2D &p) const {
		return rangeX.contains(p.x) && rangeY.contains(p.y);
	}

	inline bool contains(const Range2D &that) const {
		return rangeX.contains(that.rangeX) && rangeY.contains(that.rangeY);
	}

	inline bool intersects(const Range2D &that) const {
		return rangeX.intersects(that.rangeX) && rangeY.intersects(that.rangeY);
	}

	friend std::ostream &operator<<(std::ostream &out, const Range2D &b) {
		return out << "Range2D(" << b.rangeX << " x " << b.rangeY << ")";
	}
};

#endif	// RANGE_HPP
