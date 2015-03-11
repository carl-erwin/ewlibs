#include "undo.h"
#include "../../core/event/event.hpp"

#include "../../core/core.hpp"

#include "../../api/include/buffer_log.h"

namespace eedit
{

namespace core
{

bool buffer_undo(struct event * msg)
{
	buffer_log_id_t        log;
	buffer_commit_rev_t    rev;
	buffer_log_commit_data_t ci_data{ buffer_log_nop, 0, nullptr, 0 };


	assert(0);
	// log = buff->txt_buffer()->log_id();

	buffer_log_get_current_commit(log, &rev, &ci_data);

	// app_log << "rev    = " << rev << "\n";
	// app_log << "op     = " << op << "\n";
	// app_log << "offset = " << offset << "\n";

	switch (ci_data.op) {
	case buffer_log_insert_op: {
		// app_log << "undo insert @ " << offset << "\n";

		// FIXME //auto b = buff->txt_buffer()->buffer()->begin() + offset;
		// FIXME //buff->txt_buffer()->buffer()->erase(b, b + size);

	}
	break;

	case buffer_log_remove_op: {
		// app_log << "undo remove @ " << offset << "\n";
		// FIXME: // buff->cursor_it() = offset;
		// FIXME: // u64 nrWritten = 0;
		// FIXME: // buff->txt_buffer()->buffer()->insert(offset, data, (size_t)size, &nrWritten);
	}
	break;

	default: {

	} break;
	}


	buffer_log_undo(log);


	assert(0);
	//FIXME:    set_buffer_changed_flag(process_ev_ctx); // send remove/insert event


	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool buffer_redo(event * msg)
{
	buffer_log_id_t log;
	buffer_commit_rev_t rev;
	buffer_commit_rev_t oldrev;

	buffer_log_commit_data_t ci_data{ buffer_log_nop, 0, nullptr, 0 };

	assert(0);
	// log = buff->txt_buffer()->log_id();

	buffer_log_get_current_commit(log, &oldrev, &ci_data);
	buffer_log_redo(log);
	buffer_log_get_current_commit(log, &rev, &ci_data);

	if (oldrev == rev) {
		app_log << "nothing to redo\n";
		return true;
	}

	// app_log << "rev    = " << rev << "\n";
	// app_log << "op     = " << op << "\n";
	// app_log << "offset = " << offset << "\n";

	switch (ci_data.op) {
	case buffer_log_remove_op: {
		// app_log << "do remove @ " << offset << "\n";
		assert(0);
		//FIXME// auto b = buff->txt_buffer()->buffer()->begin() + offset;
		//FIXME// buff->txt_buffer()->buffer()->erase(b, b + size);
		//FIXME// buff->cursor_it()->move_to_offset(offset);
		//FIXME// cursor_off = offset;
	}
	break;

	case buffer_log_insert_op: {
		// app_log << "do insert @ " << offset << "\n";

		assert(0);
		//FIXME// buff->cursor_it()->move_to_offset(offset);
		//FIXME// u64 nrWritten = 0;
		//FIXME// buff->txt_buffer()->buffer()->insert(offset, data, (size_t)size, &nrWritten);
		//FIXME// cursor_off = offset + size;
	}
	break;

	default: {

	} break;
	}

	assert(0);
	// set_buffer_changed_flag(process_ev_ctx);


	// TODO: insert/remove op

	return true;
}

} // namespace core

} // namespace eedit
