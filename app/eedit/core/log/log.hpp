#pragma once

#include <stdint.h>
#include <stdarg.h>


void app_log_init();
void app_log_quit();

void app_log(uint64_t level, const char *format, ...);
void app_logln(uint64_t level, const char *format, ...); // add new line at end
