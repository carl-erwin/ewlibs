#pragma once

#include <vector>
#include <tuple>

// simple enough

template <class T>
struct handle_index_allocator {

	// TODO: use std::forward to construct ad the same time

	handle_index_allocator()
		:
		table(1000, nullptr)
	{
	}

	template<typename... Args>
	std::tuple<T*, size_t> construct(Args... args)
	{
		size_t idx = get_index();
		if (idx == 0)
			return std::make_tuple(nullptr, idx);

		T * t = new T(args...);
		table[idx] = t;
		++used;
		return std::make_tuple(t,idx);

	}

	bool destroy(const size_t idx)
	{
		T * t = get(idx);
		if (!t)
			return false;

		delete t;
		release(idx);
		--used;

		return true;
	}

	T * get(const size_t idx)
	{
		if (idx < table.size()) {
			return table[idx];
		} else {
			return nullptr;
		}
	}

private:
	size_t get_index()
	{
		if (table.size() == used) {
			table.resize(table.size() + 1000);
		}

		size_t idx  = last_idx + 1;
		if (idx >= table.size()) {
			idx = 1;
		}

		while (table[ idx ] != nullptr || (idx == 0)) {
			++idx;
			if (idx == last_idx)
				return 0;
		}

		last_idx   = idx;
		return last_idx;
	}

	T * release(const size_t idx)
	{
		T * t = nullptr;
		if (idx < table.size()) {
			t = table[idx];
			table[idx] = nullptr;
		}

		return t;
	}


private:
	std::vector<T *> table;
	size_t last_idx = 0;
	size_t used = 0;

};
