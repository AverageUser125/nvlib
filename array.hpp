#pragma once
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include <ostream>
#include "common.h"

namespace nv
{

template <typename T>
class _NV_PACKED array {
	T* ptr;
	size_t sz;

  public:
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;

	template <typename T>
	class _NV_NODISCARD array_iterator {
		T* ptr;

	  public:
#if _NV_HAS_CXX20
		using iterator_category = std::contiguous_iterator_tag;
#else
		using iterator_category = std::random_access_iterator_tag;
#endif
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;

		_NV_ALWAYS_INLINE array_iterator(T* ptr) noexcept : ptr(ptr) {
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE const_reference operator*() const noexcept {
			return *ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE const_pointer operator->() const noexcept {
			return ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE reference operator*() noexcept {
			return *ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE pointer operator->() noexcept {
			return ptr;
		}

		_NV_ALWAYS_INLINE array_iterator& operator++() noexcept {
			++ptr;
			return *this;
		}

		_NV_ALWAYS_INLINE array_iterator operator++(int) noexcept {
			array_iterator temp = *this;
			++ptr;
			return temp;
		}

		_NV_ALWAYS_INLINE array_iterator& operator--() noexcept {
			--ptr;
			return *this;
		}

		_NV_ALWAYS_INLINE array_iterator operator--(int) noexcept {
			array_iterator temp = *this;
			--ptr;
			return temp;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE reference operator[](difference_type n) const noexcept {
			return ptr[n];
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE array_iterator operator+(difference_type n) const noexcept {
			return array_iterator(ptr + n);
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE array_iterator operator-(difference_type n) const noexcept {
			return array_iterator(ptr - n);
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator==(const array_iterator& other) const noexcept {
			return ptr == other.ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator!=(const array_iterator& other) const noexcept {
			return ptr != other.ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator<(const array_iterator& other) const noexcept {
			return ptr < other.ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator>(const array_iterator& other) const noexcept {
			return ptr > other.ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator<=(const array_iterator& other) const noexcept {
			return ptr <= other.ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE bool operator>=(const array_iterator& other) const noexcept {
			return ptr >= other.ptr;
		}

		_NV_NODISCARD _NV_ALWAYS_INLINE difference_type operator-(const array_iterator& other) const noexcept {
			return ptr - other.ptr;
		}

		_NV_ALWAYS_INLINE array_iterator& operator+=(difference_type n) noexcept {
			ptr += n;
			return *this;
		}

		_NV_ALWAYS_INLINE array_iterator& operator-=(difference_type n) noexcept {
			ptr -= n;
			return *this;
		}
	};

	using iterator = array_iterator<T>;
	using const_iterator = const array_iterator<T>;

	_NV_ALWAYS_INLINE array() : sz(0), ptr(nullptr) {
	}

	_NV_ALWAYS_INLINE explicit array(size_t size) noexcept : sz(size) {
		if (sz > 0) {
			ptr = new T[sz];
		} else {
			ptr = nullptr;
		}
	}

	template <typename InputIterator>
	_NV_ALWAYS_INLINE array(std::move_iterator<InputIterator> first, std::move_iterator<InputIterator> last) noexcept
		: sz(std::distance(first, last)) {
		if (sz > 0) {
			ptr = new T[sz];
			std::move(first, last, ptr);
		} else {
			ptr = nullptr;
		}
	}

	template <typename it>
	_NV_ALWAYS_INLINE array(it first, it last) noexcept : sz(std::distance(first, last)) {
		if (sz > 0) {
			ptr = new T[sz];
			std::copy(first, last, ptr);
		} else {
			ptr = nullptr;
		}
	}

	_NV_ALWAYS_INLINE array(std::initializer_list<T> list) noexcept : sz(list.size()) {
		if (sz > 0) {
			ptr = new T[sz];
			std::copy(list.begin(), list.end(), ptr);
		} else {
			ptr = nullptr;
		}
	}

	_NV_ALWAYS_INLINE array(const std::vector<T>& vec) noexcept : sz(vec.size()) {
		if (sz > 0) {
			ptr = new T[sz];
			std::copy(vec.begin(), vec.end(), ptr);
		} else {
			ptr = nullptr;
		}
	}

	_NV_ALWAYS_INLINE array(std::vector<T>&& vec) noexcept : sz(vec.size()) {
		// TOCONSIDER: steal the data pointer from vec, but AddressSanitzer hates it
		if (sz > 0) {
			ptr = new T[sz];
			std::move(vec.begin(), vec.end(), ptr);
		} else {
			ptr = nullptr;
		}
	}

	_NV_ALWAYS_INLINE array(const array& other) noexcept : sz(other.sz) {
		if (sz > 0) {
			ptr = new T[sz];
			std::copy(other.begin(), other.end(), ptr);
		} else {
			ptr = nullptr;
		}
	}

	_NV_ALWAYS_INLINE array(array&& other) noexcept : ptr(other.ptr), sz(other.sz) {
		other.ptr = nullptr;
		other.sz = 0;
	}

	_NV_ALWAYS_INLINE array& operator=(const array& other) noexcept {
		if (this != &other) {
			auto* new_ptr = new T[other.sz];
			std::copy(other.begin(), other.end(), new_ptr);
			delete[] ptr;
			ptr = new_ptr;
			sz = other.sz;
		}
		return *this;
	}

	_NV_ALWAYS_INLINE array& operator=(array&& other) noexcept {
		if (this != &other) {
			delete[] ptr;
			ptr = other.ptr;
			sz = other.sz;
			other.ptr = nullptr;
			other.sz = 0;
		}
		return *this;
	}

	_NV_ALWAYS_INLINE array& operator=(std::vector<T>&& vec) noexcept {
		delete[] ptr;
		sz = vec.size();
		ptr = new T[sz];
		std::move(vec.begin(), vec.end(), ptr);
	}

	_NV_ALWAYS_INLINE array& operator=(const std::vector<T>& vec) noexcept {
		delete[] ptr;
		sz = vec.size();
		ptr = new T[sz];
		std::copy(vec.begin(), vec.end(), ptr);
	}

	_NV_ALWAYS_INLINE ~array() noexcept {
		delete[] ptr;
	}

	_NV_ALWAYS_INLINE void swap(array& other) noexcept {
		std::swap(ptr, other.ptr);
		std::swap(sz, other.sz);
	}

	_NV_ALWAYS_INLINE friend void swap(array& lhs, array& rhs) noexcept {
		std::swap(lhs.ptr, rhs.ptr);
		std::swap(lhs.sz, rhs.sz);
	}

	_NV_ALWAYS_INLINE void resize(size_t new_size) noexcept {
		if (new_size == sz) {
			return;
		}
		T* new_ptr = new T[new_size];
		size_t min_size = (new_size < sz) ? new_size : sz;
		std::move(ptr, ptr + min_size, new_ptr);

		delete[] ptr;
		ptr = new_ptr;
		sz = new_size;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const T* data() const noexcept {
		return ptr;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE T* data() noexcept {
		return ptr;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE size_t size() const noexcept {
		return sz;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const T& operator[](size_t index) const noexcept {
		return at(index);
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE T& operator[](size_t index) noexcept {
		return at(index);
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const T& at(size_t index) const noexcept {
		assert(index < sz);
		return ptr[index];
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE T& at(size_t index) noexcept {
		assert(index < sz);
		return ptr[index];
	}

	_NV_ALWAYS_INLINE void clear() noexcept {
		delete[] ptr;
		ptr = nullptr;
		sz = 0;
	}

	// This returns an owned pointer to the data
	_NV_NODISCARD _NV_ALWAYS_INLINE T* release() noexcept {
		T* out = ptr;
		ptr = nullptr;
		sz = 0;
		return out;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE bool empty() const noexcept {
		return sz == 0;
	}

	_NV_ALWAYS_INLINE void fill(const T& value) noexcept {
		std::fill(ptr, ptr + sz, value);
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE iterator begin() noexcept {
		return ptr;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE iterator end() noexcept {
		return ptr + sz;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const_iterator begin() const noexcept {
		return ptr;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const_iterator end() const noexcept {
		return ptr + sz;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const_iterator cbegin() const noexcept {
		return ptr;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const_iterator cend() const noexcept {
		return ptr + sz;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE T& front() noexcept {
		assert(sz > 0);
		return ptr[0];
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE T& back() noexcept {
		assert(sz > 0);
		return ptr[sz - 1];
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const T& front() const noexcept {
		assert(sz > 0);
		return ptr[0];
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const T& back() const noexcept {
		assert(sz > 0);
		return ptr[sz - 1];
	}
} _NV_UNPACKED;

_NV_UNPACKED
}
