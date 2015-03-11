#pragma once

#include "common.hpp"

namespace playground
{

class CounterDerived : public Counter
{
public:
	virtual void setValue(int a);
};

}

extern "C" {
	playground::CounterDerived * createCounter(void);
	void destroyCounter(playground::CounterDerived *);
}
