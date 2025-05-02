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
#define _NV_GSTRING_SIZE 16

#define _NV_GSTRING_DATA_SIZE (_NV_GSTRING_SIZE - sizeof(uint32_t))
#define _NV_GSTRING_PREFIX_SIZE _NV_GSTRING_DATA_SIZE - _NV_PTR_SIZE
#else
#define _NV_GSTRING_PREFIX_SIZE 4

#define _NV_GSTRING_DATA_SIZE (_NV_GSTRING_PREFIX_SIZE + _NV_PTR_SIZE)
#define _NV_GSTRING_SIZE (_NV_GSTRING_DATA_SIZE + sizeof(uint32_t))
#endif

class _NV_PACKED gstring {
	uint32_t sz;

	union {
		struct {
			char prefix[_NV_GSTRING_PREFIX_SIZE];
			char* ptr;
		};

		char buf[_NV_GSTRING_DATA_SIZE];
	} data;

  public:
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using value_type = char;
	using pointer = const char*;
	using const_pointer = const char*;
	using reference = const char&;
	using const_reference = const char&;
	friend struct std::hash<gstring>;

	class _NV_NODISCARD gstring_iterator {
		const gstring* const str;
		size_t index;

	  public:
		using iterator_category = std::random_access_iterator_tag;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using value_type = char;
		using pointer = const char*;
		using const_pointer = const char*;
		using reference = const char&;
		using const_reference = const char&;

		_NV_ALWAYS_INLINE gstring_iterator(const gstring* s, size_t i) noexcept : str(s), index(i) {
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE value_type operator*() const noexcept {
			return ((const gstring*)str)->at(index);
		}

		_NV_ALWAYS_INLINE gstring_iterator& operator++() noexcept {
			++index;
			return *this;
		}

		_NV_ALWAYS_INLINE gstring_iterator operator++(int) noexcept {
			return gstring_iterator(str, index++);
		}

		_NV_ALWAYS_INLINE gstring_iterator& operator--() noexcept {
			--index;
			return *this;
		}

		_NV_ALWAYS_INLINE gstring_iterator operator--(int) noexcept {
			return gstring_iterator(str, index--);
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE gstring_iterator operator+(difference_type n) const noexcept {
			return gstring_iterator(str, index + n);
		}

		_NV_ALWAYS_INLINE gstring_iterator& operator+=(difference_type n) noexcept {
			index += n;
			return *this;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE gstring_iterator operator-(difference_type n) const noexcept {
			return gstring_iterator(str, index - n);
		}

		_NV_ALWAYS_INLINE gstring_iterator& operator-=(difference_type n) noexcept {
			index -= n;
			return *this;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE difference_type operator-(const gstring_iterator& other) const noexcept {
			return index - other.index;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE value_type operator[](difference_type n) const noexcept {
			return str->at(index + n);
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator==(const gstring_iterator& other) const noexcept {
			return index == other.index;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator!=(const gstring_iterator& other) const noexcept {
			return index != other.index;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator<(const gstring_iterator& other) const noexcept {
			return index < other.index;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator<=(const gstring_iterator& other) const noexcept {
			return index <= other.index;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator>(const gstring_iterator& other) const noexcept {
			return index > other.index;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator>=(const gstring_iterator& other) const noexcept {
			return index >= other.index;
		}
	};

	using iterator = gstring_iterator;
	using const_iterator = const gstring_iterator;

	_NV_ALWAYS_INLINE gstring() noexcept : sz(0) {
		memset(data.buf, 0, sizeof(data.buf));
	}

	_NV_ALWAYS_INLINE ~gstring() {
		if (sz > sizeof(data.buf)) {
			delete[] data.ptr;
		}
	}

	_NV_ALWAYS_INLINE gstring(const char* str, size_t size) noexcept : sz((uint32_t)size) {
		if (size <= sizeof(data.buf)) {
			memcpy(data.buf, str, sz);
		} else {
			memcpy(data.prefix, str, sizeof(data.prefix));
			data.ptr = new char[sz - sizeof(data.prefix)];
			memcpy(data.ptr, str + sizeof(data.prefix), sz - sizeof(data.prefix));
		}
	}

	_NV_ALWAYS_INLINE gstring(const char* str) noexcept : gstring(str, strlen(str)) {
	}

	_NV_ALWAYS_INLINE gstring(const std::string& str) noexcept : gstring(str.data(), str.size()) {
	}

	_NV_ALWAYS_INLINE gstring(const gstring& other) noexcept : sz(other.sz) {
		if (sz <= sizeof(data.buf)) {
			memcpy(data.buf, other.data.buf, sizeof(data.buf));
		} else {
			memcpy(data.prefix, other.data.prefix, sizeof(data.prefix));
			data.ptr = new char[sz - sizeof(data.prefix)];
			memcpy(data.ptr, other.data.ptr, sz - sizeof(data.prefix));
		}
	}

	_NV_ALWAYS_INLINE gstring(gstring&& other) noexcept : sz(other.sz) {
		memcpy(data.buf, other.data.buf, sizeof(data.buf));
		other.sz = 0;
		memset(other.data.buf, 0, sizeof(other.data.buf));
	}

	_NV_ALWAYS_INLINE gstring& operator=(const gstring& other) noexcept {
		if (this != &other) {
			if (sz > sizeof(data.buf)) {
				delete[] data.ptr;
			}

			sz = other.sz;
			if (sz <= sizeof(data.buf)) {
				memcpy(data.buf, other.data.buf, sz);
			} else {
				memcpy(data.prefix, other.data.prefix, sizeof(data.prefix));
				data.ptr = new char[sz - sizeof(data.prefix)];
				memcpy(data.ptr, other.data.ptr, sz - sizeof(data.prefix));
			}
		}
		return *this;
	}

	_NV_ALWAYS_INLINE gstring& operator=(gstring&& other) noexcept {
		if (this != &other) {
			if (sz > sizeof(data.buf))
				delete[] data.ptr;
			sz = other.sz;
			memcpy(data.buf, other.data.buf, sizeof(data.buf));
			other.sz = 0;
			memset(other.data.buf, 0, sizeof(other.data.buf));
		}
		return *this;
	}
#if _NV_HAS_CXX17
	_NV_ALWAYS_INLINE gstring(const std::string_view str) noexcept : gstring(str.data(), str.size()) {
	}
#endif
	_NV_NODISCARD size_t size() const noexcept {
		return (size_t)sz;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE char at(size_t index) const {
		assert(index < sz);
		if (sz <= sizeof(data.buf))
			return data.buf[index];
		if (index < sizeof(data.prefix))
			return data.prefix[index];
		return data.ptr[index - sizeof(data.prefix)];
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE char operator[](size_t index) const noexcept {
		return at(index);
	}

	_NV_NODISCARD int compare(const gstring& other) const {
		size_t minSize = sz < other.sz ? sz : other.sz;

		if (sz <= sizeof(data.buf)) {
			if (other.sz <= sizeof(other.data.buf)) {
				for (size_t i = 0; i < minSize; ++i) {
					unsigned char a = data.buf[i];
					unsigned char b = other.data.buf[i];
					if (a != b)
						return a - b;
				}
			} else {
				for (size_t i = 0; i < minSize; ++i) {
					unsigned char a = data.buf[i];
					unsigned char b = i < sizeof(other.data.prefix) ? other.data.buf[i]
																	: other.data.ptr[i - sizeof(other.data.prefix)];
					if (a != b)
						return a - b;
				}
			}
		} else {
			if (other.sz <= sizeof(other.data.buf)) {
				for (size_t i = 0; i < minSize; ++i) {
					unsigned char a = i < sizeof(data.prefix) ? data.buf[i] : data.ptr[i - sizeof(data.prefix)];
					unsigned char b = other.data.buf[i];
					if (a != b)
						return a - b;
				}
			} else {
				for (size_t i = 0; i < minSize; ++i) {
					unsigned char a = i < sizeof(data.prefix) ? data.buf[i] : data.ptr[i - sizeof(data.prefix)];
					unsigned char b = i < sizeof(other.data.prefix) ? other.data.buf[i]
																	: other.data.ptr[i - sizeof(other.data.prefix)];
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

		if (sz <= sizeof(data.buf)) {
			for (size_t i = 0; i < minSize; ++i) {
				unsigned char a = data.buf[i];
				unsigned char b = other[i];
				if (a != b)
					return a - b;
			}
		} else {
			for (size_t i = 0; i < minSize; ++i) {
				unsigned char a = i < sizeof(data.prefix) ? data.buf[i] : data.ptr[i - sizeof(data.prefix)];
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
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator==(const gstring& a, OtherType b) {                            \
		return a.compare(b) == 0;                                                                                      \
	}                                                                                                                  \
	_NV_NODISCARD friend bool operator!=(const gstring& a, OtherType b) {                                              \
		return a.compare(b) != 0;                                                                                      \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator<(const gstring& a, OtherType b) {                             \
		return a.compare(b) < 0;                                                                                       \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator<=(const gstring& a, OtherType b) {                            \
		return a.compare(b) <= 0;                                                                                      \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator>(const gstring& a, OtherType b) {                             \
		return a.compare(b) > 0;                                                                                       \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator>=(const gstring& a, OtherType b) {                            \
		return a.compare(b) >= 0;                                                                                      \
	}
#define DEFINE_COMPARISON_OPERATORS(OtherType)                                                                         \
	DEFINE_COMPARISON_OPERATORS_1(OtherType)                                                                           \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator==(OtherType a, const gstring& b) {                            \
		return b.compare(a) == 0;                                                                                      \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator!=(OtherType a, const gstring& b) {                            \
		return b.compare(a) != 0;                                                                                      \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator<(OtherType a, const gstring& b) {                             \
		return b.compare(a) > 0;                                                                                       \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator<=(OtherType a, const gstring& b) {                            \
		return b.compare(a) >= 0;                                                                                      \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator>(OtherType a, const gstring& b) {                             \
		return b.compare(a) < 0;                                                                                       \
	}                                                                                                                  \
	_NV_NODISCARD _NV_ALWAYS_INLINE friend bool operator>=(OtherType a, const gstring& b) {                            \
		return b.compare(a) <= 0;                                                                                      \
	}

	DEFINE_COMPARISON_OPERATORS_1(const gstring&)
	DEFINE_COMPARISON_OPERATORS(const char*)
	DEFINE_COMPARISON_OPERATORS(const std::string&)

#if _NV_HAS_CXX17
	DEFINE_COMPARISON_OPERATORS(const std::string_view)

	_NV_NODISCARD _NV_ALWAYS_INLINE int compare(const std::string_view other) const {
		return compare(other.data(), other.size());
	}
#endif

	_NV_NODISCARD _NV_ALWAYS_INLINE iterator begin() {
		return iterator(this, 0);
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE iterator end() {
		return iterator(this, sz);
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const_iterator begin() const {
		return cbegin();
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const_iterator end() const {
		return cend();
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const_iterator cbegin() const {
		return gstring_iterator((gstring*)this, 0);
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const_iterator cend() const {
		return gstring_iterator((gstring*)this, sz);
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE std::string toStdString() const {
		if (sz <= sizeof(data.buf))
			return std::string(data.buf, sz);
		return std::string(data.prefix, sizeof(data.prefix)) + std::string(data.ptr, sz - sizeof(data.prefix));
	}

	_NV_ALWAYS_INLINE friend std::ostream& operator<<(std::ostream& os, const gstring& str) {
		if (str.sz <= sizeof(data.buf)) {
			os.write(str.data.buf, str.sz);
		} else {
			os.write(str.data.prefix, sizeof(str.data.prefix));
			os.write(str.data.ptr, str.sz - sizeof(str.data.prefix));
		}
		return os;
	}
} _NV_UNPACKED;

_NV_UNPACKED
}

template <>
struct _NV_NODISCARD std::hash<nv::gstring> {
	_NV_ALWAYS_INLINE size_t operator()(const nv::gstring& s) const noexcept {
		if (s.size() <= 12) {
#if _NV_HAS_CXX17

			return std::hash<std::string_view>()(std::string_view(s.data.buf, s.size()));
#else
			return hash<std::string>()(std::string(s.data.buf, s.size()));
#endif
		} else {
#if _NV_HAS_CXX17
			size_t prefix_hash = std::hash<std::string_view>()(std::string_view(s.data.prefix, sizeof(s.data.prefix)));
			size_t data_hash =
				std::hash<std::string_view>()(std::string_view(s.data.ptr, s.size() - sizeof(s.data.prefix)));
#else
			size_t prefix_hash = hash<std::string>()(std::string(s.data.prefix, sizeof(s.data.prefix)));
			size_t data_hash = hash<std::string>()(std::string(s.data.ptr, s.size() - sizeof(s.data.prefix)));
#endif
			return prefix_hash ^ (data_hash + 0x9e3779b9 + (prefix_hash << 6) + (prefix_hash >> 2));
		}
	}
};
