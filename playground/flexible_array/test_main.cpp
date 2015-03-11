#include <iostream>
using std::cout;
using std::cerr;


#include <deque>

#include <assert.h>
#include <cstdlib>

#include <ew/maths/functions.hpp>
#include <ew/core/time/time.hpp>

#ifdef  TEST_STD_VECTOR
#include <vector>
#endif

#ifdef TEST_STD_DEQUE
#include <deque>
#endif

#ifdef TEST_QT_VECTOR
#include <QtCore/QVector>
#endif

#ifdef TEST_SIMPLE_ARRAY
#include "ew/core/container/simple_array.hpp"
#endif

#ifdef TEST_SIMPLE_BSP
#include "ew/core/container/flexible_array.hpp"
#endif

#ifdef TEST_MAPPED_FILE
#include "ew/core/container/flexible_array.hpp"
#include "ew/filesystem/mapped_file.hpp"
#endif


#ifdef TEST_BOOST_ARRAY
#include <boost/array.hpp>
#define BOOST_ARRAY_SIZE (50*1024*1024)
#endif


using namespace ew::core::time;

/*
 flexarray

 provide a calibrate loop for a max_insert_time

 must track insert time to decide to split or not

 add minItemsPerpage
 add maxItemsPerpage
 to allow resize

*/

/*
array interface

push_back(elm)
push_front(elm)
insert(pos, elm_vec, nr)
remove(start, end, action_on_elm)
elm_at
size
capacity

copy(array, start, end)
split()

*/


#define DUMP_SINGLE_ARRAY( A1 )                                     \
    for (u64 i = 0; i < A1.size(); i++) {                                 \
        std::cout << "array[" << (i) << "] =     " << A1[i] << " , " << " array.size() = " << A1.size() << "\n"; \
        std::cout << "---------------------------------------------\n";          \
    }                                                                     \


#define DUMP_ARRAY( A1, A2 )                                            \
    std::cout << "array 1 size = " << A1.size() << "\n";                     \
    std::cout << "array 2 size = " << A2.size() << "\n";                     \
    assert( A1.size() == A2.size());                                    \
    for (u64 i = 0; i < A1.size(); i++) {                               \
        std::cout << "array[" << (i) << "] =     " << A1[i] << " , " << " array.size() = " << A1.size() << "\n"; \
        std::cout << "ref_array[" << (i) << "] = " << A2[i] << " , " << " re_array.size() = " << A1.size() << "\n"; \
        std::cout << "---------------------------------------------\n";      \
    }                                                                   \


#define CMP_ARRAY( A1, A2 )                                             \
    for (u64 i = 0; i < array.size(); i++) {                              \
        if (!(A1[i] == A2[i]))                                                \
        {                                                                     \
            std::cout << "arrays differ @ index " << i << "\n";                 \
            assert( A1[i] == A2[i]);                                            \
            abort();                                                            \
        }                                                                     \
    }                                                                     \



#include "ew/core/container/simple_array.hpp"
#include <vector>

namespace test
{
namespace core
{
namespace container
{


typedef class ew::core::container::simple_array<unsigned char> byteArray;
typedef class ew::core::container::simple_array<unsigned int> intArray;

int test_integer(unsigned int pageSize, unsigned int nrInsert)
{
	std::cout << " --- RUNNING TEST : PAGE_SIZE = " << pageSize << " , NR_LOOP = " << nrInsert << "\n";

	// byteArray array(pageSize);
	intArray array(pageSize);

	std::vector<unsigned int> ref_array;

#if 0
	array.push_back(0);
	DUMP_SINGLE_ARRAY(array);

	array.push_back(1);
	DUMP_SINGLE_ARRAY(array);

	array.push_back(2);
	DUMP_SINGLE_ARRAY(array);

	array.insert(1, 8);
	DUMP_SINGLE_ARRAY(array);

	return 0;
#endif

	std::cout << " --- Test insert ---\n";
	for (unsigned int i = 0; i < nrInsert; i++) {
		unsigned int val;

		bool ret;
		int index;

		val = (i + 1) * 1000;
		index = i;

		val++;

		// std::cout << "\n\n\n\n -- PASS 1 -- \n";

		//    DUMP_ARRAY(array, ref_array);

		// std::cout << "array.insert( index = " << index << " , val = " << val << ")\n";

		index = array.size() / 2;
		array.insert(index, val);
		std::vector<unsigned int>::iterator it = ref_array.begin() + index;
		assert(index == it - ref_array.begin());
		ref_array.insert(it, val);

		assert(array.size());
		// std::cout << "CHECK index/val :   array[" << index << " ] == " <<  array[index] << "\n\n";

		// DUMP_ARRAY(array, ref_array);

		assert(array[index] == val);
		assert(ref_array[index] == val);

		CMP_ARRAY(array, ref_array);

		// std::cout << "\n\n\n\n -- PASS 2 -- \n";

		val++;

		// std::cout << "array.push_back( val = " << (int)val << ")\n";

		array.push_back(val);
		ref_array.push_back(val);
		assert(array[array.size() - 1] == val);
		assert(ref_array[ref_array.size() - 1] == val);

		CMP_ARRAY(array, ref_array);


		// std::cout << "\n\n\n\n -- PASS 3 -- \n";

		val++;

		// std::cout << "array.push_front( val = " << val << ")\n";

		ret = array.push_front(val);
		if (ret == false) {
			// TODO
		}

		ref_array.insert(ref_array.begin(), val);
		assert(array[0] == val);
		assert(ref_array[0] == val);

		CMP_ARRAY(array, ref_array);
	}

	return 0;

	std::cout << "------- final dump --------\n";
	for (u64 i = 0; i < array.size(); i++) {
		std::cout << "array[" << (i) << "] =     " << (int)array[i] << " , " << " array.size() = " << array.size() << "\n";
		std::cout << "ref_array[" << (i) << "] = " << (int)ref_array[i] << " , " << " re_array.size() = " << array.size() << "\n";
		assert(array[i] == ref_array[i]);
		std::cout << "---------------------------------------------\n";
	}

	return 0;
}



int nr_alloc = 0;
int nr_free = 0;

//#define DEBUG_TEST
//#define USE_TEST_PTR

#define DO_RESERVE

class Test
{
public:
	Test(unsigned int i)
	{
#ifdef DEBUG_TEST
		std::cout << " --- Test (unsigned  int ) ---\n";
#endif
		val = i;
#ifdef USE_TEST_PTR
		p = new int[1];
		*p = val;
#endif
		nr_alloc++;
		init = 0xcecece;
	}

	Test()
	{
#ifdef DEBUG_TEST
		std::cout << " --- Test() ---\n";
#endif
		val = 0;
#ifdef USE_TEST_PTR
		p = new int[1];
		*p = val;
#endif
		nr_alloc++;
		init = 0xcecece;
	}

	~Test()
	{
#ifdef DEBUG_TEST
		std::cout << " --- ~Test() ---\n";
#endif
		assert(init == 0xcecece);

		val = 0;
#ifdef USE_TEST_PTR
		delete [] p;
		p = 0;
#endif

		nr_free++;
	}

	Test & operator = (const Test & orig)
	{
#ifdef DEBUG_TEST
		std::cout << " --- Test & operator = (const Test & orig = " << orig.val << ") ---\n";
#endif
		assert(orig.init == 0xcecece);

		if (this != &orig) {
			// assert(val != 0);
			val = orig.val;
#ifdef USE_TEST_PTR
			// assert(orig.p);
			// *p = *orig.p;
#endif
		}
		return *this;
	}



	Test & operator = (int i)
	{
#ifdef DEBUG_TEST
		std::cout << " --- Test & operator = (int i = " << i << ") ---\n";
#endif
		assert(init == 0xcecece);

		init = 0xcecece;
		val = i;
#ifdef USE_TEST_PTR
		*p = i;
#endif
		return *this;
	}

	Test(const Test & orig)
	{
#ifdef DEBUG_TEST
		std::cout << " --- Test( const Test & ) ---\n";
#endif
		assert(orig.init == 0xcecece);
		init = 0xcecece;
		val = orig.val;
#ifdef USE_TEST_PTR
		p = new int [1];
		*p = val;
#endif
		nr_alloc++;
	}

public:
	int init;
	int val;
#ifdef USE_TEST_PTR
	int * p;
	char test[24];
#endif
};

inline std::ostream & operator<<(std::ostream & stream, const Test & t)
{
	stream << "( t.val = ";
	stream << t.val;
	stream << ")";
	return (stream);
}

template <class T>
bool operator != (const Test & t, const T i)
{
#ifdef DEBUG_TEST
	std::cout << " --- " << __FUNCTION__ << "\n";
#endif
	Test t2(i);
	return t.val != t2.val;
}

/////

//#define test_t Test
//#define test_t unsigned char
//#define test_t u64
#define test_t unsigned int


#ifdef TEST_SIMPLE_ARRAY
typedef class ew::core::container::simple_array< test_t > simpleArray;
#endif

unsigned int * indexes = 0;
test_t * values  = 0;

void  init_tables(unsigned int nrInsert)
{
	indexes = new unsigned int [nrInsert];
	values     = new test_t [nrInsert];

	cout << "generate indexes/values...\n";

	u32 sz = nrInsert * sizeof(test_t);
	cout << " for nrInsert(" << nrInsert << ") * " << sizeof(test_t) << " =  \n";
	cout << sz << " bytes\n";
	cout << sz / 1024 << " kibytes\n";
	cout << sz / (1024 * 1024) << " mibytes\n";

	for (unsigned int i = 0; i < nrInsert; i++) {

		// values[i] = ( (int) ( double( rand() ) / ( double(RAND_MAX) + 1 ) * 0xffffffff ) );
		// indexes[i] = (unsigned int)( double( rand() ) / ( double(RAND_MAX) + 1 ) * nrInsert );

		// if (i % 2) {
		//   indexes[i] = i - 1;
		// } else {
		//   indexes[i] = 0;
		// }

		// if (i == nrInsert - 1)
		//   indexes[i] = 0;
		// else
		//   indexes[i] = i;

		// indexes[i] = i;
		// if (i < nrInsert / 2)
		//   indexes[i] = 0;

		indexes[i] = i;
		values[i] = i;

		//    indexes[i] = i / 2;

		//     if (indexes[i] > i)
		//   indexes[i] = i;
	}
}

void  release_tables()
{
	delete [] indexes;
	delete [] values;
}

u32 myGetTicks()
{
	//return  get_milliseconds_since_startup();
	// return  get_microseconds_since_startup();
	return  get_nanoseconds_since_startup();
}

template <typename Container>
void    Try_Reserve(Container & array,  unsigned int size)
{
	array.reserve(size);
}

#ifdef TEST_STD_DEQUE
template <>
void    Try_Reserve(std::deque<test_t> & array,  unsigned int size)
{
}
#endif


template <class T>
T * getContainer(unsigned int pageSize)
{
	return new T;
}


#ifdef TEST_SIMPLE_BSP
template <>
void    Try_Reserve(ew::core::container::flexible_array<test_t> & array,  unsigned int size)
{
}

template < >
ew::core::container::flexible_array<test_t> * getContainer< ew::core::container::flexible_array<test_t> >(unsigned int pageSize)
{
	return new ew::core::container::flexible_array<test_t>(pageSize, pageSize);
}


#endif


template <typename Container>
u64 Get_Capacity(Container & array, bool do_print = false)
{
	if (do_print) {
		cout << "size     = " << array.size() << ", ";
		cout << "capacity = " << array.capacity() << "\n";
	}
	return array.capacity();
}


#ifdef TEST_STD_DEQUE
template <>
u64    Get_Capacity(std::deque<test_t> & array, bool do_print)
{
	return 0;
}
#endif


#ifdef TEST_SIMPLE_BSP
template <>
u64    Get_Capacity(ew::core::container::flexible_array<test_t> & array, bool do_print)
{
	return 0;
}

#endif


void print_time(const char * time_name, float val)
{
	cout << time_name << " = " << val << " ns, " << (float)val / 1000.0f << " us, " << (float)val / 1000000.0f << " ms\n";
}


void print_time(const char * Cname, u32 nrTest,
		const unsigned int nrInsert,
		u32 insert_min, u32 insert_max,
		u32 test_min, u32 test_max, u32 test_sum)
{
	cout << Cname;
	print_time("insert_min", (float)insert_min);
	print_time("insert_max", (float)insert_max);
	print_time("test_min", (float)test_min);
	print_time("test_max", (float)test_max);

	float test_avg = (float)test_sum / (float)nrTest;
	print_time("test_avg", (float)test_avg);

}


#define BENCH_MULTIPLE_INSERT
// MULTIPLE INSERT

template <typename Container>
void test_multiple_insert(const char * Cname, const unsigned int pageSize, const unsigned int nrInsert, const unsigned int nrTestLoop)
{
	cout << "\n\n";
	cout << "BEGIN test (" << Cname << "), test_multiple_inserts\n";

	u32 test_min = 0xffffffff;
	u32 test_max = 0;
	u32 test_sum = 0;

#ifdef BENCH_MULTIPLE_INSERT
	u32 insert_min = 0xffffffff;
	u32 insert_max = 0;
	u32 insert_sum = 0;
#endif

	u32 max_size = 0;
	u32 max_capacity = 0;

	for (unsigned int i = 0; i < nrTestLoop; i++) {
		Container * parray = getContainer<Container>(pageSize);
		Container & array = *parray;

		// cout << "before test : array.size() = " << array.size() << "\n";

#ifdef DO_RESERVE
# ifdef DEBUG_TEST
		cout << Cname << " reserve\n";
# endif
		Try_Reserve(array, nrInsert);
		//Get_Capacity(array);

#endif

#ifdef DEBUG_TEST
		cout << "begin pass\n";
#endif
		u32 test_t0 = myGetTicks();
		{
			for (unsigned int i = 0; i < nrInsert; i++) {
#ifdef DEBUG_TEST
				cout << "insert................................\n";
#endif

#ifdef BENCH_MULTIPLE_INSERT
				u32 insert_t0 = myGetTicks();
#endif
				{
					// array.push_back(values[i]);
					// array.insert(array.end(), values[i]);
					// array.insert(array.end(), values[i]);

					auto itb = array.begin();
					array.insert(itb + indexes[i], values[i]);

					// array.insert(indexes[i], values[i]);
					// array.insert(array.begin(), values[i]);
					// array.insert(array.begin() + i ,  i);
					// array.insert(array.begin() + i ,  i);
					// array.insert(array.begin(),  i);
				}
#ifdef BENCH_MULTIPLE_INSERT
				u32 insert_t1 = myGetTicks();

				u32 insert_elapse = insert_t1 - insert_t0;
				insert_min = (insert_elapse < insert_min) ? insert_elapse : insert_min;
				insert_max = (insert_elapse > insert_max) ? insert_elapse : insert_max;
				insert_sum += insert_elapse;
#endif
			}
		}
		u32 test_t1 = myGetTicks();
		u32 elapse2 = test_t1 - test_t0;
		test_min = (elapse2 < test_min) ? elapse2 : test_min;
		test_max = (elapse2 > test_max) ? elapse2 : test_max;
		test_sum += elapse2;


		// cout << "after test \n";
		// Get_Capacity(array);

		max_size = ew::maths::max(max_size, (u32)array.size());

		max_capacity = ew::maths::max(max_size, (u32)Get_Capacity(array));

#ifdef DEBUG_TEST
		cout << "end of test\n";
		DUMP_SINGLE_ARRAY(array);
#endif

#if 1
		for (u64 i = 0; i < (u64)array.size() ; i++) {
			//cout << "(" << Cname << ")[" << i << "] = " << array[i] << "\n";
			//      if ( array[i] != values[nrInsert - i - 1])  { // reverse
			cout << " values[" << i << "] =  " << (int)values[i] << "\n";
			if (array[i] != values[ indexes[i] ]) {
				cout << "ERROR @ (" << Cname << ")[" << i << "] = " << array[i] << ", ";
				cout << " instead of " << values[i] << "\n";
			}
		}
#endif

		delete parray;
	}

	print_time(Cname, nrTestLoop, nrInsert, insert_min, insert_max, test_min, test_max, test_sum);

	cout << " max_size = " << max_size << "\n";
	cout << " max_capacity = " << max_capacity << "\n";

#ifdef BENCH_MULTIPLE_INSERT
	cout << " ...\n";
	cout << Cname << " insert_min = " << insert_min << "\n";
	cout << Cname << " insert_max = " << insert_max << "\n";
	cout << Cname << " insert_sum / " << nrInsert << " = " << insert_sum / nrInsert << "\n";
#endif
}


#ifdef TEST_BOOST_ARRAY

void test_boost_array_multiple_insert(const char * Cname, const unsigned int pageSize, const unsigned int nrInsert, const unsigned int nrTestLoop)
{
	cout << "\n\n";
	cout << "BEGIN test (" << Cname << "), test_multiple_inserts\n";

	u32 test_min = 0xffffffff;
	u32 test_max = 0;
	u32 test_sum = 0;

#ifdef BENCH_MULTIPLE_INSERT
	u32 insert_min = 0xffffffff;
	u32 insert_max = 0;
	u32 insert_sum = 0;
#endif

	for (unsigned int i = 0; i < nrTestLoop; i++) {
		boost::array<test_t, BOOST_ARRAY_SIZE> * parray = new boost::array<test_t, BOOST_ARRAY_SIZE>;
		boost::array<test_t, BOOST_ARRAY_SIZE> & array = *parray;

		// cout << "before test : array.size() = " << array.size() << "\n";

#ifdef DO_RESERVE
# ifdef DEBUG_TEST
		cout << Cname << " reserve\n";
# endif
		// Get_Capacity(array);

#endif

#ifdef DEBUG_TEST
		cout << "begin pass\n";
#endif
		u32 test_t0 = myGetTicks();
		{
			for (unsigned int i = 0; i < nrInsert; i++) {
#ifdef DEBUG_TEST
				cout << "insert................................\n";
#endif

#ifdef BENCH_MULTIPLE_INSERT
				u32 insert_t0 = myGetTicks();
#endif
				{
					if (i != nrInsert) {
						array[i] = values[i];
					}
				}
#ifdef BENCH_MULTIPLE_INSERT
				u32 insert_t1 = myGetTicks();

				u32 insert_elapse = insert_t1 - insert_t0;
				insert_min = (insert_elapse < insert_min) ? insert_elapse : insert_min;
				insert_max = (insert_elapse > insert_max) ? insert_elapse : insert_max;
				insert_sum += insert_elapse;
#endif
			}
		}
		u32 test_t1 = myGetTicks();
		u32 elapse2 = test_t1 - test_t0;
		test_min = (elapse2 < test_min) ? elapse2 : test_min;
		test_max = (elapse2 > test_max) ? elapse2 : test_max;
		test_sum += elapse2;

		//    cout << "after test : " << Cname << ".size() = " << array.size() << "\n";
		// Get_Capacity(array);


#ifdef DEBUG_TEST
		cout << "end of test\n";
		DUMP_SINGLE_ARRAY(array);
#endif

#if 1
		for (u64 i = 0; i < (u64)array.size() ; i++) {
			//cout << "(" << Cname << ")[" << i << "] = " << array[i] << "\n";
			if (array[i] != i) {
				cout << "ERROR @ (" << Cname << ")[" << i << "] = " << array[i] << "\n";
				cout << " instead of " << values[i] << "\n";
			}
		}
#endif

#if 0
		for (testArray::iterator it = array.begin() ; it != array.end(); it++) {
			cout << *it << " ";
		}
		cout << "\n";
#endif

#if defined(TEST_ERASE)
		array.erase(array.begin() + (array.size() / 2));
#endif

		delete parray;
	}

	print_time(Cname, nrTestLoop, nrInsert, insert_min, insert_max, test_min, test_max, test_sum);

#ifdef BENCH_MULTIPLE_INSERT
	cout << " ...\n";
	cout << Cname << " insert_min = " << insert_min << "\n";
	cout << Cname << " insert_max = " << insert_max << "\n";
	cout << Cname << " insert_sum / " << nrInsert << " = " << insert_sum / nrInsert << "\n";
#endif
}

#endif


int test_multiple_inserts(unsigned int pageSize, unsigned int nrInsert, unsigned int nrTestLoop, unsigned int nrRun)
{
	int ret = 0;

#ifdef TEST_STD_VECTOR
	for (unsigned int i = 0; i < nrRun; i++) {
		test_multiple_insert< std::vector<test_t> > ("std::vector", 0, nrInsert, nrTestLoop);
	}
#endif

#ifdef TEST_STD_DEQUE
	for (unsigned int i = 0; i < nrRun; i++) {
		test_multiple_insert< std::deque<test_t> > ("std::deque", 0, nrInsert, nrTestLoop);
	}
#endif

// #ifdef TEST_QT_VECTOR
//   for (unsigned int i = 0; i < nrRun; i++){
//     test_multiple_insert< QVector<test_t> > ("QVector", 0, nrInsert, nrTestLoop);
//   }
// #endif

#ifdef TEST_SIMPLE_ARRAY
	for (unsigned int i = 0; i < nrRun; i++) {
		test_multiple_insert< ew::core::container::simple_array<test_t> > ("simple_array", nrInsert, nrInsert, nrTestLoop);
	}
#endif


#ifdef TEST_SIMPLE_BSP
	for (unsigned int i = 0; i < nrRun; i++) {
		test_multiple_insert< ew::core::container::flexible_array<test_t> > ("flexarray", pageSize, nrInsert, nrTestLoop);
	}
#endif


#ifdef TEST_BOOST_ARRAY
	for (unsigned int i = 0; i < nrRun; i++) {
		test_boost_array_multiple_insert("boost", pageSize, nrInsert, nrTestLoop);
	}
#endif

	return ret;
}


// RANGE INSERT

template <typename Container>
void test_range_insert(const char * Cname, const unsigned int pageSize, const unsigned int nrInsert, const unsigned int nrTestLoop)
{
	cout << "\n\n";
	cout << "BEGIN test (" << Cname << ") , range insert\n";

	u32 test_min = 0xffffffff;
	u32 test_max = 0;
	u32 test_sum = 0;

	u32 insert_min = 0xffffffff;
	u32 insert_max = 0;


	u32 max_size = 0;
	u32 max_capacity = 0;

	for (unsigned int i = 0; i < nrTestLoop; i++) {
		Container * parray = getContainer<Container>(pageSize);
		Container & array = *parray;

		u32 test_t0 = myGetTicks();
		u32 insert_t0 = get_nanoseconds_since_startup();

		// int nr_block = 1;
		// for (int i = 0; i < nr_block; ++i)
		// array.insert(array.begin(), &values[0], &values[1]);
		array.insert(array.begin(), &values[0], &values[nrInsert]);
		//      Get_Capacity(array);
		//
		u32 insert_t1 = get_nanoseconds_since_startup();

		u32 test_t1 = myGetTicks();
		u32 elapse = test_t1 - test_t0;
		u32 insert_elapse = insert_t1 - insert_t0;

		test_min = ew::maths::min(elapse , test_min);
		test_max = ew::maths::max(elapse, test_max);
		test_sum += elapse;

		insert_min = ew::maths::min(insert_elapse , insert_min);
		insert_max = ew::maths::max(insert_elapse, insert_max);

		max_size = ew::maths::max(max_size, (u32)array.size());
		max_capacity = ew::maths::max(max_size, (u32)Get_Capacity(array));

		// TODO pass opt for check
#if 0
		for (unsigned int i = 0; i < nrInsert; i++) {
			//cout << "(" <<  Cname << ")[" << i << "] = " << array[i] << "\n";
			if (array[i] != values[i]) {
				cout << "ERROR @ (" << Cname << ")[" << i << "] = " << array[i] << "\n";
			}
		}
#endif

#if 0
#if !defined(TEST_QT_VECTOR) && defined(TEST_STD_VECTOR) || defined(TEST_FLEX_ARRAY) || defined(TEST_SIMPLE_ARRAY)
		{
			int count = 0;
			// wall through
			typename Container::iterator it = array.end();
			typename Container::iterator it_end = array.begin();
			u32 t0 = myGetTicks();
			while (it != it_end) {
				--it;
				cout << "*it [" << count << "] = " << (int)*it << "\n";
				++count;
			}
			u32 t1 = myGetTicks();
			cout << Cname << " wallk through " << count << " elements = " << t1 - t0 << "\n";
		}
		cout << "\n";
#endif
#endif

		// cout << "after test : " << Cname << ".size() = " << array.size() << "\n";
		// Get_Capacity(array);

#if defined(TEST_ERASE)
		array.erase(array.begin() + (array.size() / 2), array.end());
#endif

		delete parray;
	}

	print_time(Cname, nrTestLoop, nrInsert, insert_min, insert_max, test_min, test_max, test_sum);
	cout << Cname << " max_size = " << max_size << "\n";
	cout << Cname << " max_capacity = " << max_capacity << "\n";

#if !defined(TEST_QT_VECTOR) && defined(TEST_STD_VECTOR) || defined(TEST_FLEX_ARRAY) || defined(TEST_SIMPLE_ARRAY)
	// recursive insert
	if (0) {
		cout << "\n\n\n" << Cname << " sub insert test\n";

		int table[] = { 1, 2, 3, 4};

		Container array;

		cout << "before push_back , array.size = " << array.size() << "\n";

		for (unsigned int i = 1; i <= 10; i++) {
			cout << "array.push_back(" << i << ")\n";
			array.push_back(i);
		}

		cout << "after push_back , array.size = " << array.size() << "\n";

		cout << "before insert range\n";
		for (unsigned int i = 0; i < array.size(); i++) {
			cout << "loop i = " << i << "\n";
			cout << "array[" << i << "] = " << array[i] << "\n";
		}

		cout << "array.capacity() = " << Get_Capacity(array) << "\n";

		array.insert(array.begin() + 2, &table[0], &table[4]);

		cout << "after insert range\n";
		for (unsigned int i = 0; i < array.size(); i++)
			cout << "array[" << i << "] = " << array[i] << "\n";
	}
#endif

}

void test_range_inserts(unsigned int pageSize, unsigned int nrInsert, unsigned int nrTestLoop, unsigned int nrRun)
{

#ifdef TEST_STD_VECTOR
	for (unsigned int i = 0; i < nrRun; i++) {
		test_range_insert< std::vector<test_t> > ("std::vector", 0, nrInsert, nrTestLoop);
	}
#endif

#ifdef TEST_STD_DEQUE
	for (unsigned int i = 0; i < nrRun; i++) {
		test_range_insert< std::deque<test_t> > ("std::deque", 0, nrInsert, nrTestLoop);
	}
#endif

#ifdef TEST_SIMPLE_ARRAY
	for (unsigned int i = 0; i < nrRun; i++) {
		test_range_insert< ew::core::container::simple_array<test_t> > ("simple_array", nrInsert, nrInsert, nrTestLoop);
	}
#endif


#ifdef TEST_SIMPLE_BSP
	for (unsigned int i = 0; i < nrRun; i++) {
		test_range_insert< ew::core::container::flexible_array<test_t> > ("flexarray", pageSize, nrInsert, nrTestLoop);
	}
#endif


}

#ifdef TEST_MAPPED_FILE
typedef ew::filesystem::mapped_file FileBuffer;

int mapped_file_test_1(int ac, char ** av)
{
	if (ac != 3) {
		std::cout << "usage " << av[0] << " filename nrPagesPerBlock\n";
		return 1;
	}

	u32 nr_pages = ::atoi(av[2]);
	cerr << "nr_pages = " << nr_pages << "\n";
	FileBuffer test(av[1], nr_pages);

	FileBuffer::iterator it = test.begin();
	FileBuffer::iterator it_end = test.end();

	cerr << "test->begin().offset() " << it.offset() << "\n\n";
	cerr << "test->end().offset() " << it_end.offset() << "\n\n";
	cerr << "--- begin loop ---\n";

#if 1
	u64 nrRead = 0;
	u64 size = test.size();
	u64 remain = size;
	u32 cache_size = 16 * 1024;
	u8 * cache = new u8 [cache_size + 1];

	u64 total_read = 0;
	while (remain) {
		//it.dump();
		test.read(it, cache, &cache[cache_size], &nrRead);
		total_read += nrRead;
		cache[nrRead] = 0;
		cout << cache;
		remain -= nrRead;
		u64 new_off = it.offset() + nrRead;
		test.get_iterator_by_offset(new_off, &it);
		assert(it.offset() == new_off);
	}
	delete [] cache;
	cerr << "\n--- total_read " << total_read << " ---\n";

#else
	u8 b;
	while (it != it_end) {
		b = *it;
		cout << b;
		++it;
	}
#endif
	cerr << "\n--- end loop ---\n";
	return 0;
}

int mapped_file_test_2(int ac, char ** av)
{
	cerr << "ac=" << ac << "\n";
	if (ac != 4) {
		cerr << "usage " << av[0] << " nr_pages file1 file2\n";
		return 1;
	}

	u32 nr_pages = ::atoi(av[1]);

	nr_pages = ew::maths::min<u32>(nr_pages, 256);
	cerr << "nr_pages = " << nr_pages << "\n";


	FileBuffer a(av[2], nr_pages);
	FileBuffer b(av[3], nr_pages);

	FileBuffer::iterator ita = a.begin();
	FileBuffer::iterator ita_end = a.end();

	FileBuffer::iterator itb = b.begin();
	FileBuffer::iterator itb_end = b.end();

	u64 max = ew::maths::max(ita_end.offset(), itb_end.offset());


	cerr << "max = " << max << "\n";

	while (max) {
		u8 bytea = 0;
		u8 byteb = 0;

		if (ita != ita_end)
			bytea = *ita;

		if (itb != itb_end)
			byteb = *itb;

		if (bytea != byteb) {
			cout << "diff @ offset a " << ita.offset() << " | " << " offset b " << itb.offset() << " : ";
			cout << "'" << bytea << "' != '" << byteb << "'\n";
		}

		++ita;
		++itb;
		--max;
	}

	return 0;
}

int main(int ac, char ** av)
{
	if (ac < 2) {
		cerr << "usage : " << av[0] << " test_number\n";
		return 1;
	}

	u32 test_nr = ::atoi(av[1]);

	ac--;
	av++;
	switch (test_nr) {
	case 1:
		av[0] = (char *)"test1";
		return mapped_file_test_1(ac, av);
		break;

	case 2:
		av[0] = (char *)"test2";
		return mapped_file_test_2(ac, av);
		break;
	}
	return 1;
}

#else

int main(int ac, char ** av)
{
	if ((ac < 5) || (ac > 6)) {
		std::cout << "usage " << av[0] << " max_items_per_pages nr_insert nr_test_pass global_run test_mask\n";
		return 1;
	}

	char test_mask = 0xff;
	if (ac == 6) {
		test_mask = atoi(av[5]);
	}

	ew::core::time::init();

	srand(time(NULL));

	u32 max_items_per_page = ::atoi(av[1]);
	u32 nr_insert = ::atoi(av[2]);
	u32 nr_pass = ::atoi(av[3]);
	u32 nr_glb_run = ::atoi(av[4]);

	std::cout << "max_items_per_page = " << max_items_per_page << "\n";
	std::cout << "nr_insert          = " << nr_insert << "\n";
	std::cout << "nr_pass            = " << nr_pass << "\n";
	std::cout << "nr_glb_run         = " << nr_glb_run << "\n";

	init_tables(nr_insert);

	//  test_integer(max_items_per_page, nr_insert);
	if (test_mask & 0x1)
		test_multiple_inserts(max_items_per_page, nr_insert, nr_pass, nr_glb_run);

	if (test_mask & 0x2)
		test_range_inserts(max_items_per_page, nr_insert, nr_pass, nr_glb_run);

	release_tables();

	return 0;
}
#endif


} // ! namespace test
} // ! namespace core
} // ! namespace container


int main(int ac, char ** av)
{
	return test::core::container::main(ac, av);
}
