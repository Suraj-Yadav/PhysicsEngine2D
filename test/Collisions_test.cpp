#include <doctest.h>

#include "TestUtil.hpp"

TYPE_TO_STRING(BruteForceCollision);
TYPE_TO_STRING(BruteForceSATCollision);
TYPE_TO_STRING(IntervalTreeCollision);

TEST_CASE_TEMPLATE(
	"Test Get Collisions", Collision, BruteForceCollision,
	BruteForceSATCollision, IntervalTreeCollision) {
	const size_t length = 1000;
	for (size_t i = 0; i < length; i++) {
		auto particles =
			getRandomParticles({-40, 40, -40, 40}, {1, 2}, {1, 2}, length);
		std::vector<std::reference_wrapper<BaseShape>> objects(
			particles.begin(), particles.end());

		auto collisionsExpected = getCollisionBruteForce(objects);
		auto collisionsGot = Collision::getCollisions(objects);

		std::sort(collisionsGot.begin(), collisionsGot.end());
		std::sort(collisionsExpected.begin(), collisionsExpected.end());

		CAPTURE(collisionsExpected);
		CAPTURE(collisionsGot);

		REQUIRE_EQ(collisionsExpected.size(), collisionsGot.size());
		for (size_t i = 0; i < collisionsGot.size(); i++) {
			REQUIRE_EQ(collisionsExpected[i], collisionsGot[i]);
		}
	}
}