#include "tftp.hpp"


#include <cassert>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ew/network/connection.hpp>
#include <ew/core/time/time.hpp>
#include <ew/filesystem/file.hpp>
#include <ew/utils/utils.hpp>

#include "byte_stream.hpp"

using namespace ew::core::types;
using namespace ew::core::objects::stream; // mode


//

enum tftp_opcode {
	TFTP_INVALID =  0,
	TFTP_READ    =  1,
	TFTP_WRITE   =  2,
	TFTP_DATA    =  3,
	TFTP_ACK     =  4,
	TFTP_ERR     =  5
};

class tftp_connection : public ew::network::connection
{
public:
	tftp_connection(ew::network::udp_connection * conn, char * host, char * port);

	virtual ~tftp_connection();

	virtual ew::network::udp_connection * transport_layer();

	virtual bool open(open_mode mode);
	virtual bool read(u8 * buff, u32 len, u32 * nb_read);
	virtual bool write(const u8 * buff, u32 len, u32 * nb_written);
	virtual bool close();

	// stream oriented
	virtual tftp_connection * accept();

	virtual bool reopen(ew::core::objects::stream::open_mode);
	virtual ew::core::objects::stream::open_mode get_open_mode();
	virtual bool is_opened();
	virtual bool is_closed();

	virtual bool set_position(ew::core::types::u64, ew::core::objects::stream::location, ew::core::objects::stream::direction);
	virtual ew::core::types::u64 get_position();

	virtual bool get_addr(const char * addr, const char * port,  ew::network::address ** pptr);

	ew::network::address * new_address();

	virtual bool has_data() const
	{
		if (!m_conn)
			return false;

		return m_conn->has_data();
	}

	virtual bool sendto(const ew::network::address * to_addr, const u8 * buff, const u32 len, u32 * nb_sent)
	{
		return m_conn->sendto(to_addr, buff, len, nb_sent);
	}

	virtual bool recvfrom(ew::network::address * from_addr, u8 * buff, u32 len, u32 * nb_read)
	{
		return m_conn->recvfrom(from_addr, buff, len, nb_read);
	}

public:
	char * m_host;
	char * m_port;
	ew::network::address * m_srv_addr;
	ew::network::udp_connection * m_conn;
};

tftp_connection::tftp_connection(ew::network::udp_connection * conn, char * host, char * port)
{
	m_srv_addr = nullptr;
	m_host = ew::utils::c_string_dup(host);
	m_port = ew::utils::c_string_dup(port);
	m_conn = conn;
}

tftp_connection::~tftp_connection() { }


ew::network::udp_connection * tftp_connection::transport_layer()
{
	return m_conn;
}

bool tftp_connection::open(open_mode mode)
{
	return transport_layer()->open(mode);
}


bool tftp_connection::read(u8 * buff, u32 len, u32 * nb_read)
{
	return false;
}

bool tftp_connection::write(const u8 * buff, u32 len, u32 * nb_written)
{
	return false;
}

bool tftp_connection::close()
{
	return false;
}


tftp_connection * tftp_connection::accept()
{
	return nullptr;
}

bool tftp_connection::reopen(ew::core::objects::stream::open_mode)
{
	return false;
}

ew::core::objects::stream::open_mode tftp_connection::get_open_mode()
{
	return ew::core::objects::stream::mode::invalid_mode;
}

bool tftp_connection::is_opened()
{
	return false;
}

bool tftp_connection::is_closed()
{
	return false;
}

bool tftp_connection::set_position(ew::core::types::u64, ew::core::objects::stream::location, ew::core::objects::stream::direction)
{
	return false;
}

ew::core::types::u64 tftp_connection::get_position()
{
	return 0;
}

bool tftp_connection::get_addr(const char * addr, const char * port, ew::network::address ** pptr)
{
	return false;
}

ew::network::address * tftp_connection::new_address()
{
	return transport_layer()->new_address();
}


// tftp.cpp
int tftp_client_prompt(char * buff, size_t maxlen)
{
	std::cerr << "TFTP> ";

	buff[0] = 0;

	int len = read(0, buff, maxlen);
	buff[maxlen - 1] = 0;

	if ((len > 0) && buff[len - 1] == '\n') {
		buff[len - 1] = '\0';
	}

	if (len == 0) {
		return -1;
	}
	return len;
}

typedef int (*tftp_do_op)(bool * loop, tftp_connection * conn, char * cmd, size_t cmdlen);

// ---------- TFTP READ ----------

// TODO:
// add on_data() callback
// add on_err()  callback
//

typedef enum  read_state_e {
	send_rrq        = 0,
	wait_data,
	send_ack,
	read_ok,
	read_error,
	last_read_state_index = read_error
} read_state_t;

typedef struct tftp_read_machine_s {
	tftp_read_machine_s(tftp_connection * _conn,
			    const char * _filename,
			    const size_t _filename_len,
			    ew::core::objects::write_interface * _out)
		: filename(_filename)
		, filename_len(_filename_len)
	{
		conn = _conn;
		data_from = conn->new_address();
		out = _out;

		std::cerr << "init read machine file(" << filename << ")[" << filename_len << "]\n";

		st = send_rrq;

		ret = 1;                // machine return code 0 => no error
		read_loop = true;      // running flag


		last_recv_block = 0;
		expected_block = 0;
		is_last_block = false;
		nr_send_try = 0;
		nr_timeout  = 0;

	}

	~tftp_read_machine_s()
	{
		if (data_from)
			data_from->release();
	}

	ew::core::objects::write_interface * out;
	const char * filename;
	const size_t filename_len;

	tftp_connection * conn;
	ew::network::address * data_from; // srv data link addr

	int ret = 1;                // machine return code 0 => no error
	bool read_loop = true;      // running flag
	read_state_t st; // current state
	u8 buffer[4 + 512];         // tmp buffer

	u16  last_recv_block;
	u16  expected_block;
	bool is_last_block;
	u32  nr_send_try;
	u32  nr_timeout;

} tftp_read_machine_t;

typedef int (*tftp_read_machine_state_cb)(tftp_read_machine_t * machine);


int read_state_send_rrq_cb(tftp_read_machine_t * machine)
{
	ew::network::udp_connection * conn = machine->conn->transport_layer();

	std::cerr << " machine send_rrq filename = '" << machine->filename << "'\n";

	++machine->nr_send_try;

	// prepare read_request
	u8 * p = machine->buffer;
	u8 * pend = machine->buffer + sizeof(machine->buffer);
	bool build_ret;

	build_ret = insert_byte(p, pend, &p, 0);
	if (build_ret == true)
		build_ret = insert_byte(p, pend, &p, TFTP_READ);
	if (build_ret == true)
		build_ret = insert_c_string(p, pend, &p, machine->filename, machine->filename_len);
	if (build_ret == true)
		build_ret = insert_c_string(p, pend, &p, "octet", 5);

	if (build_ret == true) {
		u32 len = p - machine->buffer;
		u32 nr_written = 0;

		// conn->end_point_addr()
		bool ret = conn->sendto(machine->conn->m_srv_addr, machine->buffer, len, &nr_written);
		if (ret  == true) {
			machine->last_recv_block = 0;
			machine->expected_block = 1;
			machine->st =  wait_data;
		}
	}

	if (build_ret == false) {
		// sleep before retry
		ew::core::time::sleep(2000);

		if (machine->nr_send_try == 3) {
			machine->st = read_error;
		}
	}

	return 0;
}

int read_state_wait_data_cb(tftp_read_machine_t * machine)
{
	ew::network::udp_connection * conn = machine->conn->transport_layer();

	u32 nr_read = 0;
	u32 len = sizeof(machine->buffer);
	bool ret = true;
	ret = conn->wait_for_data(2000);
	if (ret == true) {

		ret = conn->recvfrom(machine->data_from, machine->buffer, len, &nr_read);

		if (ret  == true) {

			// decode_tftp_data();
			{
				u8 * p = machine->buffer;
				u8 * pend = machine->buffer + nr_read;
				u16 tmp_block_num;

				ret = expect_byte(p, pend, &p, 0);
				if (ret == true)
					ret = expect_byte(p, pend, &p, TFTP_DATA);
				if (ret == true)
					ret = extract_u16(p, pend, &p, &tmp_block_num);

				if (ret == true) {

					u64 nb_written = 0;
					// TODO: machine->data_cb()
					ret = machine->out->write(p, (u64)(pend - p), &nb_written);
					if (ret != true) {
						std::cerr << "cannot write to file...\n";
						machine->st = read_error;
						return 1;
					}

					if ((pend - p) < 512) {
						machine->is_last_block = true;
					}

					if (tmp_block_num != machine->expected_block) {
						// st = error invalid block num/sequence
					} else {
						machine->last_recv_block = tmp_block_num;
					}

					machine->nr_timeout = 0;
					machine->st = send_ack;
				}
			}
		}
	} else {
		++machine->nr_timeout;
		if (machine->nr_timeout == 3) {
			machine->st = read_error;
		} else {
			machine->st = send_ack;
		}

		std::cerr << " no data\n";
	}

	return 0;
}

int read_state_send_ack_cb(tftp_read_machine_t * machine)
{
	ew::network::udp_connection * conn = machine->conn->transport_layer();

	// prepare read_request
	u8 * p = machine->buffer;
	u8 * pend = machine->buffer + sizeof(machine->buffer);
	bool build_ret;

	build_ret = insert_byte(p, pend, &p, 0);
	if (build_ret == true)
		build_ret = insert_byte(p, pend, &p, TFTP_ACK);
	if (build_ret == true)
		build_ret = insert_byte(p, pend, &p, machine->last_recv_block >> 8);
	if (build_ret == true)
		build_ret = insert_byte(p, pend, &p, machine->last_recv_block & 0xFF);

	if (build_ret == true) {
		u32 len = p - machine->buffer;
		u32 nr_written = 0;

		bool ret = conn->sendto(machine->data_from, machine->buffer, len, &nr_written);
		if (ret  == true) {
			machine->st =  wait_data;
			machine->expected_block = machine->last_recv_block + 1;
			if (machine->is_last_block == true) {
				machine->st = read_ok;
			}
		} else {
			// sleep before retry
			ew::core::time::sleep(1000);
		}
	}

	return 0;
}

int read_state_read_ok_cb(tftp_read_machine_t * machine)
{
	machine->read_loop = false;
	machine->ret = 0;
	return 0;
}

int read_state_read_error_cb(tftp_read_machine_t * machine)
{
	std::cerr << " read error \n";
	machine->read_loop = false;
	ew::core::time::sleep(1000);
	return 0;
}


int tftp_recvfile(tftp_connection * conn,
		  const char * filename,
		  const size_t filename_len,
		  ew::core::objects::write_interface * out)
{
	int ret = 0;
	tftp_read_machine_t * machine = new tftp_read_machine_t(conn, filename, filename_len, out);

	tftp_read_machine_state_cb  state_cb[last_read_state_index + 1];

	// use callback table
	state_cb[send_rrq]   = read_state_send_rrq_cb;
	state_cb[wait_data]  = read_state_wait_data_cb;
	state_cb[send_ack]   = read_state_send_ack_cb;
	state_cb[read_ok]    = read_state_read_ok_cb;
	state_cb[read_error] = read_state_read_error_cb;

	while (machine->read_loop == true) {
		ret = state_cb[machine->st](machine);
	}

	ret = machine->ret;
	delete machine;
	return ret;
}

int tftp_client_process_read(bool * loop, tftp_connection * conn, char * cmd, size_t cmdlen)
{
	char * filename;
	if (cmd[3] != ' ') {
		std::cerr << "usage : read filename" << "\n";
		return 1;
	}
	filename = &cmd[4];

	size_t filename_len = strlen(filename);

	ew::filesystem::file * out = nullptr;

	out = new ew::filesystem::file(filename);

	if (out->exists()) {
		std::cerr << "Error : " << filename << " exists\n";
		return 1;
	}

	std::cerr << "try to create '" << filename << "'";
	if (out->create(mode::read_write) != true) {
		std::cerr << "\nError : failed to create " << filename << "\n";
		return 1;
	}

	std::cerr << " read => filename '" << filename << "' len '" << filename_len << "'\n";
	std::cerr << " out ptr =>  '" << out << "'\n";

	int ret = tftp_recvfile(conn,
				filename,
				filename_len,
				out);

	if (out) {
		delete out;
	}

	return ret;
}

// ---------- TFTP WRITE ----------

typedef enum  write_state_e {
	send_wrq        = 0,
	wait_ack,
	send_data,
	write_ok,
	write_error,
	last_write_state_index = write_error
} write_state_t;

typedef struct tftp_write_machine_s {
	tftp_write_machine_s(tftp_connection * _conn,
			     const char * _filename,
			     const size_t _filename_len,
			     ew::core::objects::read_interface * _in)
		: filename(_filename)
		, filename_len(_filename_len)
	{
		conn = _conn;
		data_from = conn->new_address();
		in = _in;

		std::cerr << "init write machine file(" << filename << ")[" << filename_len << "]\n";

		st = send_wrq;

		ret = 1;                // machine return code 0 => no error
		write_loop = true;      // running flag

		last_sent_block = 0;
		expected_block = 0;
		is_last_block = false;
		nr_send_try = 0;
		nr_timeout  = 0;

	}

	~tftp_write_machine_s()
	{
		if (data_from)
			data_from->release();
	}

	ew::core::objects::read_interface * in;
	const char * filename;
	const size_t filename_len;

	tftp_connection * conn;
	ew::network::address * data_from; // srv data link addr

	int ret = 1;                // machine return code 0 => no error
	bool write_loop = true;      // running flag
	write_state_t st; // current state
	u8 buffer[4 + 512];         // tmp buffer
	u8 * buffer_end; // pointer to end of data to send

	u16  last_sent_block;
	u16  expected_block;
	bool is_last_block;
	u32  nr_send_try;
	u32  nr_timeout;

} tftp_write_machine_t;

typedef int (*tftp_write_machine_state_cb)(tftp_write_machine_t * machine);

int write_state_send_wrq_cb(tftp_write_machine_t * machine)
{
	ew::network::udp_connection * conn = machine->conn->transport_layer();

	std::cerr << " machine send_wrq filename = '" << machine->filename << "'\n";

	++machine->nr_send_try;

	// prepare write_request
	u8 * p = machine->buffer;
	u8 * pend = machine->buffer + sizeof(machine->buffer);
	bool build_ret;

	build_ret = insert_byte(p, pend, &p, 0);
	if (build_ret == true)
		build_ret = insert_byte(p, pend, &p, TFTP_WRITE);
	if (build_ret == true)
		build_ret = insert_c_string(p, pend, &p, machine->filename, machine->filename_len);
	if (build_ret == true)
		build_ret = insert_c_string(p, pend, &p, "octet", 5);
	if (build_ret == true)
		build_ret = insert_c_string(p, pend, &p, "blksize", 7);
	if (build_ret == true)
		build_ret = insert_c_string(p, pend, &p, "512", 3);

	if (build_ret == true) {
		u32 len = p - machine->buffer;
		u32 nr_written = 0;

		// conn->end_point_addr()
		bool ret = conn->sendto(machine->conn->m_srv_addr, machine->buffer, len, &nr_written);
		if (ret  == true) {
			machine->last_sent_block = 0;
			machine->expected_block = 0;
			machine->st =  wait_ack;
		}
	}

	if (build_ret == false) {
		// sleep before retry
		ew::core::time::sleep(2000);

		if (machine->nr_send_try == 3) {
			machine->st = write_error;
		}
	}

	return 0;
}

int write_state_wait_ack_cb(tftp_write_machine_t * machine)
{
	ew::network::udp_connection * conn = machine->conn->transport_layer();

	u32 nr_read = 0;
	u32 len = sizeof(machine->buffer);
	bool ret = true;

	ret = conn->wait_for_data(2000);
	if (ret == true) {

		ret = conn->recvfrom(machine->data_from, machine->buffer, len, &nr_read);
		if (ret  == true) {

			// decode_tftp_ack();
			{
				u8 * p = machine->buffer;
				u8 * pend = machine->buffer + nr_read;
				u16 tmp_block_num = 0;
				u16 op = 0;

				ret = extract_u16(p, pend, &p, &op);
				std::cerr << " Op = " << op << "\n";
				if (op == 6) {

					machine->st = send_data;

				} else if (op == TFTP_ACK) {

					if (ret == true) {
						ret = extract_u16(p, pend, &p, &tmp_block_num);
						std::cerr << " tmp_block_num = " << tmp_block_num << "\n";
					}
				}

				if (ret == true) {

					if (tmp_block_num != machine->expected_block) {
						ret = false;

						std::cerr << " unexpected blocknum\n";

						// TODO: send TFTP_ERROR invalid  block number ...
						machine->st = write_error;
					}
				}


				if (ret == true)

					if (machine->is_last_block == true) {
						machine->st = write_ok;
						return 0;
					}

				// TODO: assert/check no more data after block num field
				if (ret == true) {

					machine->last_sent_block = machine->expected_block;

					u64 nb_read = 0;
					// TODO: machine->read_data_cb()
					ret = machine->in->read(p, 512, &nb_read);
					if (ret != true) {
						std::cerr << "cannot read to file...\n";
						machine->st = write_error;
						return 1;
					}

					if (nb_read < 512) {
						machine->is_last_block = true;
					}

					machine->buffer_end = machine->buffer + 4 + nb_read;
					machine->nr_timeout = 0;
					machine->st = send_data;
				}
			}
		}

	} else {

		++machine->nr_timeout;
		if (machine->nr_timeout == 3) {
			machine->st = write_error;
		} else {
			machine->st = wait_ack;
		}

		std::cerr << " no ack\n";
	}

	return 0;
}

int write_state_send_data_cb(tftp_write_machine_t * machine)
{
	ew::network::udp_connection * conn = machine->conn->transport_layer();

	// prepare write_request
	u8 * p = machine->buffer;
	u8 * pend = machine->buffer + sizeof(machine->buffer);
	bool build_ret;

	u16 block_num = machine->last_sent_block + 1;

	if (block_num == 2) {
		ew::core::time::sleep(3000);
		return 0;
	}

	build_ret = insert_byte(p, pend, &p, 0);
	if (build_ret == true)
		build_ret = insert_byte(p, pend, &p, TFTP_DATA);
	if (build_ret == true)
		build_ret = insert_byte(p, pend, &p, block_num >> 8);
	if (build_ret == true)
		build_ret = insert_byte(p, pend, &p, block_num & 0xFF);

	if (build_ret == true) {
		u32 len = machine->buffer_end - machine->buffer;
		u32 nr_written = 0;

		bool ret = conn->sendto(machine->data_from, machine->buffer, len, &nr_written);
		if (ret  == true) {
			machine->st =  wait_ack;
			machine->expected_block = machine->last_sent_block + 1;

		} else {
			// sleep before retry
			ew::core::time::sleep(1000);
		}
	}

	return 0;
}

int write_state_write_ok_cb(tftp_write_machine_t * machine)
{
	machine->write_loop = false;
	machine->ret = 0;
	return 0;
}

int write_state_write_error_cb(tftp_write_machine_t * machine)
{
	std::cerr << " write error\n";
	machine->write_loop = false;
	return 0;
}



int tftp_sendfile(tftp_connection * conn,
		  const char * filename,
		  const size_t filename_len,
		  ew::core::objects::read_interface * in)
{
	int ret = 0;
	tftp_write_machine_t * machine = new tftp_write_machine_t(conn, filename, filename_len, in);
	tftp_write_machine_state_cb  state_cb[last_write_state_index + 1];

	// use callback table
	state_cb[send_wrq]    = write_state_send_wrq_cb;
	state_cb[wait_ack]    = write_state_wait_ack_cb;
	state_cb[send_data]   = write_state_send_data_cb;
	state_cb[write_ok]    = write_state_write_ok_cb;
	state_cb[write_error] = write_state_write_error_cb;

	while (machine->write_loop == true) {
		ret = state_cb[machine->st](machine);
	}

	ret = machine->ret;
	delete machine;
	return ret;
}

int tftp_client_process_write(bool * loop, tftp_connection * conn, char * cmd, size_t cmdlen)
{
	char * filename;
	if (cmd[3] != ' ') {
		std::cerr << "usage : put filename" << "\n";
		return 1;
	}
	filename = &cmd[4];

	size_t filename_len = strlen(filename);

	ew::filesystem::file * in = nullptr;

	in = new ew::filesystem::file(filename);

	if (in->exists() == false) {
		std::cerr << "Error : " << filename << " doest not exists\n";
		return 1;
	}

	std::cerr << "try to open '" << filename << "'";
	if (in->open(mode::read) != true) {
		std::cerr << "\nError : failed to create " << filename << "\n";
		return 1;
	}

	std::cerr << " write => filename '" << filename << "' len '" << filename_len << "'\n";
	std::cerr << " in ptr =>  '" << in << "'\n";

	int ret = tftp_sendfile(conn,
				filename,
				filename_len,
				in);
	if (in) {
		delete in;
	}

	return ret;
}

int tftp_client_process_quit(bool * loop, tftp_connection * conn, char * cmd, size_t cmdlen)
{
	*loop = false;
	return 0;
}

int tftp_client_process_command(bool * loop, tftp_connection * conn, char * cmd, size_t cmdlen)
{
	struct cmd_desc {
		const char * cmd_str;
		size_t len;
		tftp_do_op do_op;
	};

	cmd_desc cmd_tab[] = {
		{ "get" , 3, tftp_client_process_read },
		{ "put", 3, tftp_client_process_write },
		{ "quit",  4, tftp_client_process_quit },
		{ nullptr , 0, nullptr }
	};

	std::cerr << " your command is '" << cmd << "'\n";

	cmd_desc * curr_cmd = nullptr;

	for (size_t i = 0; cmd_tab[i].cmd_str != nullptr; i++) {
		if (strncmp(cmd_tab[i].cmd_str, cmd, cmd_tab[i].len) == 0) {
			curr_cmd = &cmd_tab[i];
			std::cerr << " found @ index '" << i << "'\n";
			break;
		}
	}

	if (curr_cmd != nullptr) {
		curr_cmd->do_op(loop, conn, cmd, cmdlen);
	}

	return 0;
}

int tftp_client_mainloop(tftp_connection * conn)
{
	char buff[1024];
	bool loop = true;
	while (loop == true) {
		int len = tftp_client_prompt(buff, sizeof(buff));
		if (len < 0) {
			loop = false;
			break;
		} else {

			tftp_client_process_command(&loop, conn, buff, len);
		}
	}

	std::cerr << "quit\n";
	return 0;
}

int tftp_client(char * host, char * port)
{
	tftp_connection * conn;

	ew::network::udp_connection * udp_conn = new ew::network::udp_connection(host,  port);

	conn = new tftp_connection(udp_conn, host, port);
	if (!conn->open(mode::read_write)) {
		std::cerr << "cannot contact server\n";
		return 1;
	}

	conn->transport_layer()->get_addr(host, port, &conn->m_srv_addr);
	return tftp_client_mainloop(conn);
}

