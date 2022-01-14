#include <doctest.h>

#include <PhysicsEngine2D/KdTree.hpp>
#include <PhysicsEngine2D/RangeTree2D.hpp>

#include "TestUtil.hpp"

TYPE_TO_STRING(KdTree<int>);
TYPE_TO_STRING(RangeTree2D<int>);

TEST_CASE_TEMPLATE(
	"Test RangeTree Range Query", Tree, KdTree<int>, RangeTree2D<int>) {
	SUBCASE("Empty Input") {
		std::vector<Vector2D> points(0);
		std::vector<int> values(0);
		Tree tree(points, values);
		Range2D<dataType> range2D(-30, 30, -20, 20);
		auto insidePointsGot = tree.rangeQuery(range2D);
		std::sort(insidePointsGot.begin(), insidePointsGot.end());

		CAPTURE(points);
		CAPTURE(values);
		CAPTURE(range2D);
		CAPTURE(insidePointsGot);
		REQUIRE_EQ(insidePointsGot.size(), 0);
	}

	SUBCASE("Fixed Input") {
		std::vector<Vector2D> points = {
			{0, 0}, {-20, -20}, {-20, 20}, {20, 20}, {20, -20}};
		std::vector<int> values = {0, 1, 2, 3, 4};
		Tree tree(points, values);
		Range2D<dataType> range2D(-30, 10, -30, 10);
		auto insidePointsGot = tree.rangeQuery(range2D);
		std::sort(insidePointsGot.begin(), insidePointsGot.end());
		CAPTURE(points);
		CAPTURE(values);
		CAPTURE(range2D);
		CAPTURE(insidePointsGot.size());
		CAPTURE(insidePointsGot);
		REQUIRE_EQ(insidePointsGot.size(), 2);
		REQUIRE_EQ(insidePointsGot[0], 0);
		REQUIRE_EQ(insidePointsGot[1], 1);
	}

	SUBCASE("Random Inputs") {
		const size_t length = 1000;
		for (size_t i = 0; i < length; i++) {
			auto points = getRandomPoints({-400, 400, -400, 400}, length);
			auto values = getShuffledArrayOf1ToN(length);
			Tree tree(points, values);
			for (size_t i = 0; i < length; i++) {
				auto range2D = getRandom2DRange(
					{-400, 400, -400, 400}, {10, 400, 10, 400});

				auto insidePointsGot = tree.rangeQuery(range2D);
				std::vector<int> insidePointsActual;
				for (size_t j = 0; j < length; j++) {
					if (range2D.contains(points[j])) {
						insidePointsActual.emplace_back(values[j]);
					}
				}

				std::sort(insidePointsGot.begin(), insidePointsGot.end());
				std::sort(insidePointsActual.begin(), insidePointsActual.end());

				CAPTURE(points);
				CAPTURE(values);
				CAPTURE(range2D);
				CAPTURE(insidePointsGot);
				CAPTURE(insidePointsActual);
				REQUIRE_EQ(insidePointsActual.size(), insidePointsGot.size());
				for (size_t i = 0; i < insidePointsActual.size(); i++) {
					REQUIRE_EQ(insidePointsActual[i], insidePointsGot[i]);
				}
			}
		}
	}
}