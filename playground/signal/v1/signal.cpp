#include "signal.hpp"

namespace playground
{
//----------------------------------------------------------------------------
static int test = 25;


class A : public Object
{
public:
	virtual int valueChanged() = 0; // can be commented
	DECL_SIGNAL(int, valueChanged);
};

class B : public Object
{
public:
	B() : val(0)
	{
	}

	int val;

	virtual int setValue() = 0;
	DECL_SLOT(int, B, setValue);

};



class A2 : public A
{
public:
	int valueChanged()
	{
		DBG_METHOD();
		EMIT(valueChanged);
		return 0;
	}

};


class B2 : public B
{
public:
	int setValue()
	{
		DBG_METHOD();

		val = test;
		return 0;
	}
};

class B3 : public B2
{
public:
	int setValue()
	{
		DBG_METHOD();
		return B2::setValue();
	}
};



int main(int ac, char * av[])
{
	A2 a;
	B3 b;

	A2 a1;
	B3 b1;

	connect(&a, valueChanged, &b, setValue);

	connect(&a1, valueChanged, &b1, setValue);

	std::cerr << "b.val = " << b.val << "\n";
	std::cerr << "b1.val = " << b1.val << "\n";

	test = 50;
	a.valueChanged();

	test = 100;
	a1.valueChanged();

	std::cerr << "b.val = " << b.val << "\n";
	std::cerr << "b1.val = " << b1.val << "\n";

	std::cerr << "---------" "\n";
	std::cerr << "sizeof(A) = " << sizeof(A) << "\n";
	std::cerr << "sizeof(A2) = " << sizeof(A2) << "\n";
	std::cerr << "sizeof(B) = " << sizeof(B) << "\n";
	std::cerr << "sizeof(B2) = " << sizeof(B2) << "\n";

	return 0;
}

}

int main(int ac, char * av[])
{
	return playground::main(ac, av);
}
