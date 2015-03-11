#include <cassert>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <list>
#include <thread>


#include <ew/network/connection.hpp>
#include <ew/core/time/time.hpp>
#include <ew/filesystem/file.hpp>


// TODO: #include "raw_connection.hpp"

#include "tftp.hpp"

using namespace ew::network;


// TODO: move to raw_connection.cpp
int raw_udp_client(const char * host, const char *port, ew::network::address * m_srv_addr, u32 start_size = 1024, bool fixed_size_flag = false, u32 sleep_after_send = 0)
{
	std::cerr << "client connection ok\n";
	size_t buff_max_sz = 65535 - 8 - 20;
	u8  * buff = new u8[buff_max_sz];
	size_t buff_sz = start_size;
	s32    buff_grow = 1;

	u64 accum   = 0;
	u32 nr_send = 0;

	u64 accumr  = 0;
	u32 nr_recv = 0;


	ew::network::connection * conn = new udp_connection(host, port);
	conn->open(mode::read_write);


	bool ret = ((udp_connection *)conn)->get_addr(host, port, &m_srv_addr);
	if (ret == false) {
		std::cerr << "cannot get host["<< host <<"] addr\n";
		return false;
	}


	float   X[2];
	float   Y[2];
	float coef = 1.0f;
	u32   nr_points = 0;

	u32 t0 = ew::core::time::get_ticks();
	while (true) {
		u32 rlen = 0;
		u32 wlen = 0;

		rlen = buff_sz;

		// do stuff
		memset(buff, (int)(t0), buff_max_sz);

		// send data
		bool ret = ((udp_connection *)conn)->sendto(m_srv_addr, buff, buff_sz, &wlen);
		u32 t1 = ew::core::time::get_ticks();
		u32 delta = t1 - t0;
		if (ret == false) {
			std::cerr << "sendto error for buff_sz = " << buff_sz << "\n";
			continue;
		}
		++nr_send;

		if (sleep_after_send) {
			ew::core::time::sleep(sleep_after_send);
		}

		// read back data
		if (conn->has_data() == true) {
			rlen = 0;
			bool ret = ((udp_connection *)conn)->recvfrom(m_srv_addr, buff, wlen, &rlen);
			if (ret == false) {
				std::cerr << "recvfrom error for buff_sz = " << buff_sz << "\n";
				continue;
			}
			++nr_recv;
		}

		/// accounting
		accum  += wlen;
		accumr += rlen;

		if (delta >= 1000) {

			if (fixed_size_flag == false) {
				if (nr_points < 2) {
					X[nr_points] = buff_sz >> 10;
					Y[nr_points] = accum >> 10;
					nr_points++;
				} else {

					float dy = (Y[1] - Y[0]);
					float dx = (X[1] - X[0]);
					if (dx) {
						coef = dy / dx;
					} else {
						coef = (dy < 0.0f) ? -1.0f : 1.0f;
					}

					if (coef < 1.0f) {
						buff_grow *= -1;
					}

					std::cerr << "coef (" << coef << ")\n";

					X[0] = X[1];
					Y[0] = Y[1];
					nr_points = 1;
				}
			}

			std::cerr << "-------------------------------------\n";

			std::cerr << "sleep_after_send = " << sleep_after_send << "\n";

			std::cerr << "buff_sz   = " << (buff_sz >> 0) << " bytes\n";
			std::cerr << "buff_sz   = " << (buff_sz >> 10) << " kbytes\n";
			std::cerr << "buff_sz   = " << (buff_sz >> 20) << " mbytes\n";

			std::cerr << "accum   = " << (accum >> 0) << " bytes in " << (delta) << "\n";
			std::cerr << "accum   = " << (accum >> 10) << " kbytes in " << (delta) << "\n";
			std::cerr << "accum   = " << (accum >> 20) << " mbytes in " << (delta) << "\n";
			std::cerr << "nr_send = " << nr_send << "\n\n";

			std::cerr << "accumr   = " << (accumr >> 0) << " bytes in " << (delta) << "\n";
			std::cerr << "accumr   = " << (accumr >> 10) << " kbytes in " << (delta) << "\n";
			std::cerr << "accumr   = " << (accumr >> 20) << " mbytes in " << (delta) << "\n";
			std::cerr << "nr_recv = " << nr_recv << "\n\n";


			std::cerr << "buff_grow(" << buff_grow << ")\n";

			if (fixed_size_flag == false) {
				if (buff_grow > 0) {
					buff_sz += 1024;
				} else {
					buff_sz -= 256;
				}
			}

			// restart timer
			t0 = ew::core::time::get_ticks();
			nr_send = 0;
			accum = 0;

			accumr = 0;
			nr_recv = 0;
		}

		if (buff_sz >= buff_max_sz) {
			buff_sz = buff_max_sz;
		}
	}

	m_srv_addr->release();
	delete conn;
	return 0;
}

int raw_tcp_client(const char * host, const char *port, u32 start_size = 1024, bool fixed_size_flag = false, u32 sleep_after_send = 0)
{
	std::cerr << "client connection ok\n";
	size_t buff_max_sz = 65535 - 8 - 20;
	u8  * buff = new u8[buff_max_sz];
	size_t buff_sz = start_size;


	ew::network::connection * conn = new tcp_connection(host, port);
	conn->open(mode::read_write);


	u32 t0 = ew::core::time::get_ticks();
	while (true) {

		if (1) {
			u32 wlen = 0;

			// do stuff
			memset(buff, (int)(t0), buff_max_sz);
			buff_sz = snprintf((char*)buff,  buff_max_sz, "hello world !\n");

			// send data
			std::cerr << "send data\n";

			bool ret = conn->sendto(0,  buff, buff_sz, &wlen);
			if (ret == false) {
				std::cerr << "sendto error for buff_sz = " << buff_sz << "\n";
				ew::core::time::sleep(1000);

				std::cerr << "reopen\n";

				conn->close();
				auto ret = conn->open(mode::read_write);
				std::cerr << "conn->open(mode::read_write) -> "<< ret << "\n";
				continue;
			}

			std::cerr << "wlen = " << wlen << "\n";


		}

		if (sleep_after_send) {
			ew::core::time::sleep(sleep_after_send);
		}

	}

	std::cerr << "client quitting\n";

	delete conn;
	return 0;
}



////
int main(int ac, char ** av)
{
	int  ret = 0;
	bool is_udp = false;
	bool is_tcp = false;
	bool is_tftp = false;
	u32  start_size = 1024;
	bool fixed_size_flag = false;
	u32  sleep_after_send = 0;
	u32  nr_conn = 1;

	ew::core::time::init();

	if (ac < 4 || ac > 8) {
		std::cerr << "usage : " << av[0] << " [udp|tcp|tftp] host port [nr_connection] [buffer_start_size] [fixed_size_flag] [sleep_after_send]" << "\n";
		return 1;
	}


	if (!strcmp(av[1], "udp")) {
		is_udp = true;
	}
	if (!strcmp(av[1], "tcp")) {
		is_tcp = true;
	}
	if (!strcmp(av[1], "tftp")) {
		is_tftp = true;
	}

	auto host = av[2];
	auto port = av[3];

	if (ac >= 5) {
		nr_conn = atoi(av[4]);
	}

	if (ac >= 6) {
		start_size = atoi(av[5]);
	}

	if (ac >= 7) {
		fixed_size_flag = atoi(av[6]);
	}

	if (ac >= 8) {
		sleep_after_send = atoi(av[7]);
	}

	if (is_tftp) {
		return tftp_client(host, port);
	}

	std::list<std::thread> thread_list;

	for (decltype(nr_conn) i = 0; i < nr_conn; ++i) {

		auto th = std::thread([=] () -> bool {
			auto th_id = i;
			std::cerr << " starting thread = " << th_id << "\n";

			if (is_tcp)
			{
				return raw_tcp_client(host, port, start_size, fixed_size_flag, sleep_after_send);
			}

			if (is_udp)
			{

				return raw_udp_client(host, port, 0, start_size, fixed_size_flag, sleep_after_send);
			}
			return false;
		});

		th.detach();
	}

	pause();


	return ret;
}
