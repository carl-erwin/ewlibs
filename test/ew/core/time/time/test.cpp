#include <cstdlib>

#include <ew/core/time/time.hpp>
#include <ew/core/exception/exception.hpp>
#include <ew/Console.hpp>

namespace test
{

namespace time
{


using namespace ew::core::exceptions;
using namespace ew::core::time;
using namespace ew::core::types;
using namespace ew::console;


u32 cstring_to_u32(const char * str)
{
	return ::atoi(str);
}

int main(int ac, char * av[])
{

	ew::core::time::init();
	ew::core::enable_exceptions();

	u32 loop_time = 10;
	if (ac == 2)
		loop_time = cstring_to_u32(av[1]);

	try {

		for (u32 time_out = 4; time_out > 0; --time_out) {
			cerr << "test begining in " << time_out << " seconds\n";
			ew::core::time::sleep(1000);
		}

		u32 milliPerLoop = 500;

		u32 t0 = get_seconds_since_startup();

		enum granularity_e { MILLI, MICRO, NANO };
		u32 granularity = MILLI;
		for (;;) {

			if (get_seconds_since_startup() - t0 >= loop_time)
				break ;

			cerr << "geticks() = " << get_ticks() << "\n";
			cerr << "get_seconds_since_startup() = " << get_seconds_since_startup() << "\n";
			cerr << "get_milliseconds_since_startup() = " << get_milliseconds_since_startup() << "\n";

			switch (static_cast<granularity_e>(granularity)) {
			case MILLI:
				ew::core::time::nsleep(milliPerLoop);
				break ;
			case MICRO:
				ew::core::time::nsleep(milliPerLoop * 1000);
				break ;
			case NANO:
				ew::core::time::nsleep(milliPerLoop * 1000000);
				break  ;
			}
			granularity++;
			if (granularity > 2)
				granularity = 0;
		}

		cerr << "get_seconds_since_startup() = " << get_seconds_since_startup() << "\n";
	}

	catch (ew::core::exception & e) {
		cerr << "catch Exception ::" << e.what() << "\n";
	}

	ew::core::time::quit();

	return 0;
}

} // ! namespace time

} // ! namespace test


int main(int ac, char ** av)
{
	return test::time::main(ac, av);
}

