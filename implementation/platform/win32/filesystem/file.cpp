// system
#include <windows.h>

//
#include <iostream> // debug

// ew
#include <ew/core/types/types.hpp>
#include <ew/core/exception/exception.hpp>

#include <ew/filesystem/file.hpp>

namespace ew
{
namespace FILESYSTEM
{

using namespace ew::core::types;
using namespace ew::core::objects::STREAM;
using namespace ew::core::exceptions;

class File::private_data
{
public:
	private_data() : name(0), openMode(invalid_mode)  { }

	~private_data()
	{
		::delete [] name;
	}

	struct stat stat_buff;
	char * name;
	open_mode openMode;
};


File::File(const char * fileName)
	:
	d(::new private_data())
{
	// assert fileName
	// ew::Throw Invalid_Pointer

	s32 len = strlen(fileName);
	d->name = ::new char [len + 1];
	strncpy(d->name, fileName, len);
	d->name[len] = 0;
	// set state , etc ..
}

File::~File()
{
	close();
	::delete d;
}


//ew::core::object
const char * File::class_name() const
{
	return "ew::core::FILE::File";
}

const char * File::getName() const
{
	return d->name;
}

// IStreamObject
bool File::open(open_mode mode)
{
	return false;
}

bool File::close(void)
{
	return false;
}


bool File::reOpen(open_mode mode)
{
	return false;
}

open_mode File::get_open_mode()
{
	return d->openMode;
}

bool File::is_opened()
{
	return false;
}

bool File::is_closed()
{
	return !is_opened();
}

// ILockableObject // thread lock (not file content)
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

//ew::core::objects::read_interface
bool File::read(u8 * buffer, u64 nrToRead , u64 * nrRead)
{
	return false;
}

// IWriteableObject
bool File::write(const u8 * buffer, u64 nrToWrite, u64 * nrWritten)
{
	return true;
}

bool File::set_position(u64 to, Location from, Direction direction)
{
	return false;
}

u64  File::get_position()
{
	return 0;
}


u64 File::size(void)
{
	return (u64)0;
}

bool File::exists()
{
	return true;
}

bool File::create(open_mode mode)
{
	return true;
}

}
}
