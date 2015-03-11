#include <ew/ew.hpp>
#include <ew/Ew_program.hpp>
#include <ew/core/time/time.hpp>
#include <ew/core/exception/exception.hpp>

using namespace ew::core::exceptions;
using namespace ew::core::time;
using namespace ew::core::types;

u32 cstring_to_u32(const char * str)
{
	return atoi(str);
}

int main(int ac, char * av[])
{
	ew::core::time::init();
	ew::core::enable_exceptions();

	u32 loop_time = 5;
	if (ac == 2)
		loop_time = cstring_to_u32(av[1]);

	try {

		std::cerr << "test begining in 5 seconds\n";
		ew::core::time::sleep(u32(5000));

		u32 t0 = get_seconds_since_startup();
		for (;;) {
			std::cerr << "geticks() = " << get_ticks() << "\n";
			std::cerr << "get_seconds_since_startup() = " << get_seconds_since_startup() << "\n";
			std::cerr << "get_milliseconds_since_startup() = " << get_milliseconds_since_startup() << "\n";
			if (get_seconds_since_startup() - t0 >= loop_time)
				break ;
		}
	}

	catch (Exception & e) {
		std::cerr << "catch Exception ::" << e.what() << "\n";
	}

	ew::core::time::quit();

	return (0);
}
