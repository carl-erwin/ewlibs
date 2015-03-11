#pragma once

#include <ew/core/types/types.hpp>

#include <ew/core/exception/exception.hpp>

#include <ew/core/interface/stream_interface.hpp>
#include <ew/core/interface/lock_interface.hpp>
#include <ew/core/interface/read_interface.hpp>
#include <ew/core/interface/write_interface.hpp>

namespace ew
{
namespace filesystem
{

using namespace ew::core::types;
using namespace ew::core::objects::stream;

// File are always binary
// ew::core::File::open_mode:: ... ENDIAN ????

/* remove double entries */
/* must be reworked */

/* FILESYSTEM Exception errors */

// Put list of FILE specific exceptions HERE
/* mote to system */

class file;

/* move to correct places */
class SystemErrorException;
class FatalErrorException;
class ReadOnlyFileSystem;
class WriteOnlyFileSystem;
class TooManySymbolicLink;

/* generic io error */
class IOException;

/* stream error */
class OpenException;
class CloseException;

/* readAbleObject */
class ReadException;

/* class WriteAbleObject; */
class WriteException;

/* class Stream */
class PostionException;

class NoSpaceLeftOnDeviceException;


class EW_FILESYSTEM_EXPORT file
	:
	public ew::core::object,
	public ew::core::objects::stream_object,
	public ew::core::objects::lock_interface,
	public ew::core::objects::read_interface,
	public ew::core::objects::write_interface
{
public:
	file(const char * fileName = 0);
	virtual ~file();

	// Object
	virtual const char * class_name() const;

	// stream_object
	virtual bool  open(open_mode mode);
	virtual bool  close();
	virtual bool  reopen(open_mode mode);
	virtual open_mode get_open_mode();
	virtual bool  is_opened();
	virtual bool  is_closed();

	// ILockableObject // thread lock (not file content)
	virtual bool lock();
	virtual bool unlock();
	virtual bool trylock();

	// in stream ?
	// bool lockRegion(u64 start, u64 end);
	// bool tryToLockRegion(u64 start, u64 end);
	// bool unlockRegion(u64 start, u64 end);

	//ew::core::objects::read_interface
	virtual bool read(u8 * buffer, u64 nrToRead , u64 * nrRead);

	// IWriteableObject
	virtual bool write(const u8 * buffer, u64 nrToWrite, u64 * nrWritten);

	// IStreamObject
	virtual bool set_position(u64 to, location from = beginning, direction direction = automatic_direction);
	virtual u64  get_position();

	// File
	virtual bool exists();
	virtual u64  size();

	virtual bool create(open_mode mode, bool truncate = false, bool overwrite = false);
	virtual bool destroy();

	//  u32 handle(); /* returns the operating system handle */ REMOVE

	/*
	   offset to the start of the projected area
	   size the number of byte the projectetd area covers
	   move read/write/exec
	   visibility the shared status of the projectted area (more work needed)
	   ptr pointer to the the start of of projected area
	*/
	virtual bool map(u64 offset, size_t size, open_mode mode, share_mode share, void ** ptr);
	virtual bool unmap(void * ptr, size_t size);

public: /* File exceptions */

	class exceptions
	{
	public:
		class does_not_exists;
		class already_exists;
		class in_use;
		/* bad type of file */
		class not_a_file;
		class name_too_long;
		class too_many_opened_files; // filesystem ??
		class bad_path;   /* incorrect path to file */
		class too_large; /* ?? */
	};

private:
	class private_data;
	class private_data * const d;

// TODO: add friends
// class MemoryMappedFile; MemoryMappedFile(File *);
};


class EW_CORE_EXCEPTION_EXPORT file::exceptions::does_not_exists : public ew::core::exception
{
public:
	does_not_exists() : exception("File Does not exist") { }
};

class EW_CORE_EXCEPTION_EXPORT file::exceptions::name_too_long: public ew::core::exception
{
public:
	name_too_long() : exception("Name too long") { }
};

class EW_CORE_EXCEPTION_EXPORT file::exceptions::too_many_opened_files : public ew::core::exception
{
public:
	too_many_opened_files() : exception("Too many opened files") { }
};


class EW_CORE_EXCEPTION_EXPORT file::exceptions::too_large : public ew::core::exception
{
public:
	too_large() : exception("Too Large") { }
};

class EW_CORE_EXCEPTION_EXPORT file::exceptions::in_use : public ew::core::exception
{
public:
	in_use() : exception("In Use") { }
};

/*
    for sub classes ??? :

    class FileWatcher
    {
     public:
       bool watch(File *); // this will discard the previous file

       u32 getNumberOfChanges();
       bool reset();
    };

    // watchAbleObject
    bool select() = 0;
    bool watch(File *) = 0;
    bool poll(void) = 0;

    // threadAwareFile before any write op
    bool lock(void) = 0;
    bool unlock(void) = 0;
*/

}  // ! namespace filesystem
} // ! namespace ew
