// system
#include <netinet/in.h>
#include <arpa/inet.h>


#include <netdb.h>
#include <net/if.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <limits.h>
#include <sys/types.h>

//
#include <cstdlib>

// ew
#include "../../unix/core/syscall/syscall.hpp"
#include "ew/network/connection.hpp"
#include "ew/utils/utils.hpp"
#include "ew/console/console.hpp"
using ew::console::cerr;

namespace ew
{
namespace network
{

using namespace ew::core::types;
using namespace ew::core::objects::stream;
using namespace ew::core::syscall::unix_system;

class connection_common_data
{
public:
	int       m_sfd;
	char   *  m_host;
	char   *  m_port_str;
	int       m_port;
	open_mode m_mode;

	int ai_family;
	int ai_socktype;
	int ai_protocol;

	struct sockaddr m_saddr;
	socklen_t m_addrlen;


	connection_common_data(const char * host, const char * port)
		:
		m_sfd(-1),
		m_host(0),
		m_port(-1),
		m_mode(mode::invalid_mode)
	{
		m_host = ew::utils::c_string_dup(host);
		m_port_str = ew::utils::c_string_dup(port);
		m_port = ::atoi(port);
	}

	virtual ~connection_common_data()
	{
		delete [] m_host;
		delete [] m_port_str;
		m_host = 0;
		m_port_str = 0;
		m_port = -1;
		m_sfd  = -1;
		m_mode = mode::invalid_mode;
	}

	// TODO: public ?
	bool connect()
	{
		bool res = false;

		// connected client
		if (::connect(m_sfd, &m_saddr, m_addrlen) == 0) {
			res = true;
		}

		return res;
	}


	bool open(open_mode mode, int socket_family, int socket_type)
	{
		std::cerr << __PRETTY_FUNCTION__ << "\n";

		struct addrinfo hints;
		struct addrinfo * info = NULL;
		int res = false;

		memset(&hints, 0, sizeof(struct addrinfo));

		hints.ai_family   = socket_family;
		hints.ai_socktype = socket_type;
		hints.ai_flags    = 0;
		hints.ai_protocol = 0;

		if (getaddrinfo(m_host, NULL, &hints, &info) != 0) {
			return false;
		}

		m_mode = mode;

		struct addrinfo * rp = 0;

		for (rp = info; rp != NULL; rp = rp->ai_next) {
			int sfd = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (sfd == -1)
				continue;

			switch (hints.ai_family) {
			case PF_INET:
				struct sockaddr_in * sin = (struct sockaddr_in *)&m_saddr;
				memcpy(sin, rp->ai_addr, rp->ai_addrlen);
				sin->sin_port = htons(m_port);
				std::cerr << " m_port = " << m_port << "\n";
				m_addrlen = sizeof(struct sockaddr_in);
				break;
			}

			if (m_mode & mode::listener) {

				int optval  = 1;
				int sys_ret = ::setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,  (const void *)&optval, sizeof (optval));
				if (sys_ret != 0) {
					cerr << "set sockopt(SO_REUSEADDR) error\n";
					::close(sfd);
					continue;
				}

				if (::bind(sfd, &m_saddr, m_addrlen) == 0) {
					m_sfd = sfd;
					res = true;
					cerr << "bind server ok\n";
					break;
				}

			} else {

				cerr << "client socket ok\n";

				m_sfd = sfd;
				res = true;

				res = connect();
				if (res == false) {
					cerr << "client socket : error : cannot connect to server\n";
				}

				break;
			}

			::close(sfd);
		}

		if (res == true) {
			// save socket info
			ai_family   = rp->ai_family;
			ai_socktype = rp->ai_socktype;
			ai_protocol = rp->ai_protocol;

			// hack
			if (socket_type == SOCK_STREAM) {

				if (m_mode & mode::listener) {
					if (::listen(m_sfd, 0) < 0) {
						cerr << "listen error\n";
					}
				}

			}
		}

		freeaddrinfo(info);
		return res;
	}


	bool close()
	{
		int fd = m_sfd;
		if (fd < 0)
			return 0;
		while (ew::core::syscall::unix_system::sys_close(fd) != 0) {
			if (errno != EINTR)
				return -1;
		}
		m_sfd = -1;
		return 0;
	}



	bool read(u8 * buff, u32 len, u32 * nb_read)
	{

#if 1
		struct sockaddr address;
		socklen_t address_len = sizeof(struct sockaddr_in);

		int sys_ret;


		sys_ret =  ::recvfrom(m_sfd,
				      buff, len,
				      0,
				      &address, &address_len);
#else
		int sys_ret = ::recv(m_sfd, buff, len, 0);
#endif
		// int ret = ::read(m_sfd, buff, len);
		if (sys_ret < 0) {
			*nb_read = 0;
			// check errno
			if ((errno = EINTR) || (errno == EAGAIN)) {
				return true;
			}

			std::cerr << " read error\n";
			return false;
		}

		*nb_read = sys_ret;
		return true;
	}


	bool write(const u8 * buff, u32 len, u32 * nb_written)
	{
		int ret = ::write(m_sfd, buff, len);
		if (ret < 0) {
			*nb_written = 0;
			// check errno
			if ((errno = EINTR) || (errno == EAGAIN)) {
				std::cerr << "EINTR EAGAIN\n";
				return true;
			}

			return false;

		} else {
			std::cerr << "write error\n";
		}

		std::cerr << "write " << ret << " bytes\n";
		*nb_written = ret;
		return true;
	}

};

//

class tcp_address : public address
{
public:
	tcp_address()
	{
	}

	virtual ~tcp_address()
	{
	}

	void release()
	{
		delete this;
	}

public:
	struct sockaddr addr;
	socklen_t addr_len;
};


class tcp_connection::private_data : public connection_common_data
{
public:
	private_data(const char * host, const char * port) : connection_common_data(host, port) {}
};

class udp_connection::private_data : public connection_common_data
{
public:
	private_data(const char * host, const char * port) : connection_common_data(host, port) {}
};

////

tcp_connection::tcp_connection(const char * host, const char * port)
	:
	d(new private_data(host, port))
{
	cerr << __FUNCTION__ << "\n";
}

tcp_connection::~tcp_connection()
{
	close();
	delete d;
}

bool tcp_connection::open(open_mode mode)
{
	return d->open(mode, AF_INET, SOCK_STREAM);
}

bool tcp_connection::close()
{
	return d->close();
}

bool tcp_connection::read(u8 * buff, u32 len, u32 * nb_read)
{
	return d->read(buff, len, nb_read);
}

bool tcp_connection::write(const u8 * buff, u32 len, u32 * nb_written)
{
	return d->write(buff, len, nb_written);
}

connection * tcp_connection::accept(/* addresse criteria */)
{

	int fd = ::accept(d->m_sfd, (struct sockaddr *)NULL, (socklen_t *)NULL);
	if (fd < 0) {
		cerr << "accept error\n";
		return 0;
	}

	tcp_connection * conn = new tcp_connection(this->d->m_host, this->d->m_port_str);
	conn->d->m_sfd = fd;

	cerr << "accept ok\n";
	return conn;
}

ew::network::address * tcp_connection::new_address()
{
	return new tcp_address();
}

bool tcp_connection::sendto(const address * addr, const u8 * buff, u32 len, u32 * nb_written)
{
//    const tcp_address * tcp_addr = static_cast<const tcp_address *>(addr);
//    ssize_t nr = ::sendto(d->m_sfd, buff, len, 0, &tcp_addr->addr, tcp_addr->addr_len);
	ssize_t nr = ::send(d->m_sfd, buff, len, MSG_NOSIGNAL);
	if (nr > 0) {
		*nb_written = nr;
		return true;
	}

	return false;
}

bool tcp_connection::recvfrom(address * addr, u8 * buff, u32 len, u32 * nb_recv)
{
	// tcp_address * tcp_addr = static_cast<tcp_address *>(addr);

//    tcp_addr->addr_len = sizeof(struct sockaddr_in);
//    ssize_t nr = ::recvfrom(d->m_sfd, buff, len,
//                           MSG_WAITALL,
//                           &tcp_addr->addr,
//                           &tcp_addr->addr_len);

	ssize_t nr = ::recvfrom(d->m_sfd, buff, len,
				MSG_WAITALL,
				NULL,
				0);


	if (nr > 0) {
		*nb_recv = nr;
		return true;
	}

	return false;
}


///

class udp_address : public address
{
public:
	udp_address()
	{

	}

	virtual ~udp_address()
	{
	}

	void release()
	{
		delete this;
	}

public:
	struct sockaddr addr;
	socklen_t addr_len;
};


udp_connection::udp_connection(const char * host, const char * port)
	:
	d(new private_data(host, port))
{
	cerr << __FUNCTION__ << "\n";
}

udp_connection::~udp_connection()
{
	close();
	delete d;
}


network::address * udp_connection::new_address()
{
	return new udp_address();
}

bool udp_connection::open(open_mode mode)
{
	return d->open(mode, AF_INET, SOCK_DGRAM);
}

bool udp_connection::close()
{
	return d->close();
}

bool udp_connection::read(u8 * buff, u32 len, u32 * nb_read)
{
	return d->read(buff, len, nb_read);
}

bool udp_connection::write(const u8 * buff, u32 len, u32 * nb_written)
{
	return d->write(buff, len, nb_written);
}

connection * udp_connection::accept(/* addresse criteria */)
{
	return 0;
}


bool udp_connection::get_addr(const char * addr, const char * port,  ew::network::address ** pptr)
{
	struct addrinfo hints;
	struct addrinfo * info = NULL;
	int res = false;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family   = d->ai_family;
	hints.ai_socktype = d->ai_socktype;
	hints.ai_flags    = 0;
	hints.ai_protocol = d->ai_protocol;

	if (getaddrinfo(addr, NULL, &hints, &info) != 0) {
		cerr << "getaddrinfo error\n";

		return false;
	}


	struct addrinfo * rp = 0;

	res = false;
	for (rp = info; rp != NULL; rp = rp->ai_next) {

		if ((rp->ai_family == d->ai_family) && (rp->ai_socktype == d->ai_socktype) && (rp->ai_protocol == d->ai_protocol)) {

			udp_address * udp_addr = new udp_address();

			struct sockaddr_in * sin = (struct sockaddr_in *)&udp_addr->addr;
			memcpy(sin, rp->ai_addr, rp->ai_addrlen);
			sin->sin_port = htons(atoi(port));
			udp_addr->addr_len = sizeof(struct sockaddr_in);

			*pptr = udp_addr;

			res = true;

			cerr << "found match\n";

			break;
		}
	}



	cerr << __PRETTY_FUNCTION__ << "res = " << res << "\n";


	return res;
}


bool udp_connection::sendto(const address * addr, const u8 * buff, u32 len, u32 * nb_written)
{
	const udp_address * udp_addr = static_cast<const udp_address *>(addr);

	ssize_t nr = ::sendto(d->m_sfd, buff, len, MSG_NOSIGNAL, &udp_addr->addr, udp_addr->addr_len);
	if (nr > 0) {
		*nb_written = nr;
		return true;
	}

	return false;
}

bool udp_connection::recvfrom(address * addr, u8 * buff, u32 len, u32 * nb_recv)
{
	udp_address * udp_addr = static_cast<udp_address *>(addr);

	udp_addr->addr_len = sizeof(struct sockaddr_in);

	ssize_t nr = ::recvfrom(d->m_sfd, buff, len,
				MSG_WAITALL,
				&udp_addr->addr,
				&udp_addr->addr_len);
	if (nr > 0) {
		*nb_recv = nr;
		return true;
	}

	return false;
}


bool  udp_connection::has_data() const
{
	return wait_for_data(0);

#if 0
	fd_set read_fdset;
	FD_ZERO(&read_fdset);

	FD_SET(d->m_sfd, &read_fdset);
	struct timeval tv = { 0, 0 };

	int ret = sys_select(d->m_sfd + 1,
			     &read_fdset,
			     (fd_set *)NULL,
			     (fd_set *)NULL,
			     &tv);

	if ((ret > 0) && (FD_ISSET(d->m_sfd, &read_fdset))) {
		return true;
	}

	return false;
#endif
}

bool  udp_connection::wait_for_data(u32 timeout_ms) const
{
	fd_set read_fdset;

	FD_ZERO(&read_fdset);
	FD_SET(d->m_sfd, &read_fdset);

	struct timeval tv;

	tv.tv_sec  = (timeout_ms / 1000);
	tv.tv_usec = ((timeout_ms % 1000) * 1000) % 1000000;

	int ret = sys_select(d->m_sfd + 1,
			     &read_fdset,
			     (fd_set *)NULL,
			     (fd_set *)NULL,
			     &tv);

	if ((ret > 0) && (FD_ISSET(d->m_sfd, &read_fdset))) {
		return true;
	}

	return false;
}


bool udp_connection::auto_bind()
{
	ew::network::address * addr;
	//bool ret =
	udp_connection::get_addr("0.0.0.0", "0", &addr);

	int sys_ret;

	udp_address * udp_addr = static_cast<udp_address *>(addr);
	if (udp_addr == nullptr)
		return false;

	sys_ret = ::bind(d->m_sfd, &udp_addr->addr, udp_addr->addr_len);

	return sys_ret == 0;
}


}
} // ew::NETWORK
