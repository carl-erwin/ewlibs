#include <cassert>
#include <locale.h>


#include <iostream>
#include <memory>
#include "application/application.hpp"

#include "core/log.hpp"

namespace eedit
{

// move to application.cpp
auto create_application() -> std::shared_ptr<application> {
	auto app = std::make_shared<application>();
	set_application(app);
	return app;
}

int main(int ac, char ** av)
{
	bool ret;
	int status = EXIT_SUCCESS;

	setlocale(LC_ALL, "");

	app_log_init();

	auto app = create_application();
	ret = app->run(ac, av);
	if (ret != true) {
		assert(0);
		status = EXIT_FAILURE;
	}

	return status;
}

} // ! namespace eedit

/////////////////////////////////////////////////////////////////////

int main(int ac, char ** av)
{
	return eedit::main(ac, av);
}

/////////////////////////////////////////////////////////////////////
