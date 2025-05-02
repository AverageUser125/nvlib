#pragma once
#include "common.h"
#include <vector>
#include <utility>
#include <cstddef>
#include <cassert>
#include <memory>
#include <cstdint>
#include <limits>
#include <exception>

namespace nv
{

template <typename KT, typename VT, typename Hash = std::hash<KT>>
class punordered_map {
	using size_type = std::size_t;

	struct entry {
		KT key;
		VT value;
	};

	const entry* entries = nullptr;
	size_type count = 0;

	explicit punordered_map(const entry* _entries, size_type _count) : entries(_entries), count(_count) {
	}

  public:
	punordered_map() = default;

	_NV_NODISCARD _NV_ALWAYS_INLINE const VT* find(const KT& key) const {
		size_type index = hashKey(key) % count;
		const entry& e = entries[index];
		if (e.key == key)
			return &e.value;
		return nullptr;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const VT& operator[](const KT& key) const {
		return at(key);
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE const VT& at(const KT& key) const {
		const VT* v = find(key);
		if (v == nullptr) {
			throw std::runtime_error("Value not found in function 'at'");
		}
		return *v;
	}

	_NV_NODISCARD static punordered_map make(const std::vector<std::pair<KT, VT>>& input, size_type startSize = 0,
											 size_type maxTries = 100, uint64_t startSeed = 0) {
		Hash baseHash;
		size_type n = input.size();
		if (startSize < n)
			startSize = n;

		for (size_type size = startSize; size <= n * 2; ++size) {
			uint64_t seed = startSeed;
			for (size_type attempt = 0; attempt < maxTries; ++attempt, ++seed) {
				entry* table = new entry[size];
				std::vector<bool> filled(size, false);
				bool ok = true;

				for (const auto& [k, v] : input) {
					uint64_t h = static_cast<uint64_t>(baseHash(k)) ^ seed;
					size_type i = h % size;
					if (filled[i]) {
						ok = false;
						break;
					}
					filled[i] = true;
					table[i] = {k, v};
				}

				if (ok)
					return punordered_map(table, size);

				delete[] table;
			}
		}

		throw std::runtime_error("Perfect hash table is impossible");
	}

	_NV_ALWAYS_INLINE ~punordered_map() {
		delete[] entries;
	}

	punordered_map(const punordered_map&) = delete;
	punordered_map& operator=(const punordered_map&) = delete;

	_NV_ALWAYS_INLINE punordered_map(punordered_map&& other) noexcept : entries(other.entries), count(other.count) {
		other.entries = nullptr;
		other.count = 0;
	}

	_NV_ALWAYS_INLINE punordered_map& operator=(punordered_map&& other) noexcept {
		if (this != &other) {
			delete[] entries;
			entries = other.entries;
			count = other.count;
			other.entries = nullptr;
			other.count = 0;
		}
		return *this;
	}

  private:
	_NV_NODISCARD _NV_ALWAYS_INLINE static size_type hashKey(const KT& key) {
		return Hash{}(key);
	}
};
}
