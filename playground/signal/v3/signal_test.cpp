#include <iostream>
#include <cstdlib>
#include <dlfcn.h>
#include "signal.hpp"
#include "common.hpp"
#include "lib.hpp"

namespace playground
{

using namespace SIGNALS;


int main(int ac, char ** av)
{
	void * handle = 0;
	CounterDerived *(*createCounter)(void)  = 0;
	void (*destroyCounter)(CounterDerived *) = 0;
	CounterDerived * c = 0;
	CounterDerived * d = 0;
	CounterDerived * e = 0;

	const int nr_counters = 1000;
	CounterDerived * vec[nr_counters];

	if (ac != 2) {
		::exit(1);
	}


	handle = dlopen(av[1], RTLD_LAZY);
	if (!handle) {
		::std::cerr << dlerror() << "\n";
		goto error;
	}

	createCounter  = reinterpret_cast<CounterDerived * ( *)(void)>(dlsym(handle, "createCounter"));
	if (!createCounter) {
		::std::cerr << dlerror() << "\n";
		goto error;
	}

	destroyCounter = reinterpret_cast<void ( *)(CounterDerived *)>(dlsym(handle, "destroyCounter"));
	if (!destroyCounter) {
		::std::cerr << dlerror() << "\n";
		goto error;
	}

	for (int i = 0; i < nr_counters; i++) {
		vec[i] = createCounter();
	}

	for (int i = 0; i < nr_counters - 1; i++) {
		connect_signal(Counter, setValue, vec[i], Counter, setValue, vec[i + 1]);
	}


	for (int j = 0; j < nr_counters; j++) {
		::std::cerr << "J = " << j << "\n";
		vec[j]->setValue(j + 30);
		for (int i = 0; i < nr_counters; i++) {
			vec[i]->getValue();
		}
	}

	for (int i = 0; i < nr_counters; i++) {
		destroyCounter(vec[i]);
		vec[i] = 0;
	}

	dlclose(handle);
	return 0;
error:
	if (handle)
		dlclose(handle);
	::exit(1);
}

} // ! namespace playground


int main(int ac, char ** av)
{
	return playground::main(ac, av);
}
