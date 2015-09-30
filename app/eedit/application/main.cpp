#include <cassert>
#include <locale.h>


#include <iostream>
#include <memory>
#include "application/application.hpp"

#include "core/log/log.hpp"


int main(int ac, char ** av)
{
    bool ret;
    int status = EXIT_SUCCESS;

    setlocale(LC_ALL, "");

    app_log_init();

    auto app = eedit::create_application();
    ret = app->run(ac, av);
    if (ret != true) {
        assert(0);
        status = EXIT_FAILURE;
    }

    return status;
}
