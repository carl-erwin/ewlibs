#pragma once

#include "signal.hpp"

namespace playground
{

using namespace SIGNALS;

class Counter : public Object
{
protected:
	int m_val;
public:
	Counter(): m_val(0) {}

	DECL_SIGNAL_SLOT(void , Counter, setValue, int) = 0;

	DECL_VOID_SIGNAL_SLOT(int, Counter, getValue)
	{
		::std::cerr << "m_val = " << m_val << "\n";
		return 0;
	}

};

}
