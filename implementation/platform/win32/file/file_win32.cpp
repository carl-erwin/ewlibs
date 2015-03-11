// on va utiliser les fstream ?

#include "ew/core/types/types.hpp"
#include "ew/core/file/file.hpp"

namespace ew
{
namespace Core
{
namespace FILE
{

using namespace ew::core::types;
using namespace ew::core::objects::STREAM;

class File::private_data
{
public:
	int a;
};

File::File(const char * fileName)
	:
	d(new private_data)
{

}

File::~File()
{
	delete d;
}

const char * File::class_name() const
{
	return "ew::core::FILE::File";
}

const char * File::getName() const
{
	return 0;
}

bool File::lock()
{
	return false;
}

bool File::unlock()
{
	return false;
}

bool File::trylock()
{
	return false;
}

bool File::isLocked()
{
	return false;
}

bool File::open(open_mode mode)
{
	return false;
}

bool File::reOpen(open_mode mode)
{
	return false;
}

bool File::close(void)
{
	return false;
}

bool File::is_opened()
{
	return false;
}
bool File::is_closed()
{
	return false;
}

const open_mode File::get_open_mode()
{
	return invalid_mode;
}

bool File::write(const u8 * buffer, u64 & size)
{
	return false;
}


bool File::read(u8 * buffer, u64 & size)
{
	return false;
}

bool File::seek(u64 to, Location from, Direction direction)
{
	return false;
}


u64 File::getPos(void)
{
	return 0;
}


u64 File::size(void)
{
	return 0;
}


}
}
}
