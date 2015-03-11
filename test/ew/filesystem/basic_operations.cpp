#include <cstring>

#include <ew/core/Exception.hpp>
#include <ew/Console.hpp>
#include <ew/Filesystem.hpp>

#include <string>

namespace ew
{
namespace test                  //
{

using namespace ew::core::types;
using namespace ew::core::objects;
using namespace ew::core::objects::stream;
using namespace ew::core::exceptions;
using namespace ew::console;

using namespace ew::filesystem;

void usage(char * binName)
{
	cerr << "usage : " << binName << " non existant file ...\n";
}

void do_test(const char * filename)
{
	file f(filename);

	u64 sz;
	u64 pos;

	if (f.exists()) {
		cerr << "Error : " << filename << " exists\n";
		return ;
	}

	cerr << "try to create '" << filename << "'";
	if (f.create(mode::read_write) != true) {
		cerr << "\nError : failed to create " << filename << "\n";
		return ;
	}
	cerr << " ok" << "\n";

	sz = f.size();
	cerr << " size = " << sz << " bytes.\n";

	pos = f.get_position();
	cerr << " pos = " << pos << "\n";

	const u64 nrToWrite = 10;
	u64 nrWritten = 0;
	u8 * dataToWrite = (u8 *)"0123456789";
	if (f.write(dataToWrite, nrToWrite, &nrWritten) != true) {
		cerr << "Error : c'ant write " << nrToWrite << "bytes in " << filename << "\n";
		return ;
	}
	cerr << "nrWritten = " << nrWritten << "\n";

	sz = f.size();
	cerr << " size = " << sz << " bytes.\n";

	pos = f.get_position();
	cerr << " pos = " << pos << "\n";

	if (sz != pos) {
		cerr << " Error size != pos \n";
		return ;
	}

	f.set_position(0, beginning, forward);
	pos = f.get_position();
	cerr << " pos = " << pos << "\n";

	u64 nrToRead = nrWritten;
	u64 nrRead = 0;

	u8 buffer[nrToWrite];
	if (f.read(buffer, nrToRead, &nrRead) != true) {
		cerr << "Error : c'ant read " << nrToRead << "bytes in " << filename << "\n";
		return ;
	}
	cerr << "read buffer = '" << (char *)buffer << "'\n";

	if (::strncmp((const char *)dataToWrite, (const char *)buffer, nrToWrite)) {
		cerr << "error when reading data back" << "\n";
		return ;
	}

	cerr << "\n"
	     << "testing set_position" << "\n";
	// must test here all set_position combination
	// f.erase();
	ew::core::enable_exceptions();

	bool loop = true;
	u32 test = 0;
	while (loop == true) {

		try {

			switch (test) {

			case 0: {
				f.set_position(5, beginning, forward); // reset position
				pos = f.get_position();
				cerr << " pos = " << pos << "\n\n";
			}
			break ;

			case 1: {
				f.set_position(2, current, forward);
				pos = f.get_position();
				cerr << " pos = " << pos << "\n\n";
			}
			break ;

			case 2: {
				f.set_position(1, current, backward);
				pos = f.get_position();
				cerr << " pos = " << pos << "\n\n";
			}
			break ;

			case 3: {
				f.set_position(6, end, backward);
				pos = f.get_position();
				cerr << " pos = " << pos << "\n\n";
			}
			break ;

			case 4: {
				f.set_position(6, end, forward);
				pos = f.get_position();
				cerr << " pos = " << pos << "\n\n";
			}
			break ;

			case 5: {
				f.set_position(f.size() + 1, beginning, forward);
				pos = f.get_position();
				cerr << " pos = " << pos << "\n\n";
			}
			break ;

			case 6: {
				f.set_position(f.size() + 1, current, forward);
				pos = f.get_position();
				cerr << " pos = " << pos << "\n\n";
			}
			break ;


			case 7: {
				f.set_position(f.size() + 1, current, backward);
				pos = f.get_position();
				cerr << " pos = " << pos << "\n\n";
			}
			break ;


			default:
				loop = false;
				break;
			}
		}

		catch (::ew::core::exception e) {
			cerr << "catch exception " << e.what() << "\n";
			cerr << "\n";
		}

		++test;
	}

}

int main(int ac, char ** av)
{
	if (ac == 1) {
		usage(av[0]);
		return -1;
	}

	for (s32 count = 1; count < ac; ++count) {
		do_test(av[count]);
	}

	return 0;
}


}
}


int main(int ac, char ** av)
{
	return ew::test::main(ac, av);
}
