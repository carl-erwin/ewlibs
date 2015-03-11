#pragma once

#ifdef __EW_SIGNALS__

#include "signals_forward_decl.hpp"

//----

//slot 0 args
template <typename Ret>
class SlotBase<Ret, DummyType, DummyType, DummyType, DummyType, DummyType, DummyType, DummyType>
{
public:
	virtual Ret operator()(Object * obj) = 0;
};
///

//slot 1 arg
template <typename Ret, typename Arg0Type>
class SlotBase<Ret, Arg0Type, DummyType, DummyType, DummyType, DummyType, DummyType, DummyType, DummyType>
{
public:
	virtual Ret operator()(Object * obj, Arg0Type a0) = 0;
};

//slot 2 args
template <typename Ret, typename Arg0Type, typename Arg1Type>
class SlotBase<Ret, Arg0Type, Arg1Type, DummyType, DummyType, DummyType, DummyType, DummyType, DummyType>
{
public:
	virtual Ret operator()(Object * obj, Arg0Type a0, Arg1Type a1) = 0;
};

//slot 3 args
template <typename Ret, typename Arg0Type, typename Arg1Type, typename Arg2Type>
class SlotBase<Ret, Arg0Type, Arg1Type, Arg2Type, DummyType, DummyType, DummyType, DummyType, DummyType>
{
public:
	virtual Ret operator()(Object * obj, Arg0Type a0, Arg1Type a1, Arg0Type a2) = 0;
};

//slot 4 args
template <typename Ret, typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type >
class SlotBase<Ret, Arg0Type, Arg1Type, Arg2Type, Arg3Type, DummyType, DummyType, DummyType, DummyType>
{
public:
	virtual Ret operator()(Object * obj, Arg0Type a0, Arg1Type a1, Arg0Type a2, Arg1Type a3) = 0;
};

//slot 5 args
template < typename Ret,
	   typename Arg0Type, typename Arg1Type,
	   typename Arg2Type, typename Arg3Type,
	   typename Arg4Type
	   >
class SlotBase<Ret, Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type, DummyType, DummyType, DummyType>
{
public:
	virtual Ret operator()(Object * obj, Arg0Type a0, Arg1Type a1, Arg0Type a2, Arg1Type a3, Arg0Type a4) = 0;
};


//slot 6 args
template < typename Ret,
	   typename Arg0Type, typename Arg1Type,
	   typename Arg2Type, typename Arg3Type,
	   typename Arg4Type, typename Arg5Type
	   >
class SlotBase<Ret, Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type, Arg5Type, DummyType, DummyType>
{
public:
	virtual Ret operator()(Object * obj,
			       Arg0Type a0, Arg1Type a1,
			       Arg0Type a2, Arg1Type a3,
			       Arg0Type a4, Arg1Type a5) = 0;
};

//slot 7 args
template < typename Ret,
	   typename Arg0Type, typename Arg1Type,
	   typename Arg2Type, typename Arg3Type,
	   typename Arg4Type, typename Arg5Type,
	   typename Arg6Type
	   >
class SlotBase<Ret, Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type, Arg5Type, Arg6Type, DummyType>
{
public:
	virtual Ret operator()(Object * obj,
			       Arg0Type a0, Arg1Type a1,
			       Arg0Type a2, Arg1Type a3,
			       Arg0Type a4, Arg1Type a5,
			       Arg0Type a6) = 0;
};

//slot 8 args
template < typename Ret,
	   typename Arg0Type, typename Arg1Type,
	   typename Arg2Type, typename Arg3Type,
	   typename Arg4Type, typename Arg5Type,
	   typename Arg6Type, typename Arg7Type
	   >
class SlotBase
{
public:
	virtual Ret operator()(Object * obj,
			       Arg0Type a0, Arg1Type a1,
			       Arg0Type a2, Arg1Type a3,
			       Arg0Type a4, Arg1Type a5,
			       Arg0Type a6, Arg1Type a7) = 0;
};



//------------- REAL SLOTS -----------------

template < typename OBJ, typename Ret,
	   typename Arg0Type = DummyType, typename Arg1Type = DummyType,
	   typename Arg2Type = DummyType, typename Arg3Type = DummyType,
	   typename Arg4Type = DummyType, typename Arg5Type = DummyType,
	   typename Arg6Type = DummyType, typename Arg7Type = DummyType,
	   typename real_slot = DummyType >
class Slot
{
	virtual Ret operator()(Object * obj,
			       Arg0Type a0, Arg1Type a1,
			       Arg0Type a2, Arg1Type a3,
			       Arg0Type a4, Arg1Type a5,
			       Arg0Type a6, Arg7Type a7) = 0;
};

// Slot0
template <typename OBJ, typename Ret>
class Slot<OBJ, Ret> : public SlotBase<Ret>
{
public:
	Slot() : real_slot(0) {}
	Ret(OBJ::*real_slot)();  // bind in connect

private:
	Ret operator()(OBJ * o)
	{
		std::cerr << __FUNCTION__ << "\n";
		return (o->*real_slot)();
	}

	Ret operator()(Object * obj)
	{
		std::cerr << __FUNCTION__ << "\n";
		return operator()(static_cast<OBJ *>(obj));
	}
};

// Slot1
template <typename OBJ, typename Ret, typename Arg0Type>
class Slot<OBJ, Ret, Arg0Type> : public SlotBase<Ret, Arg0Type>
{
public:
	Slot() : real_slot(0) {}
	Ret(OBJ::*real_slot)(Arg0Type);  // bind in connect

private:
	Ret operator()(OBJ * o, Arg0Type a0)
	{
		std::cerr << __FUNCTION__ << "\n";
		return (o->*real_slot)(a0);
	}

	Ret operator()(Object * obj, Arg0Type a0)
	{
		std::cerr << __FUNCTION__ << "\n";
		return operator()(static_cast<OBJ *>(obj), a0);
	}
};

// Slot2
template <typename OBJ, typename Ret, typename Arg0Type, typename Arg1Type>
class Slot<OBJ, Ret, Arg0Type, Arg1Type> : public SlotBase<Ret, Arg0Type, Arg1Type>
{
public:
	Slot() : real_slot(0) {}
	Ret(OBJ::*real_slot)(Arg0Type, Arg1Type);  // bind in connect

private:
	Ret operator()(OBJ * o, Arg0Type a0, Arg1Type a1)
	{
		std::cerr << __FUNCTION__ << "\n";
		return (o->*real_slot)(a0, a1);
	}

	Ret operator()(Object * obj, Arg0Type a0, Arg1Type a1)
	{
		std::cerr << __FUNCTION__ << "\n";
		return operator()(static_cast<OBJ *>(obj), a0, a1);
	}
};

#endif // __EW_SIGNALS__
