#include <3rdParty/doctest.h>

#include "types/UNorm.h"

#include <limits>

namespace {
	template<typename T>
	void Test() {
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
}    // namespace

TEST_CASE("unorm") {
	Test<uint8_t>();
	Test<uint16_t>();
}
