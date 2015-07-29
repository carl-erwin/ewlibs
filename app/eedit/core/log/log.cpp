#include <stdio.h>
#include <stdarg.h>

#include "log.hpp"

static FILE * _app_log = nullptr;

void app_log_init()
{
    _app_log = fopen("/tmp/eedit.log", "a");
    app_logln(-1, "%s", "*** log initialized ***");
}


void app_log_quit()
{
    if (_app_log) {
        fclose(_app_log);
        _app_log = nullptr;
    }
}


void app_log(uint64_t level, const char *format, ...)
{
    if (_app_log == nullptr)
        return ;

    va_list ap;
    va_start(ap, format);
    vfprintf(_app_log, format, ap);
    va_end(ap);
    fflush(_app_log);
}

void app_logln(uint64_t level, const char *format, ...)
{
    if (_app_log == nullptr)
        return ;

    va_list ap;
    va_start(ap, format);
    vfprintf(_app_log, format, ap);
    va_end(ap);
    fprintf(_app_log, "\n");
    fflush(_app_log);
}
