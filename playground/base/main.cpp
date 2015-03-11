/*

faire des fontions
genre
 ew::core::Os::sys_open() ?

std::cout does not disply ll character

ew::cout


channel --> fd , HANDLE ,
raw_read/raw_chnnel -> channel

File -> channel

*/

// for big files handling
#define __USE_FILE_OFFSET64

#include <iostream>
#include <string>

# include <stdint.h>
# include <inttypes.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "ew_base_classes.hpp"

/*
  File file("path")

  file.create(); // remains opened ? : no
  file.open(); // error if not exists

  ..
  read
  write
  ..
  close();
*/


namespace playground
{


class Channel : public EwIObject
{
public:
	Channel(u64 channel = 1)
	{
		_fd = channel;
	}
private:
	u64 _fd;

	friend bool raw_write(Channel & channel, const u8 * buffer, u64 bufferSize);
	friend bool raw_read(Channel & channel, u8 * buffer, u64 nrBytes);
};

bool raw_write(Channel & channel, const u8 * buffer, u64 bufferSize)
{
	ssize_t ret = ::write(channel._fd, (const void *) buffer, (size_t) bufferSize);
	if (ret == -1)
		return false;

	bufferSize = ret;

	return true;
}

bool raw_read(Channel & channel, u8 * buffer, u64 bufferSize)
{
	ssize_t ret = ::read(channel._fd, (void *) buffer, (size_t) bufferSize);
	if (ret == -1)
		return false;

	bufferSize = ret;

	return true;
}






class File : public EwIFile
{
public:

	File(const char * filename = 0)
		: _filename(0)
	{
		if (filename == 0)
			return ;

		delete [] _filename;

		u64 sz = strlen(filename);
		_filename = new char [ sz + 1 ];
		strncpy(_filename , filename, sz);
		_filename[ sz ] = 0;
	}

	virtual ~File()
	{
		this->close();
		delete [] _filename;
		_filename = 0;
	}

	//
	virtual const char * getName()
	{
		return _filename;
	}

	// set mode
	virtual bool open(open_mode mode = ReadOnly)
	{

		switch (mode) {
		case Read:
		case ReadOnly:
			_openFlags = O_RDONLY;
			break ;

		case Write:
		case WriteOnly:
			_openFlags = O_WRONLY;
			break ;

		case ReadWrite:
			_openFlags = O_RDWR;
			break ;
		};


		_fd = ::open((const char *) _filename, _openFlags);
		if (_fd == -1) {
			if (ew::core::AreExceptionEnabled() == true) {
				switch (errno) {

				case EACCES: {
					/*
					  EACCES The requested access to the file is not allowed, or search  permission
					  is denied for one of the directories in the path prefix
					  of pathname, or the file did not exist yet and write  access  to
					  the  parent  directory  is  not allowed.
					*/

					throw ew::core::Exception::PermissionDenied();
				}
				break ;

				//
				case EEXIST: {
					/*
					  EEXIST pathname already exists and O_CREAT and O_EXCL were used.
					*/
				}
				break ;

				case EFAULT: {
					/*
					  EFAULT pathname points outside your accessible address space.
					*/
					throw ew::core::Exception::PermissionDenied();
				}
				break ;

				case EISDIR: {
					/*
					  EISDIR pathname refers to a directory and the access requested involved
					  writing (that is, O_WRONLY or O_RDWR is set).
					*/
				}
				break ;

				case ELOOP: {
					/*
					  ELOOP  Too  many symbolic links were encountered in resolving pathname,
					  or O_NOFOLLOW was specified but pathname was a symbolic link.
					*/
				}
				break ;

				case EMFILE: {
					/*
					  EMFILE The process already has the maximum number of files open.
					*/
				}
				break ;

				case ENAMETOOLONG: {
					/*
					  ENAMETOOLONG
					  pathname was too long.
					*/
				}
				break ;

				case ENFILE: {
					/*
					  ENFILE The system limit on the total number  of  open  files  has  been
					  reached.
					*/
				}
				break ;

				case ENODEV: {
					/*
					  pathname  refers  to  a device special file and no corresponding
					  device exists.  (This is a Linux kernel bug; in  this  situation
					  ENXIO must be returned.)
					*/
				}
				break ;

				case ENOENT: {
					/*
					  O_CREAT  is  not  set  and the named file does not exist.  Or, a
					  directory component in pathname does not exist or is a  dangling
					  symbolic link.
					*/
				}
				break ;

				case ENOMEM: {
					/*
					  Insufficient kernel memory was available.
					*/
				}
				break ;

				case ENOSPC: {
					/*
					  pathname  was  to  be created but the device containing pathname
					  has no room for the new file.
					*/
				}
				break;

				case ENOTDIR: {
					/*
					  A component used as a directory in pathname is not, in  fact,  a
					  directory,  or  O_DIRECTORY was specified and pathname was not a
					  directory.
					*/
					throw ew::core::Exception::EwIException(std::string("Bad path"));

				}
				break ;


				case ENXIO: {
					/*
					  O_NONBLOCK | O_WRONLY is set, the named file is a  FIFO  and  no
					  process has the file open for reading.  Or, the file is a device
					  special file and no corresponding device exists.
					*/
				}
				break ;

				case EOVERFLOW: {
					/*
					  pathname refers to a regular file, too large to be  opened;  see
					  O_LARGEFILE above.
					*/
				}
				break ;

				case EPERM: {
					/*
					  The  O_NOATIME  flag was specified, but the effective user ID of
					  the caller did not match the owner of the file  and  the  caller
					  was not privileged (CAP_FOWNER).
					*/
				}
				break ;

				case EROFS: {
					/*
					  EROFS  pathname  refers  to  a file on a read-only filesystem and write
					  access was requested.
					*/
				}
				break ;

				case ETXTBSY: {
					/*
					  ETXTBSY
					  pathname refers to an executable image which is currently  being
					  executed and write access was requested.
					*/
				}
				break ;

				default:
					throw ew::core::Exception::EwIException(ew::core::types::String("Open Error"));
				}
			}

			return false;
		}


		// stat
		struct stat stat_buf;
		if (fstat(_fd, &stat_buf) == -1) {
			if (ew::core::AreExceptionEnabled() == true) {

				switch (errno) {
				case EACCES:
					// Search permission is denied for one of the  directories  in  the
					// path prefix of path.  (See also path_resolution(2).)
					break ;

				case EBADF:
					// filedes is bad.
					break ;

				case EFAULT:
					// Bad address.
					break ;

				case ELOOP:
					// Too many symbolic links encountered while traversing the path.
					break ;

				case ENAMETOOLONG:
					// File name too long.
					break ;

				case ENOENT:
					// A  component  of the path path does not exist, or the path is an
					// empty string.
					break ;

				case ENOMEM:
					// Out of memory (i.e. kernel memory).
					break ;

				case ENOTDIR:
					// A component of the path is not a directory.
					break ;

				default:
					throw ew::core::Exception::EwIException();
				}
			}

			return false;
		}

		// if dir
		if (S_ISDIR(stat_buf.st_mode)) {
			if (ew::core::AreExceptionEnabled() == true) {
				throw ew::core::Exception::EwIException(ew::core::types::String("File is a directory"));
			} else {
				return false;
			}
		}

		return true;
	}

	virtual bool close()
	{
		if (::close(_fd) < 0) {
			return false;
		}

		_fd = -1; // to avoid an other File::open

		return true;
	}

	virtual bool read(u8 * buffer, u64 & size)
	{
		int ret = ::read(_fd, (void *) buffer, size);
		if (ret <= 0)
			return false;

		size = (u64) ret;
		buffer[ size ] = '\0';

		// std::cerr << "nr bytes read " << size << "\n";
		return true;
	}

	virtual bool read(EwIBuffer & buffer)
	{
		return false;
	}

	virtual bool write(const u8 * buffer, u64 & size)
	{
		ssize_t ret = ::write(_fd, (const void *) buffer, (size_t) size);
		if (ret == -1)
			return false;

		size = ret;

		return true;
	}

	virtual bool write(EwIBuffer & buffer)
	{
		return true;
	}

private:
	char * _filename;
	int _fd;
	int _openFlags;
};


} // ! namespace ew

/////////////////////////////////////////////////////////////////////////////////////

ew::Channel STDIN(0);
ew::Channel STDOUT(1);
ew::Channel STDERR(2);
ew::Channel STDLOG(2);



// ----------------------------------------------------------------------------
// example of user programm : cat (for stdin use /dev/stdin)
// ----------------------------------------------------------------------------

void usage(char * binName)
{
	std::cerr << "usage : " << binName << " file ...\n";
}

int main(int ac, char ** av)
{
	if (ac < 2) {
		usage(av[ 0 ]);
		exit(1);
	}


	try {
		ew::core::enableException();
		//  ew::core::disableException();

		u64 sz = (1024 * 1024 * 4);
		u64 nrRead = sz; // max to read
		u8 * buffer = new u8 [ sz ];

		for (int count = 1; count < ac; ++count) {

			ew::File * file = new ew::File(av[ count ]);

			std::cerr << "Try to open '" << file->getName() << "' : ";
			if (file->open() != true) {
				std::cerr << "Error : Can't open '" << file->getName() << "'\n";

			} else {

				// clearMemory(buffer, 0, sz+1);
				// memset(buffer, 0, sz + 1);
				// bool status;
				while (file->read(buffer, nrRead)) {
					ew::raw_write(STDOUT, buffer, nrRead);
				}
				file->close();
			}

			delete file;
			file = 0;
		}

		delete [] buffer;
		buffer = 0;
	}

	catch (ew::core::Exception::PermissionDenied & excep) {
		std::cerr << "Error : catch exception : " << excep.what() << " for file : '" << file->getName() << "'" << "\n";
	}

	// default
	catch (ew::core::Exception::EwIException & excep) {
		std::cerr << "Error : catch exception : " << excep.what() << " for file : '" << file->getName() << "'" << "\n";
	}

	return 0;
}
