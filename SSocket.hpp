#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <type_traits>
#include <cstdint>
#include "common.h"
// SSocket = Simply Socket


#define DEFINE_SHORTCUT_VAL(name)                                                                                      \
	template <typename T>                                                                                              \
	_NV_ALWAYS_INLINE auto name(SOCKET s, const T* addr) {                                                             \
		return ::name(s, (struct sockaddr*)addr, (int)sizeof(T));                                                      \
	}                                                                                                                  \
	template <typename T>                                                                                              \
	_NV_ALWAYS_INLINE auto name(SOCKET s, const T& addr) {                                                             \
		return ::name(s, (struct sockaddr*)&addr, (int)sizeof(T));                                                     \
	}                                                                                                                  \
	template <typename T, typename U>                                                                                  \
	_NV_ALWAYS_INLINE auto name(SOCKET s, const T* addr, U size) {                                                     \
		static_assert(std::is_convertible<U, int>());                                                                  \
		return ::name(s, (struct sockaddr*)addr, (int)size);                                                           \
	}
#define DEFINE_SHORTCUT_PTR(name)                                                                                      \
	template <typename T>                                                                                              \
	_NV_ALWAYS_INLINE auto name(SOCKET s, const T* addr) {                                                             \
		int size = (int)sizeof(T);                                                                                     \
		return ::name(s, (struct sockaddr*)addr, &size);                                                               \
	}                                                                                                                  \
	template <typename T>                                                                                              \
	_NV_ALWAYS_INLINE auto name(SOCKET s, const T& addr) {                                                             \
		int size = (int)sizeof(T);                                                                                     \
		return ::name(s, (struct sockaddr*)&addr, &size);                                                              \
	}                                                                                                                  \
	template <typename T, typename U>                                                                                  \
	_NV_ALWAYS_INLINE auto name(SOCKET s, const T* addr, U size) {                                                     \
		static_assert(std::is_convertible<U, int>());                                                                  \
		int sizeInt = (int)size;                                                                                       \
		return ::name(s, (struct sockaddr*)addr, &sizeInt);                                                            \
	}

_NV_ALWAYS_INLINE auto close(SOCKET s) {
	return ::closesocket(s);
}

_NV_ALWAYS_INLINE auto accept(SOCKET s) {
	return ::accept(s, NULL, NULL);
}

DEFINE_SHORTCUT_VAL(bind)
DEFINE_SHORTCUT_VAL(connect)
DEFINE_SHORTCUT_PTR(accept)

template <typename U, typename T, typename K>
_NV_ALWAYS_INLINE auto recv(SOCKET s, U* buf, T len, K flags) {
	static_assert(std::is_convertible<T, int>() && std::is_convertible<K, int>());
	return ::recv(s, (char*)buf, (int)len, (int)flags);
}

template <typename U, typename T>
_NV_ALWAYS_INLINE auto recv(SOCKET s, U* buf, T len) {
	static_assert(std::is_convertible<T, int>());
	return ::recv(s, (char*)buf, (int)len, (int)0);
}

template <size_t N>
_NV_ALWAYS_INLINE auto recv(SOCKET s, char (&buf)[N]) {
	return ::recv(s, (const char*)buf, (int)N, (int)0);
}

template <typename U, typename T, typename K>
_NV_ALWAYS_INLINE auto send(SOCKET s, const U* buf, T len, K flags) {
	static_assert(std::is_convertible<T, int>() && std::is_convertible<K, int>());
	return ::send(s, (const char*)buf, (int)len, (int)flags);
}

template <typename U, typename T>
_NV_ALWAYS_INLINE auto send(SOCKET s, const U* buf, T len) {
	static_assert(std::is_convertible<T, int>());
	return ::send(s, (const char*)buf, (int)len, (int)0);
}

template <typename T>
_NV_ALWAYS_INLINE auto send(SOCKET s, const std::string& str, T flags) {
	static_assert(std::is_convertible<T, int>());
	return ::send(s, (const char*)str.c_str(), (int)str.size(), (int)flags);
}

_NV_ALWAYS_INLINE auto send(SOCKET s, const std::string& str) {
	return ::send(s, (const char*)str.c_str(), (int)str.size(), 0);
}

template <typename T, typename K>
_NV_ALWAYS_INLINE auto send(SOCKET s, const std::vector<T>& vec, K flags) {
	static_assert(std::is_convertible<K, int>());
	static_assert(std::is_convertible<T, char>());
	return ::send(s, (const char*)vec.data(), (int)vec.size(), (int)flags);
}

template <typename T>
_NV_ALWAYS_INLINE auto send(SOCKET s, const std::vector<T>& vec) {
	static_assert(std::is_convertible<T, char>());
	return ::send(s, (const char*)vec.data(), (int)vec.size(), 0);
}

template <size_t N>
_NV_ALWAYS_INLINE auto send(SOCKET s, char (&buf)[N]) {
	return ::send(s, (const char*)buf, (int)N, (int)0);
}
