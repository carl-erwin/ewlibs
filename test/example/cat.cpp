#include <ew/console/console.hpp>
#include <ew/core/exception/exception.hpp>

#include <ew/filesystem/file.hpp>

namespace ew
{
namespace example
{

using namespace ew::core::objects;
using namespace ew::core::objects::stream;
using namespace ew::core::exceptions;
using namespace ew::console;

using namespace ew::filesystem;

void usage(char * binName)
{
	cerr << "usage : " << binName << " file ...\n";
}

const u64 buff_sz = 4096 * 16;
u8 buffer[buff_sz];

inline
bool cat(const char * filename)
{
	file file(filename);

	if (file.open(mode::read_only) != true) {
		cerr << "Error : Can't open '" << file.name() << "'\n";
		return false;
	}

	u64 nrRead;
	while (true) {
		if (file.read(buffer, buff_sz, &nrRead) != true)
			break ;
		if (nrRead == 0)
			break ;
		cout.write((char *)buffer, nrRead);
	}

	file.close();
	return true;
}


int main(int ac, char ** av)
{
	const char * default_file = "/dev/stdin";
	char ** current_file = av + 1;
	u32  nrFiles = ac - 1;

	ew::core::enable_exceptions();
	// ew::core::disable_exceptions();

	if (nrFiles == 0) {
		current_file = const_cast<char **>(&default_file);
		nrFiles = 1;
	}

	while (nrFiles--) {

		try {
			cat(*current_file);
		}

		catch (ew::core::exceptions::permission_denied & excep) {
			cerr << av[0] << " : '" << *current_file << "' : " << excep.what() << "\n";
		}

		// default
		catch (ew::core::exception & excep) {
			cerr << "Error : " << excep.what() << " ( '" << *current_file << "' )\n";
		}

		++current_file;
	}

	return 0;
}


}
}


int main(int ac, char ** av)
{
	return ew::example::main(ac, av);
}
