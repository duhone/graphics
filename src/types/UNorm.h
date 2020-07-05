#include <core/Log.h>

#include <limits>
#include <numeric>
#include <type_traits>

template<typename T>
class UNorm {
	friend std::hash<UNorm<T>>;
	static_assert(std::is_unsigned_v<T> && std::is_integral_v<T> && sizeof(T) <= 2,
	              "UNorm template parameter must be an unsiged int, and 16 bits or less");

  public:
	UNorm() noexcept = default;
	UNorm(float a_arg) noexcept {
		a_arg = std::clamp(a_arg, 0.0f, 1.0f);
		a_arg *= s_toT;
		m_data = static_cast<T>(a_arg);
	}
	~UNorm() noexcept            = default;
	UNorm(const UNorm&) noexcept = default;
	UNorm(UNorm&&) noexcept      = default;
	UNorm& operator=(const UNorm&) noexcept = default;
	UNorm& operator=(UNorm&&) noexcept = default;

	operator float() const { return ToFloat(m_data); }
	// spaceship

  private:
	T ToUnorm(float a_arg) const noexcept {
		a_arg = std::clamp(a_arg, 0.0f, 1.0f);
		a_arg *= s_toT;
		return static_cast<T>(a_arg);
	}
	float ToFloat(T a_arg) const noexcept {
		float arg = static_cast<float>(a_arg);
		return arg * s_toFloat;
	}
	inline static constexpr float s_toT     = std::numeric_limits<T>::max();
	inline static constexpr float s_toFloat = 1.0f / std::numeric_limits<T>::max();
	T m_data;
};

namespace std {
	template<typename T>
	struct hash<UNorm<T>> {
		std::size_t operator()(UNorm<T> const& a_arg) const noexcept {
			std::size_t h1 = std::hash<T>{}(a_arg.m_data);
			return h1;
		}
	};
}    // namespace std
