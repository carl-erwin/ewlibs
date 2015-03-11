#pragma once

#ifdef __EW_SIGNALS__

#include "signals_forward_decl.hpp"

template <typename Ret>
class Signal<Ret>
{
public:
	Signal(): _obj(0), _slot(0) {}

	void connect(Object * o, SlotBase<Ret> * sl)
	{
		// register
		DBG_METHOD();
		_obj = o;
		_slot = sl;
	}

	Ret emit()
	{
		DBG_METHOD();
		if (_slot)
			_slot->operator()(_obj);
	}

private:
	// replace vector of std::vector<>
	Object * _obj;
	SlotBase<Ret> * _slot;
};

// Signal1
template <typename Ret, typename Arg0Type>
class Signal<Ret, Arg0Type>
{
public:
	Signal(): _obj(0), _slot(0) {}

	void connect(Object * o, SlotBase<Ret, Arg0Type> * sl)
	{
		// register
		DBG_METHOD();
		_obj = o;
		_slot = sl;
	}

	Ret emit(Arg0Type a0)
	{
		DBG_METHOD();
		if (_slot)
			_slot->operator()(_obj, a0);
	}

private:
	// replace vector of std::vector<>
	Object * _obj;
	SlotBase<Ret, Arg0Type> * _slot;
};

// Signal2
template <typename Ret, typename Arg0Type, typename Arg1Type>
class Signal<Ret, Arg0Type, Arg1Type>
{
public:
	Signal(): _obj(0), _slot(0) {}
	void connect(Object * o, SlotBase<Ret, Arg0Type, Arg1Type> * sl)
	{
		// register
		DBG_METHOD();
		_obj = o;
		_slot = sl;
	}

	Ret emit(Arg0Type a0, Arg1Type a1)
	{
		DBG_METHOD();
		if (_slot)
			_slot->operator()(_obj, a0, a1);
	}

private:
	// replace vector of std::vector<>
	Object * _obj;
	SlotBase<Ret, Arg0Type, Arg1Type> * _slot;
};

#endif
