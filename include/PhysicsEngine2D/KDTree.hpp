
#include <algorithm>

#include "Vector2D.hpp"

struct AABB {
	Vector2D min, max;
	AABB(dataType left, dataType right, dataType bottom, dataType top)
		: min(left, bottom), max(right, top) {}
	bool contains(const Vector2D &p) const {
		return min.x <= p.x && p.x < max.x && min.y <= p.y && p.y < max.y;
	}
};

template <class ValueType> class KdTree {
	static const int NULL_NODE = -1;

	struct Node {
		Vector2D p;
		ValueType value;
		int left, right;
		Node() {}
		Node(const Vector2D &p, const ValueType &value)
			: p(p), value(value), left(NULL_NODE), right(NULL_NODE) {}

		friend std::ostream &operator<<(std::ostream &out, const Node &n) {
			return out << "[ p=" << n.p << ", value=" << n.value
					   << ", left=" << n.left << ", right=" << n.right
					   << " ]\n";
		}
	};

	std::vector<Node> nodes;
	int root = NULL_NODE;

	void inRange(
		int x, int depth, const AABB &aabb, std::vector<ValueType> &insides) {
		if (x == NULL_NODE) {
			return;
		}
		if (aabb.max[depth] < nodes[x].p[depth]) {
			inRange(nodes[x].left, (depth + 1) % VECTOR_SIZE, aabb, insides);
		}
		else if (aabb.min[depth] > nodes[x].p[depth]) {
			inRange(nodes[x].right, (depth + 1) % VECTOR_SIZE, aabb, insides);
		}
		else {
			if (aabb.contains(nodes[x].p)) insides.push_back(nodes[x].value);
			inRange(nodes[x].left, (depth + 1) % VECTOR_SIZE, aabb, insides);
			inRange(nodes[x].right, (depth + 1) % VECTOR_SIZE, aabb, insides);
		}
	}

	int generateKdTree(int i, int j, int depth = 0) {
		if (i >= j) {
			return NULL_NODE;
		}

		int mid = (i + j) / 2;

		auto start = std::next(nodes.begin(), i),
			 end = std::next(nodes.begin(), j);

		std::nth_element(
			start, std::next(start, mid), end,
			[&](const Node &a, const Node &b) {
				return a.p[depth] < b.p[depth];
			});

		nodes[mid].left = generateKdTree(i, mid, (depth + 1) % VECTOR_SIZE);
		nodes[mid].right =
			generateKdTree(mid + 1, j, (depth + 1) % VECTOR_SIZE);

		return mid;
	}

	void printTree(int root, int level) {
		if (root == NULL_NODE || root >= nodes.size()) {
			return;
		}
		printLn(level, nodes[root].p.x, nodes[root].p.y);
		printTree(nodes[root].left, level + 1);
		printTree(nodes[root].right, level + 1);
	}

   public:
	KdTree(
		const std::vector<Vector2D> &points,
		const std::vector<ValueType> &values) {
		assert(points.size() == values.size());
		// nodes.resize(points.size());
		// nodes.reserve(points.size());
		for (size_t i = 0; i < points.size(); ++i) {
			nodes.emplace_back(points[i], values[i]);
		}

		// for (size_t i = 0; i < points.size(); i++) {
		// 	print(__FILE__, __LINE__, debug(i));
		// 	nodes.push_back(Node(points[i], values[i]));
		// 	// nodes.emplace_back(points[i], values[i]);
		// }

		// printLn(
		// 	std::min_element(
		// 		points.begin(), points.end(),
		// 		[](const Vector2D &a, const Vector2D &b) {
		// 			return a.x < b.x;
		// 		})->x -
		// 		10,
		// 	std::max_element(
		// 		points.begin(), points.end(),
		// 		[](const Vector2D &a, const Vector2D &b) {
		// 			return a.x < b.x;
		// 		})->x +
		// 		10,
		// 	std::min_element(
		// 		points.begin(), points.end(),
		// 		[](const Vector2D &a, const Vector2D &b) {
		// 			return a.y < b.y;
		// 		})->y -
		// 		10,
		// 	std::max_element(
		// 		points.begin(), points.end(),
		// 		[](const Vector2D &a, const Vector2D &b) {
		// 			return a.y < b.y;
		// 		})->y +
		// 		10);
		// printLn(nodes.size());

		root = generateKdTree(0, nodes.size());
		// printTree(root, 0);
	}

	auto range(const AABB &aabb) {
		std::vector<ValueType> insides;
		inRange(root, 0, aabb, insides);
		return insides;
	}
};