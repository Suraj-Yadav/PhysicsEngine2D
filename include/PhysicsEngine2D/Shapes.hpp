#ifndef SHAPE_H
#define SHAPE_H

#include <algorithm>
#include <array>

#include "Constants.hpp"
#include "Vector2D.hpp"

std::ostream& operator<<(std::ostream& out, const ShapeType& type);
class BaseShape {
   protected:
	inline void setBounds(
		double left, double bottom, double right, double top) {
		this->top = top;
		this->left = left;
		this->right = right;
		this->bottom = bottom;
	}

   public:
	BaseShape() : top(0), left(0), right(0), bottom(0) {}
	virtual ~BaseShape() {}
	virtual ShapeType getClass() { return BASESHAPE; }
	bool intersects(const BaseShape& anotherShape) const {
		if (this->right < anotherShape.left) return false;
		if (this->left > anotherShape.right) return false;
		if (this->top < anotherShape.bottom) return false;
		if (this->bottom > anotherShape.top) return false;
		return true;
	}
	friend std::ostream& operator<<(std::ostream& out, BaseShape const&) {
		return out << "BaseShape"
				   << "[]";
	}
	dataType top, left, right, bottom;
};

class DynamicShape : public BaseShape {
   protected:
	virtual void updateAABB(double delTime) = 0;

   public:
	DynamicShape(const Vector2D& pos, const Vector2D& vel, double m)
		: pos(pos), vel(vel), mass(m), invMass(1.0 / m) {}
	virtual ~DynamicShape() {}
	virtual void applyImpulse(const Vector2D& imp, const Vector2D&) {
		vel += imp * invMass;
	}
	virtual void move(double delTime) {
		pos += vel * delTime;
		updateAABB(delTime);
	}
	virtual ShapeType getClass() { return DYNAMICSHAPE; }
	friend std::ostream& operator<<(std::ostream& out, DynamicShape const& b) {
		return out << "DynamicShape"
				   << "["
				   << "pos:" << b.pos << ", vel:" << b.vel << ", "
				   << static_cast<const BaseShape&>(b) << "]";
	}

	Vector2D pos;
	Vector2D vel;
	double mass, invMass;
};

class Particle final : public DynamicShape {
	void updateAABB(double delTime) {
		setBounds(
			pos.x - rad + std::min(0.0, delTime * vel.x),
			pos.y - rad + std::min(0.0, delTime * vel.y),
			pos.x + rad + std::max(0.0, delTime * vel.x),
			pos.y + rad + std::max(0.0, delTime * vel.y));
	}

   public:
	Particle(const Vector2D& pos, const Vector2D& vel, double m, double r)
		: DynamicShape(pos, vel, m), rad(r) {
		updateAABB(0);
	}
	~Particle() {}
	ShapeType getClass() { return PARTICLE; }
	friend std::ostream& operator<<(std::ostream& out, Particle const& b) {
		return out << "Particle"
				   << "["
				   << "rad:" << b.rad << ", "
				   << static_cast<const DynamicShape&>(b) << "]";
	}

	double rad;
};

class RigidShape : public DynamicShape {
   public:
	RigidShape(
		const Vector2D& pos, const Vector2D& vel, double m, double i,
		double a = 0, double aV = 0)
		: DynamicShape(pos, vel, m),
		  inertia(i),
		  invInertia(1.0 / i),
		  angle(a),
		  angVel(aV) {}
	virtual ~RigidShape() {}
	void applyImpulse(const Vector2D& imp, const Vector2D& point) {
		angVel += (point - pos).cross(imp) * invInertia;
		vel += imp * invMass;
	}
	virtual void move(double delTime) {
		pos += vel * delTime;
		angle += angVel * delTime;
		updateAABB(delTime);
	}
	friend std::ostream& operator<<(std::ostream& out, RigidShape const& b) {
		return out << "RigidShape"
				   << "["
				   << "angle:" << b.angle << ", angVel:" << b.angVel << ", "
				   << static_cast<const DynamicShape&>(b) << "]";
	}
	double inertia, invInertia, angle, angVel;
};

// ball class
class Ball final : public RigidShape {
	void updateAABB(double delTime) {
		setBounds(
			pos.x - rad + std::min(0.0, delTime * vel.x),
			pos.y - rad + std::min(0.0, delTime * vel.y),
			pos.x + rad + std::max(0.0, delTime * vel.x),
			pos.y + rad + std::max(0.0, delTime * vel.y));
	}

   public:
	Ball(
		const Vector2D& initialPosition, const Vector2D& initialVelocity,
		double mass, double radius, double initialAngle = 0,
		double initialAngularVelocity = 0)
		: RigidShape(
			  initialPosition, initialVelocity, mass,
			  mass * radius * radius * 0.5, initialAngle,
			  initialAngularVelocity),
		  rad(radius) {
		updateAABB(0);
	}
	~Ball() {}
	ShapeType getClass() { return BALL; }
	friend std::ostream& operator<<(std::ostream& out, Ball const& b) {
		return out << "Ball"
				   << "[" << static_cast<const RigidShape&>(b) << "]";
	}
	double rad;
};

// Box class
class Box final : public RigidShape {
	void updateAABB(double delTime) {
		const auto sine = std::sin(angle), cosine = std::cos(angle);
		auto first = Vector2D(w, h).rotate(sine, cosine),
			 second = Vector2D(-w, h).rotate(sine, cosine);
		corner[0] = pos + 0.5 * first;
		corner[1] = pos + 0.5 * second;
		corner[2] = pos - 0.5 * first;
		corner[3] = pos - 0.5 * second;
		auto X =
			std::minmax({corner[0].x, corner[1].x, corner[2].x, corner[3].x});
		auto Y =
			std::minmax({corner[0].y, corner[1].y, corner[2].y, corner[3].y});
		setBounds(
			pos.x + X.first + std::min(0.0, delTime * vel.x),
			pos.y + Y.first + std::min(0.0, delTime * vel.y),
			pos.x + X.second + std::max(0.0, delTime * vel.x),
			pos.y + Y.second + std::max(0.0, delTime * vel.y));
	}

   public:
	Box(const Vector2D& pos, const Vector2D& vel, double mass, double width,
		double height, double initialAngle = 0,
		double initialAngularVelocity = 0)
		: RigidShape(
			  pos, vel, mass, mass * (width * width + height * height) / 12.0,
			  initialAngle, initialAngularVelocity),
		  w(width),
		  h(height) {
		updateAABB(0);
	}
	~Box() {}
	ShapeType getClass() { return BOX; }
	double w;
	double h;
	std::array<Vector2D, 4> corner;
};

class Line final : public BaseShape {
   public:
	Line(const Vector2D& a, const Vector2D& b)
		: start(a),
		  end(b),
		  normal((b - a).rotate(1, 0).unit()),
		  length((b - a).len()) {
		const double padding = std::max(0.05, 0.01 * (a - b).len());
		setBounds(
			std::min(start.x, end.x) - padding,
			std::min(start.y, end.y) - padding,
			std::max(start.x, end.x) + padding,
			std::max(start.y, end.y) + padding);
	}
	~Line() {}
	ShapeType getClass() { return LINE; }
	friend std::ostream& operator<<(std::ostream& out, Line const& b) {
		return out << "Line"
				   << "["
				   << "start:" << b.start << ", end:" << b.end << ", normal"
				   << b.normal << ", " << static_cast<const BaseShape&>(b)
				   << "]";
	}

	Vector2D start;
	Vector2D end;
	Vector2D normal;
	dataType length;
};

inline bool isTypeof(ShapeType a, ShapeType b) {
	int a1 = a, b1 = b;
	if (a1 == 0 || b1 == 0) return false;
	while (a1 < b1) b1 >>= 4;
	return a1 == b1;
}

#endif	// SHAPE_H
