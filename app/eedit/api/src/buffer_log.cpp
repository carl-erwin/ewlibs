#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <cstring>

#include "core/log.hpp"


#include "../include/buffer_log.h"

struct buffer_commit {
	buffer_commit_rev_t rev  = 0;

	buffer_commit * parent = nullptr; //

	buffer_commit * prev  = nullptr; // time sorted
	buffer_commit * next  = nullptr; // time sorted

	buffer_log_operation_t op = buffer_log_nop;

	// payload
	uint64_t  offset = 0;
	size_t    size   = 0;
	uint8_t * data   = nullptr;
};

struct buffer_log {
	size_t   id;
	uint64_t next_rev    = 0;

	buffer_commit * first = nullptr;    // updated @ init
	buffer_commit * cur   = nullptr;    // updated on insert/remove/undo/redo
	buffer_commit * last  = nullptr;    // updated on insert/remove
	std::vector<buffer_commit *> table; // reset this carrefully


	~buffer_log()
	{
		auto p = first;
		while (p) {
			auto tmp = p;
			p = p->next;
			delete tmp;
		}
	}

};

// global table
#define MAX_BUFFER_LOGS (1000000)
static std::array<buffer_log *, MAX_BUFFER_LOGS> log_table;
static size_t last_id = 1;

buffer_log * get_log_pointer(buffer_log_id_t log_id)
{
	if (log_id >= MAX_BUFFER_LOGS)
		return nullptr;

	return log_table[log_id];
}



///////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {

	int buffer_log_dump(buffer_log_id_t id)
	{
		app_log << __PRETTY_FUNCTION__ << " : id " << id << "\n";
		auto log = get_log_pointer(id);
		if (!log)
			return -1;

		auto ci = log->first;
		if (ci) {
			ci = ci->next;
		}

		for (; ci != nullptr; ci = ci->next) {
			app_log << "rev[" << ci->rev << "] ,"
				<< "op(" << ci->op << "), "
				<< "offset(" << ci->offset << "), ";
			if (ci->data) {
				app_log << "data["<<ci->size <<"]('" << (char*)ci->data << "')";
			}
			app_log << "\n";
		}

		return 0;
	}

	int buffer_log_init(const byte_buffer_id_t, buffer_log_id_t * allocated_id)
	{
		size_t id = last_id;
		while (log_table[id] != nullptr || (id == 0)) {
			++id;
			id %= MAX_BUFFER_LOGS;
			if (id == last_id) {
				return -1;
			}
		}
		auto log = log_table[id] = new buffer_log;
		*allocated_id = last_id = id;

		// init first commit
		log->first = log->cur = log->last  = new buffer_commit;

		app_log << __PRETTY_FUNCTION__ << " : new buffer_log_id " << *allocated_id << "\n";

		return 0;
	}

	int buffer_log_reset(buffer_log_id_t log)
	{
		return 0;
	}

	int buffer_log_destroy(buffer_log_id_t log_id)
	{
		auto log = get_log_pointer(log_id);

		delete log;
		log_table[log_id] = nullptr;
		return 0;
	}

	static int buffer_log_ll_insert_commit(buffer_log * log, buffer_log_operation_t op,uint64_t offset, const uint8_t * data, size_t size, buffer_commit_rev_t * rev)
	{
		auto ci = new buffer_commit; // TODO: ctor
		ci->op = op;
		*rev = ci->rev = log->last->rev + 1;

		ci->offset = offset;

		// copy data
		ci->size = size;
		ci->data = new uint8_t [size+1];
		::memcpy(ci->data, data, size);
		ci->data[size] = 0;

		// insert commit
		ci->parent = log->cur;

		// set time
		log->last->next = ci;
		ci->prev= log->last;

		log->cur  = ci;
		log->last = ci;

		app_log << __FUNCTION__ << " op " << op << ", @" << offset << " '" << (char *)ci->data << "' size = " << size << ": rev = " << *rev << "\n";

		return 0;
	}

	static int buffer_log_insert_revert_commits(buffer_log * log)
	{

		// from last to cur
		if (log->cur == log->last) {
			// do nothing
			return 0;
		}

		app_log << __FUNCTION__ << "\n";

		// build list of commits from last to cur while inverting the op
		auto b = log->last;
		auto e = log->cur;

		std::list<buffer_commit *> ci_list;
		while (b != e) {
			buffer_commit * ci = new buffer_commit;
			ci->op = (b->op == buffer_log_insert_op ? buffer_log_remove_op : buffer_log_insert_op);
			ci->offset = b->offset;
			ci->size   = b->size;
			ci->data   = b->data;
			ci_list.push_back(ci);

			b = b->parent;
		}

		// insert new commits
		log->cur = log->last;
		for (auto & e : ci_list) {
			buffer_commit_rev_t rev;
			buffer_log_ll_insert_commit(log, e->op, e->offset, e->data, e->size, &rev);
		}

		// destroy tmp list
		for (auto & e : ci_list) {
			delete e;
		}

		return 0;
	}

	int buffer_log_insert_commit(buffer_log_id_t log_id, buffer_log_operation_t op,uint64_t offset, const uint8_t * data, size_t size, buffer_commit_rev_t * rev)
	{
		auto log = get_log_pointer(log_id);

		// update from undo list ...
		buffer_log_insert_revert_commits(log);
		return buffer_log_ll_insert_commit(log, op, offset, data, size, rev);
	}

	int buffer_log_insert(buffer_log_id_t log_id, uint64_t offset, const uint8_t * data, size_t size, buffer_commit_rev_t * rev)
	{
		return buffer_log_insert_commit(log_id, buffer_log_insert_op, offset, data, size, rev);
	}

	int buffer_log_remove(buffer_log_id_t log, uint64_t offset, const uint8_t * data, size_t size, buffer_commit_rev_t * rev)
	{
		return buffer_log_insert_commit(log, buffer_log_remove_op, offset, data, size, rev);
	}

// helper
	static inline int buffer_commit_attr(buffer_commit * ci, buffer_commit_rev_t * rev, buffer_log_operation_t * op, uint64_t * offset, const uint8_t ** const data, size_t * size)
	{
		*rev    = ci->rev;
		*op     = ci->op;
		*offset = ci->offset;
		*data   = ci->data;
		*size   = ci->size;
		return 0;
	}

	int buffer_log_get_last_commit(buffer_log_id_t log_id, buffer_commit_rev_t * rev, buffer_log_commit_data_t * commit_data)
	{
		auto log = get_log_pointer(log_id);
		return buffer_commit_attr(log->last, rev, &commit_data->op, &commit_data->offset, &commit_data->data, &commit_data->size);
	}

	int buffer_log_get_current_commit(buffer_log_id_t log_id, buffer_commit_rev_t * rev, buffer_log_commit_data_t * commit_data)
	{
		auto log = get_log_pointer(log_id);
		return buffer_commit_attr(log->cur, rev, &commit_data->op, &commit_data->offset, &commit_data->data, &commit_data->size);
	}

	// slow if list use array
	int buffer_log_get_commit_info(buffer_log_id_t log, buffer_commit_rev_t * rev, buffer_log_commit_data_t * commit_data)
	{
		return 0;
	}

	int buffer_log_undo(buffer_log_id_t log_id)
	{
		auto log = get_log_pointer(log_id);

		if (log->cur->parent) {
			log->cur = log->cur->parent;
		}

		return 0;
	}


	int buffer_log_redo(buffer_log_id_t log_id)
	{
		auto log = get_log_pointer(log_id);

		if (log->cur->next) {
			log->cur = log->cur->next;
		}

		return 0;
	}


	int buffer_log_undo_until(buffer_log_id_t log, buffer_commit_rev_t rev)
	{
		return 0;
	}

	int buffer_log_redo_until(buffer_log_id_t log, buffer_commit_rev_t rev)
	{
		return 0;
	}

}
