#pragma once

#ifdef __EW_SIGNALS__

#include <iostream>
#include <list>
#include <utility>

#include "signals_forward_decl.hpp"

template <typename Ret>
class Signal<Ret>
{

	typedef  SlotBase<Ret> slot_t;
	typedef typename ::std::pair< Object *, slot_t * > obj_slot_pair_t;
	typedef ::std::list< obj_slot_pair_t > slot_list_t;
	typedef typename ::std::list< obj_slot_pair_t >::iterator slot_list_it;

	slot_list_t slot_list;

public:

	void connect(Object * o, slot_t * sl)
	{
		// register
		DBG_METHOD();
		slot_list.push_back(::std::make_pair(o, sl));
	}

	bool disconnect(Object * o, slot_t * sl)
	{
		DBG_METHOD();
		return false;
	}


	Ret emit()
	{
		DBG_METHOD();

		slot_list_it it = slot_list.begin();
		while (it != slot_list.end()) {
			obj_slot_pair_t p = *it;
			Object * o = p.first;
			slot_t * sl = p.second;
			sl->operator()(o);
			it++;
		}
	}

};

// Signal1
template <typename Ret, typename Arg0Type>
class Signal<Ret, Arg0Type>
{

	typedef  SlotBase<Ret, Arg0Type> slot_t;
	typedef typename ::std::pair< Object *, slot_t * > obj_slot_pair_t;
	typedef ::std::list< obj_slot_pair_t > slot_list_t;
	typedef typename ::std::list< obj_slot_pair_t >::iterator slot_list_it;

	slot_list_t slot_list;

public:
	~Signal()
	{
		DBG_METHOD();
		// this->lock()
		// foreach signal (remove_slot(this))
		// slot_list_it it = slot_list.begin();
		// while (it != slot_list.end()) {
		//  slot_t *slot = *it;
		//  slot->remove_signal(this);
		//  it++;
		// }
		// this->unlock()

	}

	void connect(Object * o, slot_t * sl)
	{
		// register
		DBG_METHOD();
		// TODO: only one instance of <o,sl>
		slot_list.push_back(::std::make_pair(o, sl));
	}

	bool disconnect(Object * o = 0, slot_t * sl = 0)
	{
		DBG_METHOD();
		if (o == 0 && sl == 0) {
			// remove all cnx
			return true;
		}

		if (sl == 0) {
			// remove all obj cnx
		}

		// default
		// remove this cnx <o, sl>

		return true;
	}

	Ret emit(Arg0Type a0)
	{
		DBG_METHOD();

		slot_list_it it = slot_list.begin();
		while (it != slot_list.end()) {
			obj_slot_pair_t p = *it;
			Object * o = p.first;
			slot_t * sl = p.second;
			sl->operator()(o, a0);
			it++;
		}

	}
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
