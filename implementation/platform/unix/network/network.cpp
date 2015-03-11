#include <ew/ew_config.hpp>
#include <ew/network/network.hpp>

namespace ew
{
namespace network
{

bool init()
{
	// add global STREAM(fd,socket) watcher
	// using select ? epoll  ?
	return true;
}

bool quit()
{
	return true;
}
}
}

/*
  File->setStatus(opened | close)
  File->setReady(Mode::ReadWrite)
*/
