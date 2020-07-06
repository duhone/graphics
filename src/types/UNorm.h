#include <3rdParty/glm.h>

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
	UNorm(float a_arg) noexcept { m_data = ToUnorm(a_arg); }
	~UNorm() noexcept            = default;
	UNorm(const UNorm&) noexcept = default;
	UNorm(UNorm&&) noexcept      = default;
	UNorm& operator=(const UNorm&) noexcept = default;
	UNorm& operator=(UNorm&&) noexcept = default;

	operator float() const { return ToFloat(m_data); }
	// spaceship

	inline friend bool operator==(const UNorm<T>& a_arg1, const UNorm<T>& a_arg2) {
		return a_arg1.m_data == a_arg2.m_data;
	}

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

template<typename T>
class UNorm2 {
	friend std::hash<UNorm2<T>>;
	static_assert(std::is_unsigned_v<T> && std::is_integral_v<T> && sizeof(T) <= 2,
	              "UNorm template parameter must be an unsiged int, and 16 bits or less");

  public:
	UNorm2() noexcept = default;
	UNorm2(glm::vec2 a_arg) noexcept {
		m_data[0] = a_arg[0];
		m_data[1] = a_arg[1];
	}
	UNorm2(float a_arg1, float a_arg2) noexcept {
		m_data[0] = a_arg1;
		m_data[1] = a_arg2;
	}
	~UNorm2() noexcept             = default;
	UNorm2(const UNorm2&) noexcept = default;
	UNorm2(UNorm2&&) noexcept      = default;
	UNorm2& operator=(const UNorm2&) noexcept = default;
	UNorm2& operator=(UNorm2&&) noexcept = default;

	operator glm::vec2() const { return {m_data[0], m_data[1]}; }
	// spaceship

	inline friend bool operator==(const UNorm2<T>& a_arg1, const UNorm2<T>& a_arg2) {
		return a_arg1.m_data[0] == a_arg2.m_data[0] && a_arg1.m_data[1] == a_arg2.m_data[1];
	}

  private:
	UNorm<T> m_data[2];
};

template<typename T>
class UNorm3 {
	friend std::hash<UNorm3<T>>;
	static_assert(std::is_unsigned_v<T> && std::is_integral_v<T> && sizeof(T) <= 2,
	              "UNorm template parameter must be an unsiged int, and 16 bits or less");

  public:
	UNorm3() noexcept = default;
	UNorm3(glm::vec3 a_arg) noexcept {
		m_data[0] = a_arg[0];
		m_data[1] = a_arg[1];
		m_data[2] = a_arg[2];
	}
	UNorm3(float a_arg1, float a_arg2, float a_arg3) noexcept {
		m_data[0] = a_arg1;
		m_data[1] = a_arg2;
		m_data[2] = a_arg3;
	}
	~UNorm3() noexcept             = default;
	UNorm3(const UNorm3&) noexcept = default;
	UNorm3(UNorm3&&) noexcept      = default;
	UNorm3& operator=(const UNorm3&) noexcept = default;
	UNorm3& operator=(UNorm3&&) noexcept = default;

	operator glm::vec3() const { return {m_data[0], m_data[1], m_data[2]}; }
	// spaceship

	inline friend bool operator==(const UNorm3<T>& a_arg1, const UNorm3<T>& a_arg2) {
		return a_arg1.m_data[0] == a_arg2.m_data[0] && a_arg1.m_data[1] == a_arg2.m_data[1] &&
		       a_arg1.m_data[2] == a_arg2.m_data[2];
	}

  private:
	UNorm<T> m_data[3];
};

template<typename T>
class UNorm4 {
	friend std::hash<UNorm4<T>>;
	static_assert(std::is_unsigned_v<T> && std::is_integral_v<T> && sizeof(T) <= 2,
	              "UNorm template parameter must be an unsiged int, and 16 bits or less");

  public:
	UNorm4() noexcept = default;
	UNorm4(glm::vec4 a_arg) noexcept {
		m_data[0] = a_arg[0];
		m_data[1] = a_arg[1];
		m_data[2] = a_arg[2];
		m_data[3] = a_arg[3];
	}
	UNorm4(float a_arg1, float a_arg2, float a_arg3, float a_arg4) noexcept {
		m_data[0] = a_arg1;
		m_data[1] = a_arg2;
		m_data[2] = a_arg3;
		m_data[3] = a_arg4;
	}
	~UNorm4() noexcept             = default;
	UNorm4(const UNorm4&) noexcept = default;
	UNorm4(UNorm4&&) noexcept      = default;
	UNorm4& operator=(const UNorm4&) noexcept = default;
	UNorm4& operator=(UNorm4&&) noexcept = default;

	operator glm::vec4() const { return {m_data[0], m_data[1], m_data[2], m_data[3]}; }
	// spaceship

	inline friend bool operator==(const UNorm4<T>& a_arg1, const UNorm4<T>& a_arg2) {
		return a_arg1.m_data[0] == a_arg2.m_data[0] && a_arg1.m_data[1] == a_arg2.m_data[1] &&
		       a_arg1.m_data[2] == a_arg2.m_data[2] && a_arg1.m_data[3] == a_arg2.m_data[3];
	}

  private:
	UNorm<T> m_data[4];
};

namespace std {
	template<typename T>
	struct hash<UNorm<T>> {
		std::size_t operator()(UNorm<T> const& a_arg) const noexcept { return std::hash<T>{}(a_arg.m_data); }
	};
	template<typename T>
	struct hash<UNorm2<T>> {
		std::size_t operator()(UNorm2<T> const& a_arg) const noexcept {
			return std::hash<T>{}(a_arg.m_data[0]) ^ (std::hash<T>{}(a_arg.m_data[1]) << 1);
		}
	};
	template<typename T>
	struct hash<UNorm3<T>> {
		std::size_t operator()(UNorm3<T> const& a_arg) const noexcept {
			return std::hash<T>{}(a_arg.m_data[0]) ^ (std::hash<T>{}(a_arg.m_data[1]) << 1) ^
			       (std::hash<T>{}(a_arg.m_data[2]) << 2);
		}
	};
	template<typename T>
	struct hash<UNorm4<T>> {
		std::size_t operator()(UNorm4<T> const& a_arg) const noexcept {
			return std::hash<T>{}(a_arg.m_data[0]) ^ (std::hash<T>{}(a_arg.m_data[1]) << 1) ^
			       (std::hash<T>{}(a_arg.m_data[2]) << 2) ^ (std::hash<T>{}(a_arg.m_data[3]) << 3);
		}
	};
}    // namespace std
