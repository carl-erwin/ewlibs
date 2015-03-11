#pragma once
// system

#include <sys/select.h>
#include <sys/time.h>
#include <time.h> // clock_gettime
#include <sched.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/mman.h> // mmap

namespace ew
{
namespace core
{
namespace syscall
{
namespace unix_system
{

int sys_stat(const char * path, struct ::stat * buf);

int sys_fstat(int filedes, struct ::stat * buf);

int sys_lstat(const char * path, struct ::stat * buf);

int sys_open(const char * pathname, int flags);

int sys_open(const char * pathname, int flags, ::mode_t mode);

int sys_creat(const char * pathname, ::mode_t mode);

ssize_t sys_read(int fd, void * buf, ::size_t count);

ssize_t sys_write(int fd, const void * buf, ::size_t count);

int sys_close(int fd);

off_t sys_lseek(int fd, off_t offset, int whence);

// pthread mutex
int sys_pthread_mutexattr_gettype(const ::pthread_mutexattr_t * attr, int * type);
int sys_pthread_mutexattr_settype(::pthread_mutexattr_t * attr, int type);

int sys_pthread_mutexattr_destroy(::pthread_mutexattr_t * attr);

int sys_pthread_mutexattr_init(::pthread_mutexattr_t * attr);

int sys_pthread_mutex_init(::pthread_mutex_t * mutex, const ::pthread_mutexattr_t * attr);

int sys_pthread_mutex_destroy(::pthread_mutex_t * mutex);

int sys_pthread_mutex_lock(::pthread_mutex_t * mutex);

int sys_pthread_mutex_trylock(::pthread_mutex_t * mutex);

int sys_pthread_mutex_unlock(::pthread_mutex_t * mutex);

// pthread
void sys_pthread_exit(void * value_ptr);

// sched
int sys_sched_yield(void);

int sys_pthread_create(::pthread_t * thread,
		       const ::pthread_attr_t * attr,
		       void * (*start_routine)(void *),
		       void * arg);

int sys_pthread_cancel(::pthread_t thread);

int sys_pthread_join(::pthread_t thread, void ** value_ptr);


int sys_pthread_cond_destroy(::pthread_cond_t * cond);

int sys_pthread_cond_init(::pthread_cond_t * cond, const ::pthread_condattr_t * attr);

int sys_pthread_cond_timedwait(::pthread_cond_t * cond,
			       ::pthread_mutex_t * mutex,
			       const struct timespec * abstime);

int sys_pthread_cond_wait(::pthread_cond_t * cond,
			  ::pthread_mutex_t * mutex);

int sys_pthread_cond_broadcast(::pthread_cond_t * cond);
int sys_pthread_cond_signal(::pthread_cond_t * cond);

int sys_pthread_setschedparam(::pthread_t thread, int policy, const struct sched_param * param);
int sys_pthread_getschedparam(::pthread_t thread, int * policy, struct sched_param * param);


// Time
int sys_clock_gettime(clockid_t clk_id, struct timespec * tp);

// select
int sys_select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * errorfds, struct timeval * timeout);


int sys_unlink(const char * pathname);

void * sys_mmap(void * addr, size_t len, int prot, int flags, int fildes, off_t off);

int sys_munmap(void * addr, size_t length);


}
}
}
}
