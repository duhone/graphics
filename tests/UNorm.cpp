#include <3rdParty/doctest.h>

#include "types/UNorm.h"

#include <limits>

namespace {
	template<typename T>
	void TestUNorm() {
		float epsilon = 1.0f / std::numeric_limits<T>::max();

		UNorm<T> unorm{0.5f};
		CHECK(unorm == doctest::Approx(0.5f).epsilon(epsilon));

		unorm = 0.3f;
		CHECK(unorm == doctest::Approx(0.3f).epsilon(epsilon));

		unorm = 0.0f;
		CHECK(unorm == doctest::Approx(0.0f).epsilon(epsilon));

		// always clamped to 0-1
		unorm = -1.2f;
		CHECK(unorm == doctest::Approx(0.0f).epsilon(epsilon));

		unorm = 1.2f;
		CHECK(unorm == doctest::Approx(1.0f).epsilon(epsilon));

		UNorm<T> unorm2 = unorm;
		CHECK(unorm == unorm);
	}

	template<typename T>
	void TestUNorm2() {
		float epsilon = 1.0f / std::numeric_limits<T>::max();

		UNorm2<T> unorm{0.5f, 0.35};
		CHECK(((glm::vec2)unorm).x == doctest::Approx(0.5f).epsilon(epsilon));
		CHECK(((glm::vec2)unorm).y == doctest::Approx(0.35f).epsilon(epsilon));

		unorm = {0.0f, 0.0f};
		CHECK(((glm::vec2)unorm).x == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec2)unorm).y == doctest::Approx(0.0f).epsilon(epsilon));

		// always clamped to 0-1
		unorm = {-1.2f, 1.2f};
		CHECK(((glm::vec2)unorm).x == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec2)unorm).y == doctest::Approx(1.0f).epsilon(epsilon));

		UNorm2<T> unorm2 = unorm;
		CHECK(unorm == unorm);
	}

	template<typename T>
	void TestUNorm3() {
		float epsilon = 1.0f / std::numeric_limits<T>::max();

		UNorm3<T> unorm{0.5f, 0.35, 0.1};
		CHECK(((glm::vec3)unorm).x == doctest::Approx(0.5f).epsilon(epsilon));
		CHECK(((glm::vec3)unorm).y == doctest::Approx(0.35f).epsilon(epsilon));
		CHECK(((glm::vec3)unorm).z == doctest::Approx(0.1f).epsilon(epsilon));

		unorm = {0.0f, 0.0f, 0.0f};
		CHECK(((glm::vec3)unorm).x == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec3)unorm).y == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec3)unorm).z == doctest::Approx(0.0f).epsilon(epsilon));

		// always clamped to 0-1
		unorm = {-1.2f, 1.2f, -0.2f};
		CHECK(((glm::vec3)unorm).x == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec3)unorm).y == doctest::Approx(1.0f).epsilon(epsilon));
		CHECK(((glm::vec3)unorm).z == doctest::Approx(0.0f).epsilon(epsilon));

		UNorm3<T> unorm2 = unorm;
		CHECK(unorm == unorm);
	}

	template<typename T>
	void TestUNorm4() {
		float epsilon = 1.0f / std::numeric_limits<T>::max();

		UNorm4<T> unorm{0.5f, 0.35, 0.1, 0.9f};
		CHECK(((glm::vec4)unorm).x == doctest::Approx(0.5f).epsilon(epsilon));
		CHECK(((glm::vec4)unorm).y == doctest::Approx(0.35f).epsilon(epsilon));
		CHECK(((glm::vec4)unorm).z == doctest::Approx(0.1f).epsilon(epsilon));
		CHECK(((glm::vec4)unorm).w == doctest::Approx(0.9f).epsilon(epsilon));

		unorm = {0.0f, 0.0f, 0.0f, 0.0f};
		CHECK(((glm::vec4)unorm).x == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec4)unorm).y == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec4)unorm).z == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec4)unorm).w == doctest::Approx(0.0f).epsilon(epsilon));

		// always clamped to 0-1
		unorm = {-1.2f, 1.2f, -0.2f, 0.7f};
		CHECK(((glm::vec4)unorm).x == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec4)unorm).y == doctest::Approx(1.0f).epsilon(epsilon));
		CHECK(((glm::vec4)unorm).z == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec4)unorm).w == doctest::Approx(0.7f).epsilon(epsilon));

		UNorm4<T> unorm2 = unorm;
		CHECK(unorm == unorm);
	}
}    // namespace

TEST_CASE("unorm") {
	TestUNorm<uint8_t>();
	TestUNorm<uint16_t>();

	TestUNorm2<uint8_t>();
	TestUNorm2<uint16_t>();

	TestUNorm3<uint8_t>();
	TestUNorm3<uint16_t>();

	TestUNorm4<uint8_t>();
	TestUNorm4<uint16_t>();
}
