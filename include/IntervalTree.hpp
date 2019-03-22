#include <limits>
#include <list>
#include <tuple>
#include <vector>

template <class Type>
struct Interval {
	Type low, high;
	Interval()
		: low(std::numeric_limits<Type>::max()),
		  high(std::numeric_limits<Type>::lowest()) {}
	Interval(Type start, Type end)
		: low(start), high(end) {}
};

template <typename T>
bool operator<(const Interval<T> &x, const Interval<T> &y) {
	return std::tie(x.low, x.high) < std::tie(y.low, y.high);
}

template <class KeyType, class ValueType>
class AVL {
	struct Node {
		Interval<KeyType> range;
		KeyType maxEnd;
		int height, balance;
		Node *left, *right;
		ValueType value;
		Node(const Interval<KeyType> &interval, ValueType val)
			: range(interval), maxEnd(interval.high), height(0), balance(0), left(nullptr), right(nullptr), value(val) {}
	};
	Node *root;
	static void updateBalance(Node *x) {
		x->balance = 0;
		if (x->left)
			x->balance += 1 + x->left->height;
		if (x->right)
			x->balance -= 1 + x->right->height;
		x->height = -1;
		if (x->left)
			x->height = std::max(x->left->height, x->height);
		if (x->right)
			x->height = std::max(x->right->height, x->height);
		++x->height;
		x->maxEnd = x->range.high;
		if (x->left)
			x->maxEnd = std::max(x->left->maxEnd, x->maxEnd);
		if (x->right)
			x->maxEnd = std::max(x->right->maxEnd, x->maxEnd);
	}
	Node *insert(Node *x, const Interval<KeyType> &range, const ValueType &value) {
		if (x == nullptr)
			return new Node(range, value);
		if (std::tie(range, value) < std::tie(x->range, x->value))
			x->left = insert(x->left, range, value);
		else
			x->right = insert(x->right, range, value);
		updateBalance(x);
		if (x->balance < -1 || x->balance > 1)
			x = rebalance(x);
		return x;
	}
	Node *search(Node *x, KeyType start) {
		if (x == nullptr)
			return nullptr;
		if (x->range.low > start)
			return search(x->left, start);
		else if (x->range.low == start)
			return x;
		else
			return search(x->right, start);
	}
	Node *removeMin(Node *x) {
		Node *returnValue;
		if (x->left == nullptr) {
			returnValue = x->right;
			delete x;
		}
		else {
			x->left = removeMin(x->left);
			updateBalance(x);
			if (x->balance < -1 || x->balance > 1)
				x = rebalance(x);
			returnValue = x;
		}
		return returnValue;
	}
	Node *minimum(Node *x) {
		while (x->left) {
			x = x->left;
		}
		return x;
	}
	Node *remove(Node *x, const Interval<KeyType> &range, const ValueType &value) {
		if (x == nullptr)
			return nullptr;
		if (std::tie(range, value) < std::tie(x->range, x->value))
			x->left = remove(x->left, range, value);
		else if (std::tie(x->range, x->value) < std::tie(range, value))
			x->right = remove(x->right, range, value);
		else {
			if (x->right == nullptr) {
				Node *temp = x->left;
				delete x;
				return temp;
			}
			else if (x->left == nullptr) {
				Node *temp = x->right;
				delete x;
				return temp;
			}
			else {
				Node *temp = x;
				x = new Node(*minimum(temp->right));
				x->right = removeMin(temp->right);
				x->left = temp->left;
			}
		}
		updateBalance(x);
		if (x->balance < -1 || x->balance > 1)
			x = rebalance(x);
		return x;
	}
	void inorder(Node *x, std::list<ValueType> &list) {
		if (x == nullptr) return;
		inorder(x->left, list);
		// print(x->value);
		list.push_back(x->value);
		inorder(x->right, list);
	}
	Node *rotateLeft(Node *h) {
		Node *x = h->right;
		h->right = x->left;
		x->left = h;
		updateBalance(h);
		updateBalance(x);
		return x;
	}
	Node *rotateRight(Node *h) {
		Node *x = h->left;
		h->left = x->right;
		x->right = h;
		updateBalance(h);
		updateBalance(x);
		return x;
	}
	Node *rebalance(Node *node) {
		if (node->balance < 0) {
			if (node->right->balance > 0)
				node->right = rotateRight(node->right);
			return rotateLeft(node);
		}
		else if (node->balance > 0) {
			if (node->left->balance < 0)
				node->left = rotateLeft(node->left);
			return rotateRight(node);
		}
		return node;
	}
	static bool intersects(Node *x, KeyType low, KeyType high) {
		if (high < x->range.low)
			return false;
		if (x->maxEnd < low)
			return false;
		return true;
	}
	bool searchAll(Node *x, KeyType low, KeyType high, std::vector<ValueType> &list) {
		bool found1 = false;
		bool found2 = false;
		bool found3 = false;
		if (x == nullptr)
			return false;
		if (intersects(x, low, high)) {
			list.push_back(x->value);
			found1 = true;
		}
		if (x->left != nullptr && x->left->maxEnd >= low)
			found2 = searchAll(x->left, low, high, list);
		if (found2 || x->left == nullptr || x->left->maxEnd < low)
			found3 = searchAll(x->right, low, high, list);
		return found1 || found2 || found3;
	}

   public:
	AVL() {
		root = nullptr;
	}
	~AVL() {
		delete root;
	}
	inline void insert(KeyType low, KeyType high, ValueType value) {
		root = insert(root, {low, high}, value);
	}
	inline void removeMin() {
		root = removeMin(root);
	}
	inline void remove(KeyType low, KeyType high, ValueType value) {
		root = remove(root, {low, high}, value);
	}
	std::list<ValueType> getInorder() {
		std::list<ValueType> list;
		inorder(root, list);
		return list;
	}
	inline std::vector<ValueType> searchAll(KeyType low, KeyType high) {
		std::vector<ValueType> list;
		searchAll(root, low, high, list);
		return list;
	}
};
