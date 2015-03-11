#include <iostream>
#include <iomanip>

#include <ew/console/console.hpp>
#include <ew/core/object/object.hpp>

using namespace std;

using ew::core::object;


int main(int ac, char ** av)
{
	object * obj = new object();
	obj->set_name("obj0");

#if 0
	cerr << "\n\n BEGIN loop \n\n";
	for (unsigned int i = 0; i < 500 * 1000; ++i) {
		object * obj1 = new object();
		obj1->set_name("");
		obj1->set_parent(obj);
	}
	cerr << "\n\n END loop \n\n";
#endif

	const size_t nrChildren = 10;
	object * objs[nrChildren];
	for (unsigned int i = 0; i < nrChildren; i++) {
		objs[i] = new object();
		objs[i]->set_name("child");
		objs[i]->set_parent(obj);
	}

	for (unsigned int i = nrChildren / 2; i > 0; i--) {
		delete objs[i];
	}

	for (unsigned int i = nrChildren / 2; i > 0; i--) {
		object * obj_tmp = new object();
		obj_tmp->set_parent(obj);
	}

	delete obj;

	return 0;
}
