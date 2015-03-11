#pragma once

#ifdef __EW_SIGNALS__

template < typename Ret,
	   typename Arg0Type = DummyType,
	   typename Arg1Type = DummyType,
	   typename Arg2Type = DummyType,
	   typename Arg3Type = DummyType,
	   typename Arg4Type = DummyType,
	   typename Arg5Type = DummyType,
	   typename Arg6Type = DummyType,
	   typename Arg7Type = DummyType >
class Signal;

template < typename Ret,
	   typename Arg0Type = DummyType,
	   typename Arg1Type = DummyType,
	   typename Arg2Type = DummyType,
	   typename Arg3Type = DummyType,
	   typename Arg4Type = DummyType,
	   typename Arg5Type = DummyType,
	   typename Arg6Type = DummyType,
	   typename Arg7Type = DummyType >
class SlotBase;

#endif
