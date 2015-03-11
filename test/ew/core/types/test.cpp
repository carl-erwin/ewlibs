#include <iostream>
#include <cstdlib>


#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>
#include <ew/core/interface/read_interface.hpp>

using namespace ew::core::types;

namespace test
{
namespace types
{

class Test : public virtual ew::core::object
{
public:
	Test()
	{
		std::cerr << "Test::Test()\n";
	}

	virtual ~Test()
	{
		std::cerr << "Test::~Test()\n";
	}

	virtual const char * class_name() const
	{
		return "Test";
	}

	Test &
	operator=(const Test & orig)
	{
		return * this;
	}

	Test(const Test & orig)
		:
		object()
	{
		std::cerr << " Test(const Test & orig)\n";
	}


};

class Test2 :
	public Test,
	public virtual ew::core::objects::read_interface
{
public:
	Test2()
	{
		std::cerr << "Test2::Test2()\n";
	}

	virtual ~Test2()
	{
		std::cerr << "Test2::~Test2()\n";
	}

	virtual
	const char *
	class_name() const
	{
		return "Test2";
	}

	virtual
	const char *
	getName() const
	{
		return "test2";
	}

	virtual
	bool
	read(u8 * buffer, u64 size, u64 * read)
	{
		return true;
	}

	Test2 &
	operator=(const Test2 & orig)
	{
		return * this;
	}

	Test2(const Test2 & orig)
		:
		ew::core::object(),
		ew::core::objects::read_interface(),
		Test()
	{
		std::cerr << " Test2(const Test2& orig)\n";
	}

};


void
Test_base_types()
{
	std::cerr
			<< "sizeof(s8)  " << sizeof(s8) << "\n"
			<< "sizeof(s16) " << sizeof(s16) << "\n"
			<< "sizeof(s32) " << sizeof(s32) << "\n"
			<< "sizeof(s64) " << sizeof(s64) << "\n"
			<< "\n"
			<< "sizeof(u8)  " << sizeof(u8) << "\n"
			<< "sizeof(u16) " << sizeof(u16) << "\n"
			<< "sizeof(u32) " << sizeof(u32) << "\n"
			<< "sizeof(u64) " << sizeof(u64) << "\n"
			<< "\n"
			<< "sizeof(f32) " << sizeof(f32) << "\n"
			<< "sizeof(double) " << sizeof(double) << "\n"
			<< "sizeof(long double) " << sizeof(long double) << std::endl;
}

bool add_children(ew::core::object ** local_root, u32 level, u32 nrChildren)
{
	if (level == 0)
		return true;

	std::cerr << "ENTER @ level " << level <<  "\n";

	for (u32 count = 0; count < nrChildren; ++count) {

		for (u32 lvl = 0; lvl < level; lvl++) {
			std::cerr << " ";
		}

		std::cerr << "bool add_children(object ** local_root, " << "u32 level = " << level <<  ", u32 child = " << count + 1 << ")" << "\n";

		ew::core::object * child = new ew::core::object();
		child->set_parent(*local_root);
		child->set_name("child");
		add_children(&child, level - 1, nrChildren);
	}

	std::cerr << "LEAVE @ level " << level <<  "\n";

	return true;
}

int
main(int ac, char * av[])
{

	if (0) {

		Test_base_types();

		Test2 * t2;
		Test2 t2_0;
		t2 = new Test2();
		t2->set_name("t2");

		t2_0 = Test2(*t2);

		delete t2;
	}

	u32 level = 3;
	u32 nrChildren = 3;

	if (ac == 3) {
		level      = ::atoi(av[1]);
		nrChildren = ::atoi(av[2]);
	}

	ew::core::object * obj1 = new ew::core::object();
	obj1->set_name("root");
	add_children(&obj1, level, nrChildren);

	delete obj1;

	return (0);
}

} // ! namespace types
} // ! namespace test


int main(int ac, char ** av)
{
	return test::types::main(ac, av);
}


