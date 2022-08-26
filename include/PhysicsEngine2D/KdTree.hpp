#ifndef KD_TREE_HPP
#define KD_TREE_HPP
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "Range.hpp"
#include "Vector2D.hpp"
#include "util.hpp"

template <class ValueType> class KdTree {
	static const int NULL_NODE = -1;

	struct Node {
		Vector2D p;
		ValueType value;
		int left, right;
		Node() {}
		Node(const Vector2D& p, const ValueType& value)
			: p(p), value(value), left(NULL_NODE), right(NULL_NODE) {}

		friend std::ostream& operator<<(std::ostream& out, const Node& n) {
			return out << "[ p=" << n.p << ", value=" << n.value
					   << ", left=" << n.left << ", right=" << n.right
					   << " ]\n";
		}
	};

	std::vector<Node> nodes;
	int root = NULL_NODE;

	void inRange(
		int x, int depth, const Range2D<dataType>& range2d,
		std::vector<ValueType>& insides) {
		if (x == NULL_NODE) {
			return;
		}
		const auto& range = depth == 0 ? range2d.rangeX : range2d.rangeY;
		const auto& coordinate = depth == 0 ? nodes[x].p.x : nodes[x].p.y;
		if (range.end < coordinate) {
			inRange(nodes[x].left, (depth + 1) % VECTOR_SIZE, range2d, insides);
		}
		else if (range.start > coordinate) {
			inRange(
				nodes[x].right, (depth + 1) % VECTOR_SIZE, range2d, insides);
		}
		else {
			if (range2d.contains(nodes[x].p)) insides.push_back(nodes[x].value);
			inRange(nodes[x].left, (depth + 1) % VECTOR_SIZE, range2d, insides);
			inRange(
				nodes[x].right, (depth + 1) % VECTOR_SIZE, range2d, insides);
		}
	}

	int generateKdTree(int i, int j, int depth = 0) {
		if (i >= j) {
			return NULL_NODE;
		}
		// for (auto &elem : nodes) {
		// 	elem.p = elem.p;
		// }

		int mid = (i + j - 1) / 2;

		auto start = std::next(nodes.begin(), i),
			 end = std::next(nodes.begin(), j);

		std::sort(start, end, [&](const Node& a, const Node& b) {
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
		writeF(
			std::cout, "ASY dot(%,L=Label(\"%,%\"));\n", nodes[root].p, level,
			nodes[root].value);
		printTree(nodes[root].left, (level + 1) % VECTOR_SIZE);
		printTree(nodes[root].right, (level + 1) % VECTOR_SIZE);
	}

   public:
	KdTree() : KdTree(std::vector<Vector2D>(0), std::vector<ValueType>(0)) {}
	KdTree(
		const std::vector<Vector2D>& points,
		const std::vector<ValueType>& values) {
		if (points.size() != values.size()) {
			throw std::invalid_argument(
				"Size of points and values should be equal");
		}
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

	auto rangeQuery(const Range2D<dataType>& range2d) {
		std::vector<ValueType> insides;
		inRange(root, 0, range2d, insides);
		return insides;
	}
};
#endif	// KD_TREE_HPP
