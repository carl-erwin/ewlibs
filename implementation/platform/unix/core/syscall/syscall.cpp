// system
#include "syscall.hpp"

#include <cstring>

// ew
#include <ew/ew_config.hpp>
#include <ew/core/Exception.hpp>


namespace ew
{
namespace core
{
namespace syscall
{
namespace unix_system
{

EW_CORE_SYSCALL_EXPORT
int sys_stat(const char * path, struct stat * buf)
{
	int ret = ::stat(path, buf);

	//    switch (errno) {
	//    default :
	//    }

	return ret;
}

EW_CORE_SYSCALL_EXPORT
int sys_fstat(int filedes, struct stat * buf)
{
	int ret = ::fstat(filedes, buf);

	//    switch (errno) {
	//    default :
	//    }

	return ret;
}

EW_CORE_SYSCALL_EXPORT
int sys_lstat(const char * path, struct stat * buf)
{
	int ret = ::lstat(path, buf);

	//    switch (errno) {
	//    default :
	//    }

	return ret;
}

EW_CORE_SYSCALL_EXPORT
int sys_open(const char * pathname, int flags)
{
	int ret = ::open(pathname, flags);

	if (ret >= 0)
		return ret;

	//DEBUG
	int err = errno;
	//cerr << ::strerror(errno) << "\n";
	errno = err;

	switch (errno) {

	case EACCES: {
		/*
		  EACCES The requested access to the file is not allowed, or search  permission
		  is denied for one of the directories in the path prefix
		  of pathname, or the file did not exist yet and write  access  to
		  the  parent  directory  is  not allowed.
		*/
		ew::Throw(ew::core::exceptions::permission_denied());
	}
	break ;

	//

	case EEXIST: {
		/*
		  EEXIST pathname already exists and O_CREAT and O_EXCL were used.
		*/
		ew::Throw(ew::core::exception("File exist"));
	}
	break ;

	case EFAULT: {
		/*
		  EFAULT pathname points outside your accessible address space.
		*/
		ew::Throw(ew::core::exception("File is unreachable"));
	}
	break ;

	case EISDIR: {
		/*
		  EISDIR pathname refers to a directory and the access requested involved
		  writing (that is, O_WRONLY or O_RDWR is set).
		*/
		ew::Throw(ew::core::exception("Requested file is a directory"));
	}
	break ;

	case ELOOP: {
		/*
		  ELOOP  Too  many symbolic links were encountered in resolving pathname,
		  or O_NOFOLLOW was specified but pathname was a symbolic link.
		*/
		ew::Throw(ew::core::exception("Too  many symbolic links"));
	}
	break ;

	case EMFILE: {
		/*
		  EMFILE The process already has the maximum number of files open.
		*/
		ew::Throw(ew::core::exception("Too  many opened files"));
	}
	break ;

	case ENAMETOOLONG: {
		/*
		  ENAMETOOLONG
		  pathname was too long.
		*/
		ew::Throw(ew::core::exceptions::object_name_too_long());

	}
	break ;

	case ENFILE: {
		/*
		  ENFILE The system limit on the total number  of  open  files  has  been
		  reached.
		*/
		ew::Throw(ew::core::exceptions::too_many_object_of_same_type());

	}
	break ;

	case ENODEV: {
		/*
		  pathname  refers  to  a device special file and no corresponding
		  device exists.  (This is a Linux kernel bug; in  this  situation
		  ENXIO must be returned.)
		*/
		ew::Throw(ew::core::exceptions::does_not_exists());

	}
	break ;

	case ENOENT: {
		/*
		  O_CREAT  is  not  set  and the named file does not exist.  Or, a
		  directory component in pathname does not exist or is a  dangling
		  symbolic link.
		*/
		ew::Throw(ew::core::exceptions::does_not_exists());
	}
	break ;

	case ENOMEM: {
		/*
		  Insufficient kernel memory was available.
		*/
		ew::Throw(ew::core::exceptions::no_memory());
	}
	break ;

	case ENOSPC: {
		/*
		  pathname  was  to  be created but the device containing pathname
		  has no room for the new file.
		*/
		// NoStorageSpace;
		ew::Throw(ew::core::exceptions::no_memory());
	}
	break;

	case ENOTDIR: {
		/*
		  A component used as a directory in pathname is not, in  fact,  a
		  directory,  or  O_DIRECTORY was specified and pathname was not a
		  directory.
		*/
		// BadPath; // or NonDirectoryInPathName
		ew::Throw(ew::core::exception("Bad parameter"));

	}
	break ;


	case ENXIO: {
		/*
		  O_NONBLOCK | O_WRONLY is set, the named file is a  FIFO  and  no
		  process has the file open for reading.  Or, the file is a device
		  special file and no corresponding device exists.
		*/
		// no end point | no attached device
		ew::Throw(ew::core::exceptions::does_not_exists());

	}
	break ;

	case EOVERFLOW: {
		/*
		  pathname refers to a regular file, too large to be  opened;  see
		  O_LARGEFILE above.
		*/
		//      TooLarge;
		ew::Throw(ew::core::exceptions::ressource_too_large());

	}
	break ;

	case EPERM: {
		/*
		  The  O_NOATIME  flag was specified, but the effective user ID of
		  the caller did not match the owner of the file  and  the  caller
		  was not privileged (CAP_FOWNER).
		*/
		ew::Throw(ew::core::exceptions::permission_denied());
	}
	break ;

	case EROFS: {
		/*
		  EROFS  pathname  refers  to  a file on a read-only filesystem and write
		  access was requested.
		*/
		// ReadOnlyFileSystem;
		ew::Throw(ew::core::exception("File system is read-only"));
	}
	break ;

	case ETXTBSY: {
		/*
		  ETXTBSY
		  pathname refers to an executable image which is currently  being
		  executed and write access was requested.
		*/

		ew::Throw(ew::core::exception("File in use"));
	}
	break ;

	default:
		ew::Throw(ew::core::exception("Open Error"));
	}

	return ret;
}

EW_CORE_SYSCALL_EXPORT
int sys_open(const char * pathname, int flags, mode_t mode)
{
	int ret = ::open(pathname, flags, mode);

	//    switch (errno) {
	//    default :
	//    }

	return ret;
}

EW_CORE_SYSCALL_EXPORT
int sys_creat(const char * pathname, mode_t mode)
{
	int ret = ::creat(pathname, mode);

	//    switch (errno) {
	//    default :
	//    }

	return ret;
}

EW_CORE_SYSCALL_EXPORT
ssize_t sys_read(int fd, void * buf, size_t count)
{
	int ret = ::read(fd, buf, count);

	//    switch (errno) {
	//    default :
	//    }

	return ret;
}

EW_CORE_SYSCALL_EXPORT
ssize_t sys_write(int fd, const void * buf, size_t count)
{
	int ret = ::write(fd, buf, count);

	//    switch (errno) {
	//    default :
	//    }

	return ret;
}

EW_CORE_SYSCALL_EXPORT
int sys_close(int fd)
{
	int ret = ::close(fd);

	//    switch (errno) {
	//    default :
	//    }

	return ret;
}

EW_CORE_SYSCALL_EXPORT
off_t sys_lseek(int fd, off_t offset, int whence)
{
	return ::lseek(fd, offset, whence);
}



// ____________________________________________________________________

EW_CORE_SYSCALL_EXPORT
#ifdef __linux__
int sys_pthread_mutexattr_gettype(const ::pthread_mutexattr_t * attr, int * type)
#endif
#ifdef __FreeBSD__
int sys_pthread_mutexattr_gettype(::pthread_mutexattr_t * attr, int * type)
#endif
{
	return  ::pthread_mutexattr_gettype(attr, type);
}

EW_CORE_SYSCALL_EXPORT
int sys_pthread_mutexattr_settype(::pthread_mutexattr_t * attr, int type)
{
	return ::pthread_mutexattr_settype(attr, type);
}


EW_CORE_SYSCALL_EXPORT
int sys_pthread_mutexattr_destroy(::pthread_mutexattr_t * attr)
{
	return ::pthread_mutexattr_destroy(attr);
}

EW_CORE_SYSCALL_EXPORT
int sys_pthread_mutexattr_init(::pthread_mutexattr_t * attr)
{
	return ::pthread_mutexattr_init(attr);
}




EW_CORE_SYSCALL_EXPORT
int sys_pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutexattr_t * attr)
{
	int ret;
	int err;
	for (;;) {

		ret = ::pthread_mutex_init(mutex, attr);
		err = errno;
		//      cerr << strerror(err) << "\n";
		if (ret == 0)
			return ret;

		switch (err) {

		case EAGAIN: {
			// The system lacked the necessary resources (other than memory) to initialize another mutex.
		} break;

		case ENOMEM: {
			// Insufficient memory exists to initialize the mutex.

			// some system hints a la openGL
			// ew::system::SYSCALL::ew_syscall_hint(AUTOMATIC_SYSCALL_RETRY)

			// sleep ???
		} break ;

		case EPERM: {
			//  The caller does not have the privilege to perform the operation.
			ew::Throw(ew::core::exceptions::permission_denied());
			return ret;

		}
		break ;

		case EBUSY: {
			// The implementation has detected an attempt to reinitialize the object referenced by mutex,
			// a  previously  initialized, but not yet destroyed, mutex.
		} break ;

		// impossible case
		default: {
			ew::Throw(ew::core::exceptions::object_creation_error());
			return -1;
		}

		}

		return ret;
	}
}


EW_CORE_SYSCALL_EXPORT
int sys_pthread_mutex_destroy(pthread_mutex_t * mutex)
{
	int ret;
	int err;
	for (;;) {

		ret = ::pthread_mutex_destroy(mutex);
		err = errno;
		//      cerr << "::pthread_mutex_destroy(mutex) = " << ret << "\n";
		if (ret == 0)
			return 0;

		switch (err) {

		case EBUSY: {
			// The implementation has detected
			// an attempt to destroy the object referenced by mutex while  it  is  locked  or
			// referenced  (for  example,  while  being used in a pthread_cond_timedwait() or
			// pthread_cond_wait()) by another thread.
			// sleep ?
		} break ;

		case EINVAL: {
			ew::Throw(ew::core::exceptions::object_destruction_error());
			return -1;
		}
		break ;

		// impossible case
		default: {
			ew::Throw(ew::core::exceptions::object_destruction_error());
			return -1;
		}

		}
	}
	return ret;
}


// TODO switch ret
EW_CORE_SYSCALL_EXPORT
int sys_pthread_mutex_lock(pthread_mutex_t * mutex)
{
	int ret = ::pthread_mutex_lock(mutex);
	if (ret == 0) {
		return ret;
	}
	return ret;
}


EW_CORE_SYSCALL_EXPORT
int sys_pthread_mutex_trylock(pthread_mutex_t * mutex)
{
	int ret = ::pthread_mutex_trylock(mutex);
	if (ret == 0) {
		return ret;
	}
	return ret;
}

EW_CORE_SYSCALL_EXPORT
int sys_pthread_mutex_unlock(pthread_mutex_t * mutex)
{
	int ret = ::pthread_mutex_unlock(mutex);
	if (ret == 0) {
		return ret;
	}
	return ret;
}

EW_CORE_SYSCALL_EXPORT
void sys_pthread_exit(void * value_ptr)
{
	::pthread_exit(value_ptr);
}

EW_CORE_SYSCALL_EXPORT
int sys_sched_yield(void)
{
	int ret = ::sched_yield();
	return ret;
}

EW_CORE_SYSCALL_EXPORT
int sys_pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine)(void *), void * arg)
{
	int ret = ::pthread_create(thread, attr, start_routine, arg);

	return ret;
}

EW_CORE_SYSCALL_EXPORT
int sys_pthread_cancel(::pthread_t thread)
{
	int ret = pthread_cancel(thread);
	return ret;
}

EW_CORE_SYSCALL_EXPORT
int sys_pthread_join(::pthread_t thread, void ** value_ptr)
{
	int ret = ::pthread_join(thread, value_ptr);
	return ret;
}



EW_CORE_SYSCALL_EXPORT
int sys_pthread_cond_destroy(::pthread_cond_t * cond)
{
	return ::pthread_cond_destroy(cond);
}


EW_CORE_SYSCALL_EXPORT
int sys_pthread_cond_init(::pthread_cond_t * cond, const ::pthread_condattr_t * attr)
{
	return ::pthread_cond_init(cond, attr);
}



EW_CORE_SYSCALL_EXPORT
int sys_pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t * mutex, const struct timespec * abstime)
{
	int ret = ::pthread_cond_timedwait(cond, mutex, abstime);
	if (ret == 0)
		return 0;

	switch (errno) {
	case ETIMEDOUT: {
		// DEBUG
		// set conditionvariable last error to timeoutReached
		// DEBUG
		// cerr << "pthread_cond_timedwait : ETIMEDOUT\n";
		return 0;
	}
	break ;

	case EINVAL: {
		// set last error to InvalidParameter
		//cerr << "pthread_cond_timedwait : EINVAL\n";
	}
	break ;

	case EPERM: {
		//cerr << "pthread_cond_timedwait : EPERM\n";
	}
	break ;

	}

	return -1;
}

EW_CORE_SYSCALL_EXPORT
int sys_pthread_cond_broadcast(::pthread_cond_t * cond)
{
	return ::pthread_cond_broadcast(cond);
}

EW_CORE_SYSCALL_EXPORT
int sys_pthread_cond_signal(::pthread_cond_t * cond)
{
	return ::pthread_cond_signal(cond);
}

EW_CORE_SYSCALL_EXPORT
int sys_pthread_cond_wait(::pthread_cond_t * cond, ::pthread_mutex_t * mutex)
{
	return ::pthread_cond_wait(cond, mutex);
}

EW_CORE_SYSCALL_EXPORT
int sys_pthread_setschedparam(::pthread_t thread, int policy, const struct sched_param * param)
{
	return ::pthread_setschedparam(thread, policy, param);
}

EW_CORE_SYSCALL_EXPORT
int sys_pthread_getschedparam(::pthread_t thread, int * policy, struct sched_param * param)
{
	return sys_pthread_getschedparam(thread, policy, param);
}

EW_CORE_SYSCALL_EXPORT
int  sys_clock_gettime(clockid_t clk_id, struct timespec * tp)
{
	return ::clock_gettime(clk_id, tp);
}


EW_CORE_SYSCALL_EXPORT
int sys_select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * errorfds, struct timeval * timeout)
{
	return ::select(nfds, readfds, writefds, errorfds, timeout);
}

EW_CORE_SYSCALL_EXPORT
int sys_unlink(const char * pathname)
{
	return ::unlink(pathname);
}

EW_CORE_SYSCALL_EXPORT
void * sys_mmap(void * addr, size_t len, int prot, int flags, int fildes, off_t off)
{
	return ::mmap(addr, len, prot, flags, fildes, off);
}

EW_CORE_SYSCALL_EXPORT
int sys_munmap(void * addr, size_t len)
{
	return ::munmap(addr, len);
}


}
}
}
}

