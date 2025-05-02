#pragma once
#include "common.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <array>

namespace nv
{

struct ascii_lowercase_index {
	static constexpr size_t size = 26;

	_NV_ALWAYS_INLINE static size_t index(char ch) {
		return static_cast<size_t>(ch - 'a');
	}
};

struct ascii_index {
	static constexpr size_t size = 128;

	_NV_ALWAYS_INLINE static size_t index(char ch) {
		return static_cast<size_t>(ch);
	}
};

template <typename T, size_t N>
struct array_storage {
	std::array<T*, N> children = {};

	_NV_ALWAYS_INLINE T*& operator[](size_t i) {
		return children[i];
	}

	_NV_ALWAYS_INLINE const T* operator[](size_t i) const {
		return children[i];
	}
};

template <typename T>
struct map_storage {
	std::unordered_map<size_t, T*> children;

	_NV_NODISCARD _NV_ALWAYS_INLINE T*& operator[](size_t i) {
		return children[i];
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const T* operator[](size_t i) const {
		auto it = children.find(i);
		return it == children.end() ? nullptr : it->second;
	}
};

template <typename char_t, typename index_policy, template <typename> class storage_policy>
struct trie_node {
	bool is_end = false;
	storage_policy<trie_node> children;
};

template <typename T>
using array_storage_wrapper = nv::array_storage<T, nv::ascii_index::size>;

template <typename char_t = char, typename index_policy = ascii_index,
		  template <typename> class storage_policy = array_storage_wrapper>
class trie {
	using node = trie_node<char_t, index_policy, storage_policy>;
	node* _root = new node;

  public:
	_NV_ALWAYS_INLINE ~trie() {
		_clear(_root);
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE bool search(const std::basic_string<char_t>& word) const {
		const node* cur = _root;
		for (char_t ch : word) {
			size_t i = index_policy::index(ch);
			cur = cur->children[i];
			if (!cur)
				return false;
		}
		return cur->is_end;
	}

	_NV_ALWAYS_INLINE void insert(const std::basic_string<char_t>& word) {
		node* cur = _root;
		for (char_t ch : word) {
			size_t i = index_policy::index(ch);
			if (!cur->children[i])
				cur->children[i] = new node;
			cur = cur->children[i];
		}
		cur->is_end = true;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE bool starts_with(const std::basic_string<char_t>& prefix) const {
		const node* cur = _root;
		for (char_t ch : prefix) {
			size_t i = index_policy::index(ch);
			cur = cur->children[i];
			if (!cur)
				return false;
		}
		return true;
	}

  private:
	void _clear(node* n) {
		if (!n)
			return;
		for (size_t i = 0; i < index_policy::size; ++i)
			_clear(n->children[i]);
		delete n;
	}
};
}
