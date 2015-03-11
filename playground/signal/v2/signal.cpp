#include "signal.hpp"

namespace playground
{

using namespace playground::SIGNALS;

class TestSender : public Object
{
public:
	void send(int a)
	{
		DBG_METHOD();
		EMIT(send)(a);
	}

	DECL_SIGNAL(TestSender, send, void, int);
};


class TestReceiver : public Object
{
public:

	/* BETTER:
	DECL_SLOT(TestReceiver, recv, void, int);

	DECL_SLOT(TestReceiver, void, recv, int);
	{
	  DBG_METHOD();
	}

	  will expand in
	  void recv(int);
	  slot_type slot_recv;
	  void recv(int)
	  {
	    DBG_METHOD();
	  }
	*/

	void recv(int a)
	{
		DBG_METHOD();
	}
	DECL_SLOT(TestReceiver, recv, void, int);
};


class Counter : public Object
{
protected:
	int m_val;
public:
	Counter(): m_val(0) {}

	virtual int getValue()
	{
		std::cerr << "m_val = " << m_val << "\n";
	}

	virtual void setValue(int a) = 0;
	DECL_SLOT(Counter, setValue, void, int);
	DECL_SIGNAL(Counter, setValue, void, int);

};


class CounterDerived : public Counter
{
public:
	virtual void setValue(int a)
	{
		DBG_METHOD();

		if (m_val != a) {
			m_val = a;
			EMIT(setValue)(a); // MUST be here to avoid recursive calls
		}
	}
};


int main(int ac, char * av[])
{
	TestSender s;
	TestReceiver r;

	connect_signal(TestSender, send, &s, TestReceiver, recv, &r);

	s.send(0);

	CounterDerived c;

	CounterDerived d;

	connect_signal(Counter, setValue, &c, Counter, setValue, &d);

	c.setValue(19);
	c.getValue();
	d.getValue();

	d.setValue(25);
	c.getValue();
	d.getValue();

	return 0;
}

}

int main(int ac, char * av[])
{
	return playground::main(ac, av);
}
