#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <random>

std::random_device rd;
std::mt19937 gen(rd());

// namespace std {
// 	template <typename T>
// 	ostream& operator<<(ostream& os, const vector<T>& in) {
// 		writeContainer(os, in);
// 		return os;
// 	}
// }  // namespace std
