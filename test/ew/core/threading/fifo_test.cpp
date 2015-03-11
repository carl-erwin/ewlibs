#include <iostream>
#include <ew/Core.hpp>
#include <list>

// -------------------------------

using namespace ew::core;
using namespace ew::core::time;
using namespace ew::core::threading;


struct Test {
	u8 array[64];
};

#define TYPE Test

namespace ew
{

namespace test
{
template <class T>
class test_fifo
{
	typedef T type_val;

public:
	test_fifo()
	{
		m_putcond = new condition_variable(&m_mtx);
	}


	bool put(T val)
	{
		m_mtx.lock();
		m_list.push_back(val);
		m_putcond->signal();
		m_mtx.unlock();

		return true;
	}

	bool get(const u32 timeout, T & val)
	{
		bool ret = false;

		m_mtx.lock();
		{
			if (m_list.size() == 0) {
				m_putcond->timed_wait(timeout);
			}

			if (m_list.size() > 0) {
				val = *(m_list.rbegin());
				m_list.pop_back();
				ret = true;
			}
		}
		m_mtx.unlock();

		return ret;
	}

private:
	mutex m_mtx;
	condition_variable * m_putcond = nullptr;
	std::list<T> m_list;
};


void * produce(test_fifo<TYPE> * fifo)
{
	TYPE val;
//            u32 t0, t1;

	for (;;) {
		fifo->put(val);
		memset(&val, 0, sizeof(val));
	}

	return nullptr;
}


void * consume(test_fifo<TYPE> * fifo)
{
	TYPE val;
	u32 t0, t1;
	u32 accum = 0;

	t0 = ew::core::time::get_ticks();
	for (;;) {
		if (fifo->get(1000, val) == true) {
			++accum;
		}

		t1 = ew::core::time::get_ticks();
		if (t1 - t0 >= 1000) {

			std::cerr << "nr push  = " << (accum) << "\n";

			accum *= sizeof(TYPE);
			std::cerr << "accum = " << (accum) << " bytes\n";
			std::cerr << "accum = " << (accum >> 10) << " kbytes\n";
			std::cerr << "accum = " << (accum >> 20) << " mbytes\n";
			t0 = t1;
			accum = 0;
		}
	}

	return nullptr;
}


int main(int ac, char ** av)
{
	ew::core::time::init();

	test_fifo<TYPE> fifo;
	// start to thread to exchange byte messages
	ew::core::threading::thread * producer = nullptr;
	ew::core::threading::thread * consumer = nullptr;



	producer = new ew::core::threading::thread((thread::func_t)produce,
			(thread::arg_t)&fifo, "");
	consumer = new ew::core::threading::thread((thread::func_t)consume,
			(thread::arg_t)&fifo, "");
	consumer->start();
	producer->start();


	while (1) {
		pause();
	}

	return (0);
}

} // ! namespace test


} // ! namespace ew

int main(int ac, char ** av)
{
	return ew::test::main(ac, av);
}
