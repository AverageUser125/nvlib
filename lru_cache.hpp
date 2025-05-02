#pragma
#include "common.h"
#include <cstdint>
#include <list>
#include <unordered_map>

namespace nv
{
template <typename KT, typename VT>
class lru_cache {
	using list_t = std::list<std::pair<KT, VT>>;
	using map_t = std::unordered_map<KT, typename list_t::iterator>;

	size_t _capacity;
	list_t _list;
	map_t _map;

  public:
	_NV_ALWAYS_INLINE explicit lru_cache(size_t capacity) : _capacity(capacity) {
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE bool get(const Key& key, Value& value_out) {
		auto it = _map.find(key);
		if (it == _map.end())
			return false;

		_list.splice(_list.begin(), _list, it->second);
		value_out = it->second->second;
		return true;
	}

	_NV_NODISCARD _NV_ALWAYS_INLINE void put(const Key& key, const Value& value) {
		auto it = _map.find(key);
		if (it != _map.end()) {
			it->second->second = value;
			_list.splice(_list.begin(), _list, it->second);
			return;
		}

		if (_list.size() == _capacity) {
			auto last = _list.back();
			_map.erase(last.first);
			_list.pop_back();
		}

		_list.emplace_front(key, value);
		_map[key] = _list.begin();
	}
};
}
