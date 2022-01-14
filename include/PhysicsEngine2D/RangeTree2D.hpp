#ifndef RANGE_TREE_2D_HPP
#define RANGE_TREE_2D_HPP

#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

#include "Range.hpp"
#include "Vector2D.hpp"
#include "util.hpp"

template <class ValueType> class RangeTree2D {
	static const int NULL_NODE = -1;

	struct Meta {
		Vector2D point;
		ValueType value;
	};

	struct YNode {
		const Meta *meta;
		int left, right;

		YNode() : meta(nullptr), left(NULL_NODE), right(NULL_NODE) {}
		YNode(const Meta *d) : meta(d), left(NULL_NODE), right(NULL_NODE) {}

		inline bool operator<(const YNode &that) {
			return comparePair(
				this->meta->point.y, that.meta->point.y,
				comparePair(
					this->meta->value, this->meta->value,
					comparePair(
						this->meta->point.x, that.meta->point.x,
						this->meta < that.meta)));
		}
	};

	struct XNode {
		Meta *meta;
		int left, right;
		int yTreeIndex;
		Range<dataType> range;

		XNode() : meta(nullptr), range(0, 0) {}

		friend std::ostream &operator<<(std::ostream &out, XNode const &v) {
			out.precision(std::numeric_limits<double>::max_digits10);
			out << "[ ";
			if (v.meta != nullptr) {
				out << "value=" << v.meta->value << ", ";
			}

			return out << "range=" << v.range << ", left=" << v.left
					   << ", right=" << v.right << "]";
		}

		inline bool operator<(const XNode &that) const {
			return this->meta->point.x < that.meta->point.x;
		}

		std::string toGraphvizNode(const std::vector<YNode> &yTree) {
			std::stringstream out;
			if (meta != nullptr) {
				out << "[ label = \"{ value=" << meta->value
					<< "| p=" << meta->point << "}\" ]";
			}
			else {
				out << "[ label = \"{ range=" << range << "| left=" << left
					<< "| right=" << right << "| yTree=(";
				for (auto &elem : yTree)
					write(
						out, "(", elem.meta->value, elem.left, elem.right, ")");
				out << ")}\" ]";
			}
			return out.str();
		}
	};

	std::vector<Meta> data;
	std::vector<XNode> xNodes;
	std::vector<std::vector<YNode>> yTrees;
	const size_t N;
	size_t freeNode;

	size_t getNextFreeNode() {
		if (freeNode-- <= 0) {
			throw std::runtime_error("Invalid Memory State For RangeTree2D");
		}
		return freeNode;
	}

	/**
	 * Returns index of root node of Range Tree build with xNodes [i,j)
	 * @param i start index (inclusive)
	 * @param j end index (exclusive)
	 */
	int build2DRangeTree(int i, int j) {
		if (i >= j) {
			return NULL_NODE;
		}

		if (j - i == 1) {
			return i + N;
		}

		int mid = (i + j) / 2;

		// std::nth_element(
		// 	std::next(xNodes.begin(), i + N), std::next(xNodes.begin(), mid +
		// N), 	std::next(xNodes.begin(), j + N));

		int right = build2DRangeTree(mid, j);
		int left = build2DRangeTree(i, mid);

		int subRootIndex = getNextFreeNode();
		auto &subRoot = xNodes[subRootIndex];

		if (left != NULL_NODE) {
			subRoot.range.start = xNodes[left].range.start;
		}
		if (right != NULL_NODE) {
			subRoot.range.end = xNodes[right].range.end;
		}
		{
			int i = 0, j = 0, k = 0;
			auto &yArray = yTrees[subRootIndex], &leftYArray = yTrees[left],
				 &rightYArray = yTrees[right];

			yArray.resize(leftYArray.size() + rightYArray.size());

			while (i < leftYArray.size() && j < rightYArray.size()) {
				if (leftYArray[i] < rightYArray[j]) {
					yArray[k] = leftYArray[i];
					yArray[k].left = i;
					yArray[k].right = j;
					i++;
				}
				else {
					yArray[k] = rightYArray[j];
					yArray[k].left = i;
					yArray[k].right = j;
					j++;
				}
				k++;
			}
			while (i < leftYArray.size()) {
				yArray[k] = leftYArray[i];
				yArray[k].left = i;
				yArray[k].right = NULL_NODE;
				i++;
				k++;
			}
			while (j < rightYArray.size()) {
				yArray[k] = rightYArray[j];
				yArray[k].left = NULL_NODE;
				yArray[k].right = j;
				j++;
				k++;
			}
		}
		// std::merge(
		// 	xNodes[left].yTree.begin(), xNodes[left].yTree.end(),
		// 	xNodes[right].yTree.begin(), xNodes[right].yTree.end(),
		// 	std::back_inserter(subRoot.yTree));
		// for (int n = i; n < j; n++) {
		// 	subRoot.yTree.emplace(xNodes[n + N].point, xNodes[n + N].value);
		// }
		subRoot.left = left;
		subRoot.right = right;

		return subRootIndex;
	}

	void printTree(int root) {
		if (root == NULL_NODE || root >= xNodes.size()) {
			return;
		}
		// writeF(
		// 	std::cout, "ASY dot(%,L=Label(\"%,%\"));\n", xNodes[root].point,
		// 	xNodes[root].value, xNodes[root].isLeaf ? "*" : "");
		print(root, debug(xNodes[root]), "yTree=(");
		for (auto &elem : yTrees[root])
			print("(", elem.meta->value, elem.left, elem.right, ")");
		printLn(")");
		writeF(
			std::cout, "DOT % %;\n", root,
			xNodes[root].toGraphvizNode(yTrees[root]));

		if (xNodes[root].meta != nullptr) {
			return;
		}

		// writeC(std::cout, xNodes[root].yTree);
		if (xNodes[root].left != NULL_NODE) {
			// writeF(
			// 	std::cout, "ASY draw( % -- %, arrow=Arrow(TeXHead));\n",
			// 	xNodes[root].point, xNodes[xNodes[root].left].point);
		}
		if (xNodes[root].right != NULL_NODE) {
			// writeF(
			// 	std::cout, "ASY draw( % -- %, arrow=Arrow(TeXHead));\n",
			// 	xNodes[root].point, xNodes[xNodes[root].right].point);
		}

		writeF(std::cout, "DOT % -> %;\n", root, xNodes[root].left);
		writeF(std::cout, "DOT % -> %;\n", root, xNodes[root].right);

		printTree(xNodes[root].left);
		printTree(xNodes[root].right);
	}

	int findSplitNode(int x, dataType left, dataType right) {
		printLn(debug(xNodes[x]), left, right);
		printLn(debug(right <= xNodes[x].p.x), debug(left > xNodes[x].p.x));
		while (!xNodes[x].isLeaf &&
			   (right <= xNodes[x].p.x || left > xNodes[x].p.x)) {
			if (right <= xNodes[x].p.x) {
				x = xNodes[x].left;
			}
			else {
				x = xNodes[x].right;
			}
			printLn(
				debug(xNodes[x]), debug(right <= xNodes[x].p.x),
				debug(left > xNodes[x].p.x));
		}
		return x;
	}

	void inRange(
		int x, int yStart, const Range2D<dataType> &range2d,
		std::vector<ValueType> &insides) {
		if (x == NULL_NODE) {
			return;
		}
		if (yStart == NULL_NODE) {
			return;
		}

		if (xNodes[x].meta != nullptr) {
			if (range2d.contains(xNodes[x].meta->point)) {
				insides.emplace_back(xNodes[x].meta->value);
			}
			return;
		}

		if (range2d.rangeX.contains(xNodes[x].range)) {
			for (size_t i = yStart; i < yTrees[x].size(); i++) {
				if (range2d.contains(yTrees[x][i].meta->point)) {
					insides.emplace_back(yTrees[x][i].meta->value);
				}
				else {
					break;
				}
			}
		}
		else if (range2d.rangeX.intersects(xNodes[x].range)) {
			inRange(xNodes[x].left, yTrees[x][yStart].left, range2d, insides);
			inRange(xNodes[x].right, yTrees[x][yStart].right, range2d, insides);
		}
	}

   public:
	RangeTree2D()
		: RangeTree2D(std::vector<Vector2D>(0), std::vector<ValueType>(0)) {}
	RangeTree2D(
		const std::vector<Vector2D> &points,
		const std::vector<ValueType> &values)
		: N(points.size()) {
		if (points.size() != values.size()) {
			throw std::invalid_argument(
				"Size of points and values should be equal");
		}
		if (N == 0) {
			return;
		}

		freeNode = N;

		xNodes.resize(2 * N);
		yTrees.resize(2 * N);
		data.resize(N);

		for (size_t i = 0; i < N; i++) {
			data[i].point = points[i];
			data[i].value = values[i];
			auto &node = xNodes[N + i];
			node.meta = &data[i];
			node.range.start = node.range.end = points[i].x;
			node.yTreeIndex = N + i;
		}
		std::sort(std::next(xNodes.begin(), N), xNodes.end());
		for (size_t i = 0; i < N; i++) {
			yTrees[N + i].emplace_back(xNodes[N + i].meta);
		}

		build2DRangeTree(0, N);
		// printTree(1);
	}

	auto rangeQuery(const Range2D<dataType> &range2d) {
		std::vector<ValueType> insides;
		insides.reserve(N / 4);
		if (N == 0) {
			return insides;
		}
		Meta fakeMeta;
		fakeMeta.point = Vector2D(0, range2d.rangeY.start);
		fakeMeta.value = ValueType();
		auto yStartIter = std::lower_bound(
			yTrees[1].begin(), yTrees[1].end(), YNode{&fakeMeta});
		if (yStartIter == yTrees[1].end()) {
			return insides;
		}

		inRange(
			1, std::distance(yTrees[1].begin(), yStartIter), range2d, insides);
		return insides;
	}
	void debugDraw(std::ostream &out) {}
};

#endif	// RANGE_TREE_2D_HPP
