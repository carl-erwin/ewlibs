#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/core/interface/stream_interface.hpp>


namespace ew
{
namespace network
{

using namespace ew::core::types;
using namespace ew::core::objects::stream;



/*
 *
 */
class EW_NETWORK_EXPORT address
{
public:
	virtual ~address() {}
	virtual void release() = 0;
};

/*
 *
 */
class EW_NETWORK_EXPORT connection :
	public ew::core::objects::stream_object
{
public:
	virtual bool open(open_mode mode) = 0;
	virtual bool read(u8 * buff, u32 len, u32 * nb_read) = 0;
	virtual bool write(const u8 * buff, u32 len, u32 * nb_written) = 0;
	virtual bool close() = 0;

	// stream oriented
	virtual connection * accept() = 0;

	// TODO: from stream_object ??
	virtual bool reopen(ew::core::objects::stream::open_mode mode)
	{
		close();
		return open(mode);
	}

	virtual ew::core::objects::stream::open_mode get_open_mode() = 0;
	virtual bool is_opened() = 0;
	virtual bool is_closed() = 0;

	virtual bool set_position(ew::core::types::u64, ew::core::objects::stream::location, ew::core::objects::stream::direction) = 0;
	virtual ew::core::types::u64 get_position() = 0;

	// TODO: user call address->release();
	virtual bool get_addr(const char * addr, const char * port,  ew::network::address ** pptr) = 0;
	virtual bool wait_for_data(u32 timeout_ms) const
	{
		return false;
	}
	virtual bool has_data() const = 0;
	virtual network::address * new_address() = 0;

	virtual bool sendto(const ew::network::address * to_addr, const u8 * buff, const u32 len, u32 * nb_sent) = 0;
	virtual bool recvfrom(ew::network::address * from_addr, u8 * buff, u32 len, u32 * nb_read) = 0;
};

// move to namespace ip::tcp and proper header
// and rename ew::network::tcp::connection, ew::network::udp::connection

using namespace ew::core::objects;
class EW_NETWORK_EXPORT tcp_connection : public ew::network::connection
{
public:
	tcp_connection(const char * host, const char * port);
	virtual ~tcp_connection();

	virtual bool open(open_mode mode);
	virtual bool read(u8 * buff, u32 len, u32 * nb_read);
	virtual bool write(const u8 * buff, u32 len, u32 * nb_written);
	virtual bool close();

	// server
	virtual connection * accept();


	// TODO:
	virtual ew::core::objects::stream::open_mode get_open_mode()
	{
		return mode::invalid_mode;
	}

	// TODO:
	virtual bool is_opened()
	{
		return false;
	}

	// TODO:
	virtual bool is_closed()
	{
		return false;
	}

	// not allowed
	virtual bool set_position(ew::core::types::u64, ew::core::objects::stream::location, ew::core::objects::stream::direction)
	{
		return false;
	}

	// TODO: returns the amount of data read so far
	virtual ew::core::types::u64 get_position()
	{
		return 0;
	}

	virtual bool get_addr(const char * addr, const char * port, class address ** pptr)
	{
		return false;   // TODO
	}

	virtual network::address * new_address();

	virtual bool has_data() const
	{
		return true;    // TODO
	}

	// NEW
	virtual bool sendto(const ew::network::address * to_addr, const u8 * buff, const u32 len, u32 * nb_sent);
	virtual bool recvfrom(ew::network::address * from_addr, u8 * buff, u32 len, u32 * nb_read);

private:
	class private_data;
	private_data * d;
};

/*
 *
 */
using namespace ew::core::objects;
class EW_NETWORK_EXPORT udp_connection : public ew::network::connection
{
public:
	udp_connection(const char * host, const char * port);
	virtual ~udp_connection();

	virtual bool open(open_mode mode);
	virtual bool read(u8 * buff, u32 len, u32 * nb_read);
	virtual bool write(const u8 * buff, u32 len, u32 * nb_written);
	virtual bool close();


	// server return nullptr in udp
	virtual connection * accept();

	// not allowed for now must be sub-classed
	virtual bool reopen(ew::core::objects::stream::open_mode)
	{
		return false;
	}

	// TODO:
	virtual ew::core::objects::stream::open_mode get_open_mode()
	{
		return mode::invalid_mode;
	}

	// TODO:
	virtual bool is_opened()
	{
		return false;
	}

	// TODO:
	virtual bool is_closed()
	{
		return false;
	}

	// not allowed
	virtual bool set_position(ew::core::types::u64, ew::core::objects::stream::location, ew::core::objects::stream::direction)
	{
		return false;
	}

	// TODO: returns the amount of data read so far
	virtual ew::core::types::u64 get_position()
	{
		return 0;
	}


	virtual bool get_addr(const char * addr, const char * port, class address ** pptr);

	virtual network::address * new_address();

	virtual bool has_data() const;
	virtual bool wait_for_data(u32 timeout_ms) const; // hack

	virtual bool auto_bind();

	virtual bool sendto(const ew::network::address * to_addr, const u8 * buff, const u32 len, u32 * nb_sent);
	virtual bool recvfrom(ew::network::address * from_addr, u8 * buff, u32 len, u32 * nb_read);

private:
	class private_data;
	private_data * d;
};



}
}
