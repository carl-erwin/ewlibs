#include <ew/ew.hpp>
#include <ew/Ew_program.hpp>


// tempopray
#include <iostream>

/* ----------------------------------------------- */

/* application parameter */
u32 width = 320;
u32 height = 200;
u32 bpp = 16;
bool resize = true;
bool fullscreen = false;

using namespace ew::core::Application;
using namespace ew::core::THREAD;


void mythreadFfunc(u32 threadNo)
{
	u32 count = 0;
	while (1) {
		std::cerr << "hello i am thread[" << threadNo << "] (#" << count++ << ")\n";
		ew::core::time::usleep(threadNo * 10 * 1000);
	}
}

class myApp : public Simple_Application
{
private:
	//  IthreadInterface * _ithreadInterface;
	thread ** _thread;
	static const u32 _maxthread = 370;

public :
	myApp()
	{
		//    _ithreadInterface = 0;
		_thread = new thread * [ _maxthread ];
	}

	bool init(u32 width, u32 height, u32 bpp, bool resize, bool fullscreen)
	{
		Simple_Application::init(width, height, bpp, resize, fullscreen);

		for (u32 count = 0; count < _maxthread; ++count) {
			_thread[ count ] = new thread((func_t) mythreadFfunc, (Arg_t) count);
			_thread[ count ] ->start();
		}

		return true;
	}

	s32 quit(void)
	{
		for (u32 count = 0; count < _maxthread; ++count) {
			_thread[ count ] ->terminate();
			_thread[ count ] ->join();
		}

		return 0;
	}

};

// #include <windows.h>
// int WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)

int main(int ac, char ** av)
{
	ew::maths::vec4 v4;

	s32 retcode = 0;

	myApp application;

	application.init(width, height, bpp, resize, fullscreen);
	application.run();
	retcode = application.quit();

	return retcode;
}

