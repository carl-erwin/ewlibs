#include <iostream>

#define DBG_METHOD()    std::cerr << __FUNCTION__ << " : this = " << this << "\n";

namespace playground
{

class Object
{
public:

};

//----------------------------------------------------------------------------

template <typename Ret> class _Slot;
template <typename Ret> class Signal;


//
template <typename Ret>
class Signal
{
public:
	void connect(Object * o, _Slot<Ret> * sl)
	{
		// register
		_obj = o;
		_slot = sl;
	}

	// remove a given cnx
	void disconnect(Object * o, _Slot<Ret> * sl);

	// remove all cnx
	void disconnect(Object * o);



	Ret emit()
	{
		_slot->operator()(_obj);
	}
private:
	// replace vector of std::pair<>
	Object * _obj;
	_Slot<Ret> * _slot;
};
#define DECL_SIGNAL(RET__, name__) Signal<RET__> sig_##name__

#define SIGNAL(name) sig_##name

#define EMIT(name, ...) sig_##name.emit(__VA_ARGS__)

#define  connect(a__, SIG__, b__, SLT__) \
    (a__)->SIGNAL(SIG__).connect(static_cast<Object *>(b__), &(b__)->SLOT(SLT__))



// slot
template <typename Ret>
class _Slot
{
public:
	virtual Ret operator()(Object * obj) = 0;
};


template <typename OBJ, typename Ret, Ret(OBJ::*real_slot)() >
class Slot : public _Slot<Ret>
{
private:
	Ret operator()(OBJ * o)
	{
		return (o->*real_slot)();
	}

	Ret operator()(Object * obj)
	{
		return operator()(static_cast<OBJ *>(obj));
	}

};
#define SLOT(name) slot_##name


#define DECL_SLOT(RET__, CLASS__, MTH__) Slot< CLASS__, RET__, &CLASS__::MTH__ > slot_##MTH__
//

}
