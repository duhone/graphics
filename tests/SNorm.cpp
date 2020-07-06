#include <3rdParty/doctest.h>

#include "types/SNorm.h"

#include <limits>

namespace {
	template<typename T>
	void TestSNorm() {
		float epsilon = 2.0f / std::numeric_limits<T>::max();

		SNorm<T> snorm{0.5f};
		CHECK(snorm == doctest::Approx(0.5f).epsilon(epsilon));

		snorm = -0.3f;
		CHECK(snorm == doctest::Approx(-0.3f).epsilon(epsilon));

		snorm = 0.0f;
		CHECK(snorm == doctest::Approx(0.0f).epsilon(epsilon));

		// always clamped to -1-1
		snorm = -1.2f;
		CHECK(snorm == doctest::Approx(-1.0f).epsilon(epsilon));

		snorm = 1.2f;
		CHECK(snorm == doctest::Approx(1.0f).epsilon(epsilon));

		SNorm<T> snorm2 = snorm;
		CHECK(snorm2 == snorm);
	}

	template<typename T>
	void TestSNorm2() {
		float epsilon = 2.0f / std::numeric_limits<T>::max();

		SNorm2<T> snorm{0.5f, -0.35};
		CHECK(((glm::vec2)snorm).x == doctest::Approx(0.5f).epsilon(epsilon));
		CHECK(((glm::vec2)snorm).y == doctest::Approx(-0.35f).epsilon(epsilon));

		snorm = {0.0f, 0.0f};
		CHECK(((glm::vec2)snorm).x == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec2)snorm).y == doctest::Approx(0.0f).epsilon(epsilon));

		// always clamped to -1-1
		snorm = {-1.2f, 1.2f};
		CHECK(((glm::vec2)snorm).x == doctest::Approx(-1.0f).epsilon(epsilon));
		CHECK(((glm::vec2)snorm).y == doctest::Approx(1.0f).epsilon(epsilon));

		SNorm2<T> snorm2 = snorm;
		CHECK(snorm == snorm2);
	}

	template<typename T>
	void TestSNorm3() {
		float epsilon = 2.0f / std::numeric_limits<T>::max();

		SNorm3<T> snorm{0.5f, -0.35, 0.1};
		CHECK(((glm::vec3)snorm).x == doctest::Approx(0.5f).epsilon(epsilon));
		CHECK(((glm::vec3)snorm).y == doctest::Approx(-0.35f).epsilon(epsilon));
		CHECK(((glm::vec3)snorm).z == doctest::Approx(0.1f).epsilon(epsilon));

		snorm = {0.0f, 0.0f, 0.0f};
		CHECK(((glm::vec3)snorm).x == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec3)snorm).y == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec3)snorm).z == doctest::Approx(0.0f).epsilon(epsilon));

		// always clamped to -1-1
		snorm = {-1.2f, 1.2f, -0.2f};
		CHECK(((glm::vec3)snorm).x == doctest::Approx(-1.0f).epsilon(epsilon));
		CHECK(((glm::vec3)snorm).y == doctest::Approx(1.0f).epsilon(epsilon));
		CHECK(((glm::vec3)snorm).z == doctest::Approx(-0.2f).epsilon(epsilon));

		SNorm3<T> snorm2 = snorm;
		CHECK(snorm == snorm2);
	}

	template<typename T>
	void TestSNorm4() {
		float epsilon = 2.0f / std::numeric_limits<T>::max();

		SNorm4<T> snorm{0.5f, -0.35, 0.1, -0.9f};
		CHECK(((glm::vec4)snorm).x == doctest::Approx(0.5f).epsilon(epsilon));
		CHECK(((glm::vec4)snorm).y == doctest::Approx(-0.35f).epsilon(epsilon));
		CHECK(((glm::vec4)snorm).z == doctest::Approx(0.1f).epsilon(epsilon));
		CHECK(((glm::vec4)snorm).w == doctest::Approx(-0.9f).epsilon(epsilon));

		snorm = {0.0f, 0.0f, 0.0f, 0.0f};
		CHECK(((glm::vec4)snorm).x == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec4)snorm).y == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec4)snorm).z == doctest::Approx(0.0f).epsilon(epsilon));
		CHECK(((glm::vec4)snorm).w == doctest::Approx(0.0f).epsilon(epsilon));

		// always clamped to 0-1
		snorm = {-1.2f, 1.2f, -0.2f, 0.7f};
		CHECK(((glm::vec4)snorm).x == doctest::Approx(-1.0f).epsilon(epsilon));
		CHECK(((glm::vec4)snorm).y == doctest::Approx(1.0f).epsilon(epsilon));
		CHECK(((glm::vec4)snorm).z == doctest::Approx(-0.2f).epsilon(epsilon));
		CHECK(((glm::vec4)snorm).w == doctest::Approx(0.7f).epsilon(epsilon));

		SNorm4<T> snorm2 = snorm;
		CHECK(snorm == snorm2);
	}
}    // namespace

TEST_CASE("snorm") {
	TestSNorm<int8_t>();
	TestSNorm<int16_t>();

	TestSNorm2<int8_t>();
	TestSNorm2<int16_t>();

	TestSNorm3<int8_t>();
	TestSNorm3<int16_t>();

	TestSNorm4<int8_t>();
	TestSNorm4<int16_t>();
}
