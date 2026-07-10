#pragma once

#include <memory>
#include <string>

#define BIT(x) (1 << x)

#define AIO_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#if defined(AIO_WINDOWS)
    #define AIO_DEBUG_BREAK() __debugbreak()
#elif defined(AIO_LINUX)
    #include <signal.h>
    #define AIO_DEBUG_BREAK() raise(SIGTRAP)
#else
    #define AIO_DEBUG_BREAK()
#endif

inline std::wstring StringToWide(std::string_view str)
{
	std::wstring wide_string(str.begin(), str.end());
	return wide_string;
}

inline std::string WideToString(std::wstring_view wstr)
{
	std::string str(wstr.begin(), wstr.end());
	return str;
}

namespace aio
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}