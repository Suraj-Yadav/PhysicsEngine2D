#include <cassert>
#include <limits>
#include <list>
#include <tuple>
#include <vector>

#include "util.hpp"

#define comparePair(a1, a2, b1, b2) ((a1 < b1) || (!(b1 < a1) && a2 < b2))

template <class Type> struct Interval {
	Type low, high;
	inline Interval()
		: low(std::numeric_limits<Type>::max()),
		  high(std::numeric_limits<Type>::lowest()) {}
	inline Interval(Type start, Type end) : low(start), high(end) {}

	friend std::ostream &operator<<(std::ostream &out, Interval const &b) {
		return out << "Interval (" << b.low << "," << b.high << ")";
	}
};

template <typename T>
bool operator<(const Interval<T> &x, const Interval<T> &y) {
	return comparePair(x.low, x.high, y.low, y.high);
}

template <class KeyType, class ValueType> class AVL {
	static const int NULL_NODE = -1;
	struct Node {
		Interval<KeyType> range;
		KeyType maxEnd;
		int height, balance;
		int left, right;
		ValueType value;
		inline Node(const Interval<KeyType> &interval, ValueType val) {
			init(interval, val);
		}

		inline void init(const Interval<KeyType> &interval, ValueType val) {
			range = interval;
			maxEnd = interval.high;
			height = 0;
			balance = 0;
			left = NULL_NODE;
			right = NULL_NODE;
			value = val;
		}

		friend std::ostream &operator<<(std::ostream &out, Node const &b) {
			return out << "Node ["
					   << "range:" << b.range << ","
					   << "value:" << b.value << "]";
		}
	};

	std::vector<Node> nodes;
	std::vector<int> freeMemorySlots;

	int newNode(const Interval<KeyType> &interval, ValueType val) {
		if (freeMemorySlots.size() == 0) {
			const int newSlot = nodes.size();
			nodes.emplace_back(interval, val);
			// printLn("newNode From heap", debug(newSlot));
			return newSlot;
		}
		const int newSlot = freeMemorySlots.back();
		nodes[newSlot].init(interval, val);
		freeMemorySlots.pop_back();
		// printLn("newNode From cache", debug(newSlot));
		return newSlot;
	}

	int newNode(int index) { return index; }

	void deleteNode(int index) {
		// printLn("deleteNode", debug(index));
		freeMemorySlots.push_back(index);
	}

	int root;
	void updateBalance(int x) {
		nodes[x].balance = 0;
		if (nodes[x].left != NULL_NODE)
			nodes[x].balance += 1 + nodes[nodes[x].left].height;
		if (nodes[x].right != NULL_NODE)
			nodes[x].balance -= 1 + nodes[nodes[x].right].height;
		nodes[x].height = -1;
		if (nodes[x].left != NULL_NODE)
			nodes[x].height =
				std::max(nodes[nodes[x].left].height, nodes[x].height);
		if (nodes[x].right != NULL_NODE)
			nodes[x].height =
				std::max(nodes[nodes[x].right].height, nodes[x].height);
		++nodes[x].height;
		nodes[x].maxEnd = nodes[x].range.high;
		if (nodes[x].left != NULL_NODE)
			nodes[x].maxEnd =
				std::max(nodes[nodes[x].left].maxEnd, nodes[x].maxEnd);
		if (nodes[x].right != NULL_NODE)
			nodes[x].maxEnd =
				std::max(nodes[nodes[x].right].maxEnd, nodes[x].maxEnd);
	}
	int insert(int x, const Interval<KeyType> &range, const ValueType &value) {
		if (x == NULL_NODE) return newNode(range, value);
		if (comparePair(range, value, nodes[x].range, nodes[x].value))
			nodes[x].left = insert(nodes[x].left, range, value);
		else
			nodes[x].right = insert(nodes[x].right, range, value);
		updateBalance(x);
		if (nodes[x].balance < -1 || nodes[x].balance > 1) x = rebalance(x);
		return x;
	}
	int search(int x, KeyType start) {
		if (x == NULL_NODE) return NULL_NODE;
		if (nodes[x].range.low > start)
			return search(nodes[x].left, start);
		else if (nodes[x].range.low == start)
			return x;
		else
			return search(nodes[x].right, start);
	}
	int removeMin(int x) {
		int returnValue;
		if (nodes[x].left == NULL_NODE) {
			returnValue = nodes[x].right;
			// deleteNode(x);
		}
		else {
			nodes[x].left = removeMin(nodes[x].left);
			updateBalance(x);
			if (nodes[x].balance < -1 || nodes[x].balance > 1) x = rebalance(x);
			returnValue = x;
		}
		return returnValue;
	}
	int minimum(int x) {
		while (nodes[x].left != NULL_NODE) {
			x = nodes[x].left;
		}
		return x;
	}
	int remove(int x, const Interval<KeyType> &range, const ValueType &value) {
		// printLn(debug(x), debug(range), debug(value));
		if (x == NULL_NODE) return NULL_NODE;
		if (comparePair(range, value, nodes[x].range, nodes[x].value))
			nodes[x].left = remove(nodes[x].left, range, value);
		else if (comparePair(nodes[x].range, nodes[x].value, range, value))
			nodes[x].right = remove(nodes[x].right, range, value);
		else {
			if (nodes[x].right == NULL_NODE) {
				int temp = nodes[x].left;
				deleteNode(x);
				return temp;
			}
			else if (nodes[x].left == NULL_NODE) {
				int temp = nodes[x].right;
				deleteNode(x);
				return temp;
			}
			else {
				int temp = x;
				x = newNode(minimum(nodes[x].right));
				nodes[x].right = removeMin(nodes[temp].right);
				nodes[x].left = nodes[temp].left;
				deleteNode(temp);
			}
		}
		updateBalance(x);
		if (nodes[x].balance < -1 || nodes[x].balance > 1) x = rebalance(x);
		return x;
	}
	void inOrder(int x, std::list<ValueType> &list) {
		if (x == NULL_NODE) return;
		inOrder(nodes[x].left, list);
		// print(x->value);
		list.emplace_back(nodes[x].value);
		inOrder(nodes[x].right, list);
	}
	int rotateLeft(int h) {
		int x = nodes[h].right;
		nodes[h].right = nodes[x].left;
		nodes[x].left = h;
		updateBalance(h);
		updateBalance(x);
		return x;
	}
	int rotateRight(int h) {
		int x = nodes[h].left;
		nodes[h].left = nodes[x].right;
		nodes[x].right = h;
		updateBalance(h);
		updateBalance(x);
		return x;
	}
	int rebalance(int node) {
		if (nodes[node].balance < 0) {
			if (nodes[nodes[node].right].balance > 0)
				nodes[node].right = rotateRight(nodes[node].right);
			return rotateLeft(node);
		}
		else if (nodes[node].balance > 0) {
			if (nodes[nodes[node].left].balance < 0)
				nodes[node].left = rotateLeft(nodes[node].left);
			return rotateRight(node);
		}
		return node;
	}
	bool intersects(int x, KeyType low, KeyType high) {
		if (high < nodes[x].range.low) return false;
		if (nodes[x].maxEnd < low) return false;
		return true;
	}
	bool searchAll(
		int x, KeyType low, KeyType high, std::vector<ValueType> &list) {
		bool found1 = false;
		bool found2 = false;
		bool found3 = false;
		if (x == NULL_NODE) return false;
		if (intersects(x, low, high)) {
			// printLn(debug(nodes[x].value), debug(list.size()));
			list.emplace_back(nodes[x].value);
			found1 = true;
		}
		if (nodes[x].left != NULL_NODE && nodes[nodes[x].left].maxEnd >= low)
			found2 = searchAll(nodes[x].left, low, high, list);
		if (found2 || nodes[x].left == NULL_NODE ||
			nodes[nodes[x].left].maxEnd < low)
			found3 = searchAll(nodes[x].right, low, high, list);
		return found1 || found2 || found3;
	}

   public:
	AVL() { root = NULL_NODE; }
	~AVL() {
		assert(nodes.size() == freeMemorySlots.size());
		// if (root) {
		// 	deleteNode(root);
		// }
	}
	inline void insert(KeyType low, KeyType high, ValueType value) {
		// printLn("insert", debug(low), debug(high), debug(value));
		root = insert(root, {low, high}, value);
	}
	inline void removeMin() { root = removeMin(root); }
	inline void remove(KeyType low, KeyType high, ValueType value) {
		// printLn("remove", debug(low), debug(high), debug(value));

		root = remove(root, {low, high}, value);
	}
	std::list<ValueType> getInOrder() {
		std::list<ValueType> list;
		inOrder(root, list);
		return list;
	}
	inline std::vector<ValueType> searchAll(KeyType low, KeyType high) {
		std::vector<ValueType> list;
		searchAll(root, low, high, list);
		return list;
	}

	void printTree(int x = -2, int indent = 0) {
		if (x == -2) {
			printLn("+++++++++++++++++++++++++++++++++++++++++++++++++++++++");
			x = root;
		}

		if (x != NULL_NODE && x < nodes.size()) {
			if (nodes[x].left != NULL_NODE) {
				printTree(nodes[x].left, indent + 1);
			}
			for (int i = 0; i < indent; i++) {
				print('\t');
			}
			printLn(x, nodes[x]);
			if (nodes[x].right != NULL_NODE) {
				printTree(nodes[x].right, indent + 1);
			}
		}
	}

	void reserve(size_t size) { nodes.reserve(size); }
};
