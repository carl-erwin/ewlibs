#include <iostream>


class Exception
{
public:
	const char * what()
	{
		return "Exception";
	}
};


class ExceptionThower
{
public:
	ExceptionThower(Exception & ex) :
		_enable(true),
		_ex(&ex)
	{
		std::cerr << "ExceptionThowerr( Exception & ex )\n";
	}

	ExceptionThower(Exception * ex) :
		_enable(true),
		_ex(ex)
	{
		std::cerr << "ExceptionThowerr( Exception * ex )\n";
	}

	~ExceptionThower()
	{
		std::cerr << "~ExceptionThower()\n";
		if (_enable)
			throw * _ex;
	}

	void enable()
	{
		_enable = true;
	}
	void disable()
	{
		_enable = false;
	}

	bool _enable;
	Exception * _ex;
};

int main(int ac, char ** av)
{
	try {
		ExceptionThower exth(new Exception());

		//    exth.disable();
	}

	catch (Exception & ex) {
		std::cerr << "catch exception " << ex.what() << "\n";
	}

	return 0;
}
