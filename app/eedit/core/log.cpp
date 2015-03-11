#include <fstream>      // std::filebuf

#include "log.hpp"

std::filebuf app_fb;
std::ostream app_log(&app_fb);

void app_log_init()
{
	app_fb.pubsetbuf(0, 0);
	app_fb.open("/tmp/eedit.log", std::ios::out|std::ios::app);
	app_log << "*** log initialized ***\n";
}



void app_log_quit()
{
	app_fb.close();
}
