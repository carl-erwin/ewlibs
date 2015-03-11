#pragma once

#include <ew/core/object/buffer.hpp>


namespace  eedit
{

class text_page_data : public buffer::node_meta_data
{
public:
	text_page_data()
	{
		reset();
	}

	void reset()
	{
		m_nr_cp = 0;
		m_nr_new_line = 0;
	}

	text_page_data & operator -= (const text_page_data & to_sub)
	{
		assert(this->m_nr_new_line >= to_sub.m_nr_new_line);
#if 0
		app_log << " this->m_nr_new_line("
			<< this->m_nr_new_line
			<< ") -= to_sub.m_nr_new_line("
			<< to_sub.m_nr_new_line
			<< ");\n";
#endif
		this->m_nr_new_line -= to_sub.m_nr_new_line;
		return *this;
	}

	text_page_data & operator += (const text_page_data & to_add)
	{
#if 0
		app_log << " this->m_nr_new_line("
			<< this->m_nr_new_line
			<< ") += to_add.m_nr_new_line("
			<< to_add.m_nr_new_line
			<< ");\n";
#endif

		this->m_nr_new_line += to_add.m_nr_new_line;
		return *this;
	}

	u32 m_nr_cp;
	u32 m_nr_new_line;
};



} // ! namespace  eedit
