#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <unistd.h>


#include <ew/core/Types.hpp>
#include <ew/core/Time.hpp>
#include <ew/Console.hpp>
#include <ew/Network.hpp>
#include <ew/core/Threading.hpp>


namespace test
{
namespace network
{

using std::size_t;
using namespace ew::network;
using namespace ew::core::threading;
using ew::console::cerr;

u32 nr_client;
mutex nr_client_mtx;
condition_variable nr_client_condvar(&nr_client_mtx);

void clientthreadFunc(connection * cl)
{
	u64 accum = 0;
	u32 nr_recv = 0;

	u64 accumw = 0;
	u32 nr_send = 0;

	cerr << __FUNCTION__ << "\n";

	u32 t0 = ew::core::time::get_ticks();

	auto from = cl->new_address();

	while (true) {
		u8  buff[1024 * 64];

		u32 rlen = 0;
		u32 wlen = 0;

		cl->wait_for_data(1000);
		auto has_data = cl->has_data();

		if (has_data) {

			//auto ret = cl->read(buff, sizeof(buff) - 1, &rlen);
			auto ret = cl->recvfrom(from, buff, sizeof(buff) - 1, &rlen);
			if (ret == true) {
				if (!rlen)
					break;
				++nr_recv;
			}

			// send back data
			ret = cl->sendto(from, buff, rlen, &wlen);
			if (ret == true) {
				if (!wlen)
					break;
				++nr_send;
			}

		}

		u32 t1 = ew::core::time::get_ticks();

		accum  += rlen;
		accumw += wlen;

		u32 delta = t1 - t0;

		if (delta >= 1000) {

			std::cerr << " delta = " << delta << "\n";
			std::cerr << " t0 = " << t0 << "\n";
			std::cerr << " t1 = " << t1 << "\n";

			std::cerr << "accum   = " << (accum >> 0)  << " bytes in " << (delta) << "\n";
			std::cerr << "accum   = " << (accum >> 10) << " kbytes in " << (delta) << "\n";
			std::cerr << "accum   = " << (accum >> 20) << " mbytes in " << (delta) << "\n";
			std::cerr << "nr_recv = " << nr_recv << "\n\n";

			std::cerr << "accumw   = " << (accumw >> 0)  << " bytes in " << (delta) << "\n";
			std::cerr << "accumw   = " << (accumw >> 10) << " kbytes in " << (delta) << "\n";
			std::cerr << "accumw   = " << (accumw >> 20) << " mbytes in " << (delta) << "\n";
			std::cerr << "nr_send  = " << nr_send << "\n\n";

			accum = 0;
			accumw = 0;
			nr_recv = 0;
			nr_send = 0;
			t0 = t1; //ew::core::time::get_ticks();
		}
	}
}

int main(int ac, char ** av)
{
	if (ac != 4) {
		cerr << "usage : " << av[0] << " [udp|tcp] host port" << "\n";
		return 1;
	}

	bool is_udp = false;
	bool is_tcp = false;

	if (!strcmp(av[1], "udp")) {
		is_udp = true;
	}
	if (!strcmp(av[1], "tcp")) {
		is_tcp = true;
	}

	ew::core::time::init();
	ew::network::init();

	std::list<thread *> cl_list;

	connection * server_conn = 0;
	if (is_udp)
		server_conn = new udp_connection(av[2], av[3]);
	if (is_tcp)
		server_conn = new tcp_connection(av[2], av[3]);

	if (!server_conn->open(mode::read_write | mode::listener)) {
		cerr << "cannot create server\n";
		return 1;
	}
	cerr << "connection ok\n";

	if (is_udp) {
		clientthreadFunc(server_conn);
		return 0;
	}

	while (true) {
		connection * client_conn = server_conn->accept();
		thread * client_thd = new thread((thread::func_t) clientthreadFunc,
						 (thread::arg_t) client_conn,
						 "clientthread");
		if (client_thd->start() == true) {
			cl_list.push_back(client_thd);
		} else {
			delete client_thd;
			delete client_conn;
		}
	}
	// wait for each thread

	delete server_conn;

	ew::network::quit();

	return 0;
}

}
}

int main(int ac, char ** av)
{
	return test::network::main(ac, av);
}
