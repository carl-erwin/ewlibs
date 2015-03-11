#include <iostream> // debug

#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

// ew
#include <ew/core/types/types.hpp>
#include <ew/core/exception/exception.hpp>
#include <ew/utils/utils.hpp>
#include <ew/filesystem/file.hpp>

#include <ew/console/console.hpp>
using ew::console::cerr;

#include "../core/syscall/syscall.hpp"


namespace ew
{

namespace filesystem
{

using namespace ew::core::types;

using namespace ew::core::objects::stream;

using namespace ew::core::exceptions;

using namespace ew::core::syscall::unix_system;

class file::private_data
{

public:
	private_data() : fd(-1), openMode(mode::invalid_mode) { }

	int fd;
	struct stat stat_buff;
	open_mode openMode;
};


file::file(const char * fileName)
	:
	d(::new private_data())
{
	set_name(fileName);
}

file::~file()
{

	close();
	::delete d;
}

//ew::core::object

const char * file::class_name() const
{
	return "ew::core::filesystem::file";
}

// IStreamObject
bool file::open(open_mode mode)
{
#ifdef __linux__
	int open_flags = O_LARGEFILE;
#else
	int open_flags = 0;
#endif

	switch (mode) {

	case mode::read: {
		open_flags |= O_RDONLY;
	}
	break ;

	case mode::write: {
		open_flags |= O_WRONLY;
	}
	break ;

	case mode::read_write: {
		open_flags |= O_RDWR;
	}
	break ;

	default: {
		cerr << "file::open(" << mode << ") :: not implemented\n";
		return false;
	}
	}

	d->openMode = mode;

	d->fd = sys_open(name(), open_flags);
	if (d->fd < 0)
		return false;

	// STREAM::AutocloseObject();
	//     struct stat statbuf;
	//     int ret = fstat(d->fd, &statbuf);
	//     if (ret < 0)
	//       cerr << "first " << strerror(errno) << "\n";

	if (sys_fstat(d->fd, &d->stat_buff) < 0) {
		cerr << strerror(errno) << "\n";

		close();

		ew::Throw(ew::core::exception("file::open :: fstat error"));

		return false;
	}

	// is directory ?
	if (S_ISDIR(d->stat_buff.st_mode)) {
		close();

		//  NotAFile;

		ew::Throw(ew::core::exception("file::open :: try to open a directory"));

		return false;
	}

	return true;
}

static bool __file_real_close(int & fd)
{
	if (fd >= 0) {

		for (;;) {
			int ret = sys_close(fd);
			if (ret == 0)
				break ;

			switch (errno) {

			case EINTR:
				continue ;

			case EBADF:
				// FatalError : somebody has closed arbitrarily our file descriptor ??
				return false;

				break ;

			case EIO:
				// IoError ;
				return false;

				break ;
			}
		}
	}

	fd = -1;
	return true;
}

bool file::close(void)
{
	return __file_real_close(d->fd);
}

bool file::reopen(open_mode mode)
{
	if (close() && open(mode))
		return true;

	// set a same pos ???

	return false;
}

open_mode file::get_open_mode()
{
	return d->openMode;
}

bool file::is_opened()
{

	if (d->fd >= 0)
		return true;

	return false;
}

bool file::is_closed()
{
	return !is_opened();
}

// ILockableObject // thread lock (not file content) ???

bool file::lock()
{
	return false;
}

bool file::unlock()
{
	return false;
}

bool file::trylock()
{
	return false;
}

//ew::core::objects::read_interface
bool file::read(u8 * buffer, u64 size , u64 * nrRead)
{
	// check opened etc
	// we must use ew::System::UNIX::SYSCALL::read
	int ret = sys_read(d->fd, (void *)buffer, size);

	if (ret < 0) {
		//       switch (ret) {
		// errno
		//        EAGAIN
		//        EBADF
		//        EFAULT
		//        EINTR
		//        EINVAL
		//        EIO
		//        EISDIR
		//       }

		*nrRead = 0;
		return false;
	}

	*nrRead = ret;

	return true;
}

// IWriteableObject

bool file::write(const u8 * buffer, u64 size, u64 * nrWritten)
{
	// check opened etc
	// we must use ew::System::UNIX::SYSCALL::write
	int ret = sys_write(d->fd, (void *)buffer, size);

	if (ret < 0) {
		// switch (ret):
		// ...

		*nrWritten = 0;
		return false;
	}

	*nrWritten = ret;

	return true;
}


// ISeekableObject
bool file::set_position(u64 to, location from, direction direction)
{

	off_t offset = -1;
	int whence = SEEK_SET;

	if (direction == ew::core::objects::stream::automatic_direction) {
		if (from == ew::core::objects::stream::end)
			direction = ew::core::objects::stream::backward;
		else
			direction = ew::core::objects::stream::forward;
	}

#if 0
	cerr << "set_position( to '" << to << "', from  = ";
	if (from == Beginning) {
		cerr << "Beginning";
	}
	if (from == Current) {
		cerr << "Current";
	}
	if (from == End) {
		cerr << "End ";
	}
	cerr << ", direction = ";
	if (direction == Forward) {
		cerr << "Forward";
	}
	if (direction == Backward) {
		cerr << "Backward";
	}
	cerr << ")\n";
#endif

	switch (from) {

	case beginning : {

		if (direction != ew::core::objects::stream::forward) {
			ew::Throw(ew::core::objects::stream_object::exceptions::invalid_stream_direction());
			return false;
		}
#if 0
		if (to > this->size()) {
			ew::Throw(ew::core::objects::stream_object::exceptions::position_overflow_exception());
			return false;
		}
#endif
		offset = to;

	}
	break ;

	case current: {

		u64 pos = get_position();
		if (direction == ew::core::objects::stream::forward) {
			// check that pos + to does not overflow file size
			if ((pos + to) > this->size()) {
				ew::Throw(ew::core::objects::stream_object::exceptions::position_overflow_exception());
				return false;
			}
			offset = pos + to;
		}

		if (direction == ew::core::objects::stream::backward) {
			// check that pos - to does not underflow offset
			if (to > pos) {
				ew::Throw(ew::core::objects::stream_object::exceptions::position_underflow_exception());
				return false;
			}
			offset = pos - to;
		}

	}
	break ;

	case end: {

		if (direction != ew::core::objects::stream::backward) {
			ew::Throw(ew::core::objects::stream_object::exceptions::invalid_stream_direction());
			return false;
		}
		// check that pos - to does not underflow offset
		u64 sz = this->size();
		if (to > sz) {
			ew::Throw(ew::core::objects::stream_object::exceptions::position_underflow_exception());
			return false;
		}
		offset = sz - to;

	}
	break ;

	} // ! switch (from)


	offset = sys_lseek(d->fd, offset, whence);
	if (offset == -1) {
		cerr << "lseek error\n";
		// throw SeekError see scopEx;
		return false;
	}

	// scopEx.cancel();
	return true;
}


u64 file::get_position(void)
{

	off_t ret = sys_lseek(d->fd, 0, SEEK_CUR);
	if (ret < 0) {
		cerr << "lseek error\n";
		// throw SeekError
	}

	return (u64)ret;
}

bool file::exists()
{
	int ret = sys_stat(name(), &d->stat_buff);
	// switch ret

	if (ret < 0)
		return false;

	if (S_ISREG(d->stat_buff.st_mode) || S_ISLNK(d->stat_buff.st_mode))
		return true;

	//switch (ret) {}
	return false;
}

u64 file::size(void)
{
	int ret = sys_stat(name(), &d->stat_buff);

	if (ret < 0) {
		//  set errors
		return 0;
	}

	return (u64)d->stat_buff.st_size;
}


bool file::create(open_mode mode, bool truncate, bool overwrite)
{
	// do not use O_TRUNK
	// the user

#ifdef __linux__
	int open_flags = O_LARGEFILE;
#else
	int open_flags = 0;
#endif

	if (truncate == true)
		open_flags |= O_TRUNC;

	mode_t mode_mask = 0;

	switch (mode) {

	case mode::read:
		open_flags |= O_RDONLY;
		mode_mask = S_IRUSR;
		break ;

	case mode::write:
		open_flags |= O_WRONLY;
		mode_mask = S_IWUSR;
		break ;

	case mode::read_write:
		open_flags |= O_RDWR;
		mode_mask = S_IRUSR | S_IWUSR;
		break ;

	default:
		cerr << "file::create(" << mode << ") :: not implemented\n";
		return false;
	}

	d->openMode = mode;

	if (overwrite == false) {
		open_flags |= O_EXCL;
	}

	open_flags |= O_CREAT;

	d->fd = sys_open(name(), open_flags, mode_mask);
	if (d->fd >= 0)
		return true;

	return false;
}

bool file::destroy()
{
	int ret = sys_unlink(name());
	if (ret < 0) {
		switch (errno) {

		default:
			return false;
		}
	}

	return true;
}

bool file::map(u64 offset, size_t size, open_mode mode, share_mode share, void ** ptr)
{
	*ptr = nullptr;
	// replace PROT_READ, MAP_PRIVATE,
	void * addr = (void *) - 1;
	addr = sys_mmap((void *)0, size, PROT_READ, MAP_PRIVATE, d->fd, offset);
	if ((void *)addr == MAP_FAILED) {
		std::cerr << " error : cannot map offset (" << offset << ") : errno(" << errno << ")" << strerror(errno) << "\n";
		return false;
	}
	*ptr = addr;
	return true;
}

bool file::unmap(void * ptr, size_t size)
{
	return sys_munmap(ptr, size) == 0;
}

}
}
