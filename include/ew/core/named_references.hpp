#pragma once

#include <iostream>
#include <map>
#include <string>

template <class T>
struct named_references {
	named_references()
	{
		m_nr_ref = 0;
	}

	T * ref(std::string & ref_name,  const char * file,  const int line,  const char * function)
	{
		__sync_fetch_and_add(&m_nr_ref, 1);

		owners_iterator it = m_owners.find(ref_name);
		if (it != m_owners.end()) {
			m_owners[ ref_name ]++;
		} else {

			m_owners[ ref_name ] = 1;
			it->second++;
		}

		dump_references();

		return static_cast<T *>(this);
	}

	void dump_references()
	{

		std::cerr <<  " dump ref\n";

		owners_iterator it = m_owners.begin();
		for (; it != m_owners.end(); ++it) {
			std::cerr <<  "m_owners[" << it->first << "] = " <<  it->second <<  "\n";
		}
	}

	void unref(const std::string & refName, const char * file,  const int line,  const char * function)
	{
		assert(m_nr_ref);
		__sync_fetch_and_sub(&m_nr_ref, 1);

		owners_iterator it = m_owners.find(refName);
		if (it == m_owners.end()) {
			std::cerr <<  " bad unref '" <<  refName << "'\n";
			assert(it != m_owners.end());
		}

		assert(m_owners[ refName ]);
		m_owners[ refName ]--;

		dump_references();
	}

	size_t nrRef()
	{
		dump_references();
		return m_nr_ref;
	}

private:
	size_t m_nr_ref;
	std::map<std::string, int> m_owners;
	typedef std::map<std::string, int>::iterator owners_iterator;
	typedef std::map<std::string, int>::const_iterator owners_const_iterator;
	typedef std::pair<owners_iterator, bool> insert_pair;
};
