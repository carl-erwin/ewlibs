#include "lib.hpp"

void playground::CounterDerived::setValue(int a)
{
	DBG_METHOD();

	if (m_val != a) {
		m_val = a;
		EMIT(setValue)(a); // MUST be here to avoid recursive calls
	}
}

extern "C" {
	playground::CounterDerived * createCounter(void)
	{
		return new playground::CounterDerived();
	}

	void destroyCounter(playground::CounterDerived * c)
	{
		delete c;
	}
}
