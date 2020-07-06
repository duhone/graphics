#include <3rdParty/glm.h>

#include <limits>
#include <numeric>
#include <type_traits>

template<typename T>
class SNorm {
	friend std::hash<SNorm<T>>;
	static_assert(std::is_signed_v<T> && std::is_integral_v<T> && sizeof(T) <= 2,
	              "UNorm template parameter must be an signed int, and 16 bits or less");

  public:
	SNorm() noexcept = default;
	SNorm(float a_arg) noexcept { m_data = ToSnorm(a_arg); }
	~SNorm() noexcept            = default;
	SNorm(const SNorm&) noexcept = default;
	SNorm(SNorm&&) noexcept      = default;
	SNorm& operator=(const SNorm&) noexcept = default;
	SNorm& operator=(SNorm&&) noexcept = default;

	operator float() const { return ToFloat(m_data); }
	// spaceship

	inline friend bool operator==(const SNorm<T>& a_arg1, const SNorm<T>& a_arg2) {
		return a_arg1.m_data == a_arg2.m_data;
	}

  private:
	T ToSnorm(float a_arg) const noexcept {
		a_arg = std::clamp(a_arg, -1.0f, 1.0f);
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
class SNorm2 {
	friend std::hash<SNorm2<T>>;
	static_assert(std::is_signed_v<T> && std::is_integral_v<T> && sizeof(T) <= 2,
	              "UNorm template parameter must be an unsiged int, and 16 bits or less");

  public:
	SNorm2() noexcept = default;
	SNorm2(glm::vec2 a_arg) noexcept {
		m_data[0] = a_arg[0];
		m_data[1] = a_arg[1];
	}
	SNorm2(float a_arg1, float a_arg2) noexcept {
		m_data[0] = a_arg1;
		m_data[1] = a_arg2;
	}
	~SNorm2() noexcept             = default;
	SNorm2(const SNorm2&) noexcept = default;
	SNorm2(SNorm2&&) noexcept      = default;
	SNorm2& operator=(const SNorm2&) noexcept = default;
	SNorm2& operator=(SNorm2&&) noexcept = default;

	operator glm::vec2() const { return {m_data[0], m_data[1]}; }
	// spaceship

	inline friend bool operator==(const SNorm2<T>& a_arg1, const SNorm2<T>& a_arg2) {
		return a_arg1.m_data[0] == a_arg2.m_data[0] && a_arg1.m_data[1] == a_arg2.m_data[1];
	}

  private:
	SNorm<T> m_data[2];
};

template<typename T>
class SNorm3 {
	friend std::hash<SNorm3<T>>;
	static_assert(std::is_signed_v<T> && std::is_integral_v<T> && sizeof(T) <= 2,
	              "SNorm template parameter must be an unsiged int, and 16 bits or less");

  public:
	SNorm3() noexcept = default;
	SNorm3(glm::vec3 a_arg) noexcept {
		m_data[0] = a_arg[0];
		m_data[1] = a_arg[1];
		m_data[2] = a_arg[2];
	}
	SNorm3(float a_arg1, float a_arg2, float a_arg3) noexcept {
		m_data[0] = a_arg1;
		m_data[1] = a_arg2;
		m_data[2] = a_arg3;
	}
	~SNorm3() noexcept             = default;
	SNorm3(const SNorm3&) noexcept = default;
	SNorm3(SNorm3&&) noexcept      = default;
	SNorm3& operator=(const SNorm3&) noexcept = default;
	SNorm3& operator=(SNorm3&&) noexcept = default;

	operator glm::vec3() const { return {m_data[0], m_data[1], m_data[2]}; }
	// spaceship

	inline friend bool operator==(const SNorm3<T>& a_arg1, const SNorm3<T>& a_arg2) {
		return a_arg1.m_data[0] == a_arg2.m_data[0] && a_arg1.m_data[1] == a_arg2.m_data[1] &&
		       a_arg1.m_data[2] == a_arg2.m_data[2];
	}

  private:
	SNorm<T> m_data[3];
};

template<typename T>
class SNorm4 {
	friend std::hash<SNorm4<T>>;
	static_assert(std::is_signed_v<T> && std::is_integral_v<T> && sizeof(T) <= 2,
	              "SNorm4 template parameter must be an signed int, and 16 bits or less");

  public:
	SNorm4() noexcept = default;
	SNorm4(glm::vec4 a_arg) noexcept {
		m_data[0] = a_arg[0];
		m_data[1] = a_arg[1];
		m_data[2] = a_arg[2];
		m_data[3] = a_arg[3];
	}
	SNorm4(float a_arg1, float a_arg2, float a_arg3, float a_arg4) noexcept {
		m_data[0] = a_arg1;
		m_data[1] = a_arg2;
		m_data[2] = a_arg3;
		m_data[3] = a_arg4;
	}
	~SNorm4() noexcept             = default;
	SNorm4(const SNorm4&) noexcept = default;
	SNorm4(SNorm4&&) noexcept      = default;
	SNorm4& operator=(const SNorm4&) noexcept = default;
	SNorm4& operator=(SNorm4&&) noexcept = default;

	operator glm::vec4() const { return {m_data[0], m_data[1], m_data[2], m_data[3]}; }
	// spaceship

	inline friend bool operator==(const SNorm4<T>& a_arg1, const SNorm4<T>& a_arg2) {
		return a_arg1.m_data[0] == a_arg2.m_data[0] && a_arg1.m_data[1] == a_arg2.m_data[1] &&
		       a_arg1.m_data[2] == a_arg2.m_data[2] && a_arg1.m_data[3] == a_arg2.m_data[3];
	}

  private:
	SNorm<T> m_data[4];
};

namespace std {
	template<typename T>
	struct hash<SNorm<T>> {
		std::size_t operator()(SNorm<T> const& a_arg) const noexcept { return std::hash<T>{}(a_arg.m_data); }
	};
	template<typename T>
	struct hash<SNorm2<T>> {
		std::size_t operator()(SNorm2<T> const& a_arg) const noexcept {
			return std::hash<T>{}(a_arg.m_data[0]) ^ (std::hash<T>{}(a_arg.m_data[1]) << 1);
		}
	};
	template<typename T>
	struct hash<SNorm3<T>> {
		std::size_t operator()(SNorm3<T> const& a_arg) const noexcept {
			return std::hash<T>{}(a_arg.m_data[0]) ^ (std::hash<T>{}(a_arg.m_data[1]) << 1) ^
			       (std::hash<T>{}(a_arg.m_data[2]) << 2);
		}
	};
	template<typename T>
	struct hash<SNorm4<T>> {
		std::size_t operator()(SNorm4<T> const& a_arg) const noexcept {
			return std::hash<T>{}(a_arg.m_data[0]) ^ (std::hash<T>{}(a_arg.m_data[1]) << 1) ^
			       (std::hash<T>{}(a_arg.m_data[2]) << 2) ^ (std::hash<T>{}(a_arg.m_data[3]) << 3);
		}
	};
}    // namespace std
