#include <cstdlib>
#include <list>
#include <iostream>

#include <QtCore/QList>
#include <QtCore/QLinkedList>

#include <sys/time.h>
#include <time.h>

#define TEST1
#define TEST2
#define TEST3

namespace playground
{

template <typename T,  typename _Alloc = std::allocator<T> >

class List
{

	class Node
	{
	public:
		Node(const T & val)
		{
			m_val = val;
		}

		Node * m_next;
		Node * m_prev;
		T   m_val;
	};

public:

	class iterator
	{
	public:
		Node * current;

		T & operator *()
		{
			return current->m_val;
		}

		bool operator !=(iterator & it2)
		{
			return this->current != it2.current;
		}

		bool operator ==(iterator & it2)
		{
			return this->current == it2.current;
		}

		iterator & operator ++ ()
		{
			current = current->m_next;
			return *this;
		}
		iterator & operator -- ()
		{
			current = current->m_prev;
			return *this;
		}
	};


public:
	explicit List()
		:
		m_size(0),
		m_tail(0),
		m_head(0),
		m_end(0)
	{
		m_it_begin = new iterator();
		m_it_end = new iterator();

		m_it_begin->current = m_head;
		m_it_end->current = m_tail;
	}

	~List()
	{
		Node * current = m_head;
		while (current != m_end) {
			Node * nd = current;
			current = current->m_next;
			delete nd;
		}

		delete m_it_end;
		delete m_it_begin;

	}

	iterator begin()
	{
		return *m_it_begin;
	}

	iterator end()
	{
		return *m_it_end;
	}

	unsigned int size()
	{
		return m_size;
	}

	void push_back(T val)
	{

		Node * node = new Node(val);
		node->m_prev = m_tail;
		node->m_next = m_end;

		if (m_tail)
			m_tail->m_next = node;

		if (m_size == 0) {
			m_head = node;
			m_it_begin->current = m_head;
		}

		m_tail = node;

		++m_size;
	}

private:
	// data
	unsigned int m_size;
	Node * m_tail;
	Node * m_head;
	Node * m_end; // it end // begin

	iterator * m_it_begin;
	iterator * m_it_end;
	_Alloc   allocator; // own allocator
};

}
/// ----------------------------------------

void test1(unsigned int nr)
{
	unsigned int i = 0;
	std::list<int> stl_list;

	struct timeval tv_begin;
	struct timeval tv_end;

	gettimeofday(&tv_begin, (struct timezone *)NULL);
	for (i < 0; i < nr; ++i) {
		stl_list.push_back(i);
	}
	gettimeofday(&tv_end, (struct timezone *)NULL);

	int nano_end   = tv_end.tv_sec * 1000000 + tv_end.tv_usec;
	int nano_begin = tv_begin.tv_sec * 1000000 + tv_begin.tv_usec;

	int seconds = nano_end - nano_begin;

	std::cerr << "1 : STD : time = " << seconds / 1000000 << ", " << seconds << std::endl;
	std::cerr << "1 : STD : size   = " << stl_list.size() << std::endl;
}

void test2(unsigned int nr)
{
	unsigned int i = 0;
	QLinkedList<int> qt_list;

	struct timeval tv_begin;
	struct timeval tv_end;


	gettimeofday(&tv_begin, (struct timezone *)NULL);
	for (i < 0; i < nr; ++i) {
		qt_list.push_back(i);
	}
	gettimeofday(&tv_end, (struct timezone *)NULL);

	int nano_end   = tv_end.tv_sec * 1000000 + tv_end.tv_usec;
	int nano_begin = tv_begin.tv_sec * 1000000 + tv_begin.tv_usec;

	int seconds = nano_end - nano_begin;

	std::cerr << "2 : Qt : time = " << seconds / 1000000 << ", " << seconds << std::endl;
	std::cerr << "2 : Qt : size   = " << qt_list.size() << std::endl;
}


void test3(unsigned int nr)
{
	unsigned int i = 0;
	playground::List<int> ew_list;

	struct timeval tv_begin;
	struct timeval tv_end;

	gettimeofday(&tv_begin, (struct timezone *)NULL);
	for (i < 0; i < nr; ++i) {
		ew_list.push_back(i);
	}
	gettimeofday(&tv_end, (struct timezone *)NULL);

	int nano_end   = tv_end.tv_sec * 1000000 + tv_end.tv_usec;
	int nano_begin = tv_begin.tv_sec * 1000000 + tv_begin.tv_usec;

	int seconds = nano_end - nano_begin;

	std::cerr << "3 : Ew : time = " << seconds / 1000000 << ", " << seconds << std::endl;
	std::cerr << "3 : Ew : size   = " << ew_list.size() << std::endl;
}



int main(int ac, char * av[])
{
	std::allocator<int> alloc_int;

	int NR_ELM = 1000;
	int  NR_LOOP = 1000;

	if (ac == 3) {
		NR_ELM = ::atoi(av[1]);
		NR_LOOP = ::atoi(av[2]);
	}


#ifdef TEST3
	for (int i = 0; i < NR_LOOP; ++i)
		test3(NR_ELM);
	std::cerr << "    --------------------------\n";
#endif

#ifdef TEST2
	for (int i = 0; i < NR_LOOP; ++i)
		test2(NR_ELM);
	std::cerr << "    --------------------------\n";
#endif

#ifdef TEST1
	for (int i = 0; i < NR_LOOP; ++i)
		test1(NR_ELM);
	std::cerr << "    --------------------------\n";
#endif

	std::cerr << "-----------------------------\n";

	return 0;
}
