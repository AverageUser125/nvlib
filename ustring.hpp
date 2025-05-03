#pragma once
#include "common.h"
#include <string>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <cassert>

namespace nv
{

#if 1
#define _NV_USTRING_SIZE 16

#define _NV_USTRING_DATA_SIZE (_NV_USTRING_SIZE - sizeof(uint32_t))
#define _NV_USTRING_PREFIX_SIZE _NV_USTRING_DATA_SIZE - _NV_PTR_SIZE
#else
#define _NV_USTRING_PREFIX_SIZE 4

#define _NV_USTRING_DATA_SIZE (_NV_USTRING_PREFIX_SIZE + _NV_PTR_SIZE)
#define _NV_USTRING_SIZE (_NV_USTRING_DATA_SIZE + sizeof(uint32_t))
#endif

class _NV_PACKED ustring {
	uint32_t sz;

	union {
		struct {
			char prefix[_NV_USTRING_PREFIX_SIZE];
			char* ptr;
		};

		char buf[_NV_USTRING_DATA_SIZE];
	} Data;

  public:
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using value_type = char;
	using pointer = const char*;
	using const_pointer = const char*;
	using reference = const char&;
	using const_reference = const char&;

	class _NV_NODISCARD ustring_iterator {
		const char* ptr;

	  public:
#if _NV_HAS_CXX20
		using iterator_category = std::contiguous_iterator_tag;
#else
		using iterator_category = std::random_access_iterator_tag;
#endif
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using value_type = char;
		using pointer = char*;
		using const_pointer = const char*;
		using reference = char&;
		using const_reference = const char&;

		_NV_ALWAYS_INLINE ustring_iterator(const char* ptr) noexcept : ptr(ptr) {
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE const_reference operator*() const noexcept {
			return *ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE const_pointer operator->() const noexcept {
			return ptr;
		}

		_NV_ALWAYS_INLINE ustring_iterator& operator++() noexcept {
			++ptr;
			return *this;
		}

		_NV_ALWAYS_INLINE ustring_iterator operator++(int) noexcept {
			ustring_iterator temp = *this;
			++ptr;
			return temp;
		}

		_NV_ALWAYS_INLINE ustring_iterator& operator--() noexcept {
			--ptr;
			return *this;
		}

		_NV_ALWAYS_INLINE ustring_iterator operator--(int) noexcept {
			ustring_iterator temp = *this;
			--ptr;
			return temp;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE const_reference operator[](difference_type n) const noexcept {
			return ptr[n];
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE ustring_iterator operator+(difference_type n) const noexcept {
			return ustring_iterator(ptr + n);
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE ustring_iterator operator-(difference_type n) const noexcept {
			return ustring_iterator(ptr - n);
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator==(const ustring_iterator& other) const noexcept {
			return ptr == other.ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator!=(const ustring_iterator& other) const noexcept {
			return ptr != other.ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator<(const ustring_iterator& other) const noexcept {
			return ptr < other.ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator>(const ustring_iterator& other) const noexcept {
			return ptr > other.ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator<=(const ustring_iterator& other) const noexcept {
			return ptr <= other.ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator>=(const ustring_iterator& other) const noexcept {
			return ptr >= other.ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE difference_type operator-(const ustring_iterator& other) const noexcept {
			return ptr - other.ptr;
		}

		_NV_ALWAYS_INLINE ustring_iterator& operator+=(difference_type n) noexcept {
			ptr += n;
			return *this;
		}

		_NV_ALWAYS_INLINE ustring_iterator& operator-=(difference_type n) noexcept {
			ptr -= n;
			return *this;
		}
	};

	using iterator = ustring_iterator;
	using const_iterator = const ustring_iterator;

	_NV_ALWAYS_INLINE ustring() noexcept : sz(0) {
		memset(Data.buf, 0, sizeof(Data.buf));
	}

	_NV_ALWAYS_INLINE ~ustring() {
		if (sz > sizeof(Data.buf)) {
			delete[] Data.ptr;
		}
	}

	_NV_ALWAYS_INLINE ustring(const char* str, size_t size) noexcept : sz((uint32_t)size) {
		// includes null terminator
		if (size < sizeof(Data.buf)) {
			memcpy(Data.buf, str, sz);
			Data.buf[sz] = '\0';
		} else {
			memcpy(Data.prefix, str, sizeof(Data.prefix));
			Data.ptr = new char[sz + 1];
			memcpy(Data.ptr, str, sz);
			Data.ptr[sz] = '\0';
		}
		sz++;
	}

	// includes null terminator
	_NV_ALWAYS_INLINE ustring(const char* str) noexcept : ustring(str, strlen(str)) {
	}

	_NV_ALWAYS_INLINE ustring(const std::string& str) noexcept : ustring(str.c_str(), str.size()) {
	}

	_NV_ALWAYS_INLINE ustring(const ustring& other) noexcept : sz(other.sz) {
		if (sz <= sizeof(Data.buf)) {
			memcpy(Data.buf, other.Data.buf, sizeof(Data.buf));
		} else {
			memcpy(Data.prefix, other.Data.prefix, sizeof(Data.prefix));
			Data.ptr = new char[sz];
			memcpy(Data.ptr, other.Data.ptr, sz);
		}
	}

	_NV_ALWAYS_INLINE ustring(ustring&& other) noexcept : sz(other.sz) {
		memcpy(Data.buf, other.Data.buf, sizeof(Data.buf));
		other.sz = 0;
		memset(other.Data.buf, 0, sizeof(other.Data.buf));
	}

	_NV_ALWAYS_INLINE ustring& operator=(const ustring& other) noexcept {
		if (this != &other) {
			if (sz > sizeof(Data.buf)) {
				delete[] Data.ptr;
			}

			sz = other.sz;
			if (sz <= sizeof(Data.buf)) {
				memcpy(Data.buf, other.Data.buf, sz);
			} else {
				memcpy(Data.prefix, other.Data.prefix, sizeof(Data.prefix));
				Data.ptr = new char[sz];
				memcpy(Data.ptr, other.Data.ptr, sz);
			}
		}
		return *this;
	}

	_NV_ALWAYS_INLINE ustring& operator=(ustring&& other) noexcept {
		if (this != &other) {
			if (sz > sizeof(Data.buf))
				delete[] Data.ptr;
			sz = other.sz;
			memcpy(Data.buf, other.Data.buf, sizeof(Data.buf));
			other.sz = 0;
			memset(other.Data.buf, 0, sizeof(other.Data.buf));
		}
		return *this;
	}
#if _NV_HAS_CXX17
	_NV_ALWAYS_INLINE ustring(const std::string_view str) noexcept : ustring(str.data(), str.size()) {
	}
#endif
	_NV_NODISCARD _NV_ALWAYS_INLINE size_t size() const noexcept {
		return (size_t)sz;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE char* data() noexcept {
		if (sz <= sizeof(Data.buf)) {
			return Data.buf;
		} else {
			return Data.ptr;
		}
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const char* data() const noexcept {
		if (sz <= sizeof(Data.buf)) {
			return Data.buf;
		} else {
			return Data.ptr;
		}
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const char* c_str() const noexcept {
		return data();
	}

	_NV_ALWAYS_INLINE void resize(size_t _size) {
		uint32_t size = (uint32_t)_size;
		if (sz <= sizeof(Data.buf)) {
			if (size <= sizeof(Data.buf)) {
				if (size > sz) {
					memset(Data.buf + sz, 0, size - sz);
				} else {
					memset(Data.buf + size, 0, sz - size);
				}
			} else {
				char temp[sizeof(Data.buf)];
				memcpy(temp, Data.buf, sz);
				Data.ptr = new char[size];
				memset(Data.ptr, 0, size);
				memcpy(Data.prefix, temp, sizeof(Data.prefix));
				memcpy(Data.ptr, temp, sz);
			}
		} else {
			char* tempPtr = Data.ptr;
			if (size <= sizeof(Data.buf)) {
				memset(Data.buf, 0, sizeof(Data.buf));
				memcpy(Data.buf, tempPtr, size);
			} else {
				Data.ptr = new char[size];
				memset(Data.ptr, 0, size);
				memcpy(Data.ptr, tempPtr, sz);
			}
			delete[] tempPtr;
		}

		sz = size;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE char at(size_t index) const {
		assert(index < sz);
		if (sz <= sizeof(Data.buf))
			return Data.buf[index];
		if (index < sizeof(Data.prefix))
			return Data.prefix[index];
		return Data.ptr[index];
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE char operator[](size_t index) const noexcept {
		return at(index);
	}

	_NV_NODISCARD int compare(const ustring& other) const {
		size_t minSize = sz < other.sz ? sz : other.sz;

		if (sz <= sizeof(Data.buf)) {
			if (other.sz <= sizeof(other.Data.buf)) {
				for (size_t i = 0; i < minSize; ++i) {
					unsigned char a = Data.buf[i];
					unsigned char b = other.Data.buf[i];
					if (a != b)
						return a - b;
				}
			} else {
				for (size_t i = 0; i < minSize; ++i) {
					unsigned char a = Data.buf[i];
					unsigned char b = i < sizeof(other.Data.prefix) ? other.Data.prefix[i] : other.Data.ptr[i];
					if (a != b)
						return a - b;
				}
			}
		} else {
			if (other.sz <= sizeof(other.Data.buf)) {
				for (size_t i = 0; i < minSize; ++i) {
					unsigned char a = i < sizeof(Data.prefix) ? Data.prefix[i] : Data.ptr[i];
					unsigned char b = other.Data.buf[i];
					if (a != b)
						return a - b;
				}
			} else {
				for (size_t i = 0; i < minSize; ++i) {
					unsigned char a = i < sizeof(Data.prefix) ? Data.prefix[i] : Data.ptr[i];
					unsigned char b = i < sizeof(other.Data.prefix) ? other.Data.prefix[i] : other.Data.ptr[i];
					if (a != b)
						return a - b;
				}
			}
		}

		if (sz == other.sz)
			return 0;
		return sz < other.sz ? -1 : 1;
	}

	_NV_NODISCARD int compare(const char* other, size_t other_size) const {
		size_t minSize = sz < other_size ? sz : other_size;

		if (sz <= sizeof(Data.buf)) {
			for (size_t i = 0; i < minSize; ++i) {
				unsigned char a = Data.buf[i];
				unsigned char b = other[i];
				if (a != b)
					return a - b;
			}
		} else {
			for (size_t i = 0; i < minSize; ++i) {
				unsigned char a = i < sizeof(Data.prefix) ? Data.buf[i] : Data.ptr[i];
				unsigned char b = other[i];
				if (a != b)
					return a - b;
			}
		}

		if (sz == other_size)
			return 0;
		return sz < other_size ? -1 : 1;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE int compare(const char* other) const {
		return compare(other, strlen(other));
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE int compare(const std::string& other) const {
		return compare(other.data(), other.size());
	}

#define DEFINE_COMPARISON_OPERATORS_1(OtherType)                                                                       \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator==(const ustring& a, OtherType b) {                            \
		return a.compare(b) == 0;                                                                                      \
	}                                                                                                                  \
	_NV_NODISCARD friend bool operator!=(const ustring& a, OtherType b) {                                              \
		return a.compare(b) != 0;                                                                                      \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator<(const ustring& a, OtherType b) {                             \
		return a.compare(b) < 0;                                                                                       \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator<=(const ustring& a, OtherType b) {                            \
		return a.compare(b) <= 0;                                                                                      \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator>(const ustring& a, OtherType b) {                             \
		return a.compare(b) > 0;                                                                                       \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator>=(const ustring& a, OtherType b) {                            \
		return a.compare(b) >= 0;                                                                                      \
	}
#define DEFINE_COMPARISON_OPERATORS(OtherType)                                                                         \
	DEFINE_COMPARISON_OPERATORS_1(OtherType)                                                                           \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator==(OtherType a, const ustring& b) {                            \
		return b.compare(a) == 0;                                                                                      \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator!=(OtherType a, const ustring& b) {                            \
		return b.compare(a) != 0;                                                                                      \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator<(OtherType a, const ustring& b) {                             \
		return b.compare(a) > 0;                                                                                       \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator<=(OtherType a, const ustring& b) {                            \
		return b.compare(a) >= 0;                                                                                      \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator>(OtherType a, const ustring& b) {                             \
		return b.compare(a) < 0;                                                                                       \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator>=(OtherType a, const ustring& b) {                            \
		return b.compare(a) <= 0;                                                                                      \
	}

	DEFINE_COMPARISON_OPERATORS_1(const ustring&)
	DEFINE_COMPARISON_OPERATORS(const char*)
	DEFINE_COMPARISON_OPERATORS(const std::string&)

#if _NV_HAS_CXX17
	DEFINE_COMPARISON_OPERATORS(const std::string_view)

	_NV_NODISCARD _NV_ALWAYS_INLINE int compare(const std::string_view other) const {
		return compare(other.data(), other.size());
	}
#endif
	_NV_NODISCARD _NV_ALWAYS_INLINE bool starts_with(const char* prefix, size_t len) const noexcept {
		if (len > sz)
			return false;


		size_t prefixLen = len < sizeof(Data.prefix) ? len : sizeof(Data.prefix);
		if (strncmp(Data.prefix, prefix, prefixLen) != 0)
			return false;

		if (prefixLen < len) {
			const char* data = sz <= sizeof(Data.buf) ? Data.buf : Data.ptr;
			return strncmp(data, prefix + prefixLen, len - prefixLen) == 0;
		}

		return true;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE bool ends_with(const char* suffix, size_t len) const noexcept {
		if (len > sz)
			return false;
		const char* data = sz <= sizeof(Data.buf) ? Data.buf : Data.ptr;
		return strncmp(data + sz - len, suffix, len) == 0;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE bool starts_with(const char* str) const noexcept {
		return starts_with(str, strlen(str));
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE bool starts_with(const std::string& str) const noexcept {
		return starts_with(str.data(), str.size());
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE bool ends_with(const char* str) const noexcept {
		return ends_with(str, strlen(str));
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE bool ends_with(const std::string& str) const noexcept {
		return ends_with(str.data(), str.size());
	}
#if _NV_HAS_CXX17
	_NV_NODISCARD _NV_ALWAYS_INLINE bool starts_with(const std::string_view str) const noexcept {
		return starts_with(str.data(), str.size());
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE bool ends_with(const std::string_view str) const noexcept {
		return ends_with(str.data(), str.size());
	}
#endif

	_NV_NODISCARD _NV_ALWAYS_INLINE iterator begin() {
		if (sz <= sizeof(Data.buf)) {
			return iterator(Data.buf);
		} else {
			return iterator(Data.ptr);
		}
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE iterator end() {
		if (sz <= sizeof(Data.buf)) {
			return iterator(Data.buf + sz);
		} else {
			return iterator(Data.ptr + sz);
		}
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const_iterator begin() const {
		return cbegin();
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const_iterator end() const {
		return cend();
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const_iterator cbegin() const {
		if (sz <= sizeof(Data.buf)) {
			return const_iterator(Data.buf);
		} else {
			return const_iterator(Data.ptr);
		}
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const_iterator cend() const {
		if (sz <= sizeof(Data.buf)) {
			return const_iterator(Data.buf + sz);
		} else {
			return const_iterator(Data.ptr + sz);
		}
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE std::string toStdString() const {
		if (sz <= sizeof(Data.buf))
			return std::string(Data.buf, sz);
		return std::string(Data.ptr, sz);
	}

	_NV_ALWAYS_INLINE friend std::ostream& operator<<(std::ostream& os, const ustring& str) {
		if (str.sz <= sizeof(Data.buf)) {
			os.write(str.Data.buf, str.sz - 1);
		} else {
			os.write(str.Data.ptr, str.sz - 1);
		}
		return os;
	}
}

_NV_UNPACKED;

_NV_UNPACKED
}

template <>
struct _NV_NODISCARD std::hash<nv::ustring> {
	_NV_ALWAYS_INLINE size_t operator()(const nv::ustring& s) const noexcept {
#if _NV_HAS_CXX17

		return std::hash<std::string_view>()(std::string_view(s.data(), s.size()));
#else
		return std::hash<std::string>()(std::string(s.data(), s.size()));
#endif
	}
};
