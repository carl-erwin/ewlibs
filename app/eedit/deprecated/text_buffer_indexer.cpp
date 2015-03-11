#include <assert.h>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <list>
#include <iostream>

#include <ew/core/time/time.hpp>
#include <new>

#include <ew/core/threading/mutex_locker.hpp>

#include <ew/console/console.hpp>
using namespace ew::console;

#include <ew/codecs/text/unicode/utf8/utf8.hpp>
using namespace ew::codecs::text::unicode;

#include <ew/maths/functions.hpp>

#include "text_buffer_indexer.hpp"
#include "text_page_data.hpp"

#include "text_buffer.hpp"

namespace  eedit
{

/* FIXME: add a flag
   to know if the first byte is a split sequence
   ex  [.........\r|\n.........]
   is yes skip first byte
*/

/*
 TODO:

 class text_codec::context
 {
 public:
   virtual ~context(){};
   virtual reset();
 };

 class utf8_codec_context : text_codec::context
 {
 public:
   utf8_codec_context() { reset(); }
   virtual ~utf8_codec_context(){};

   virtual bool reset()
   {
    last_forward_byte = 0;
    last_backward_byte = 0;
    reset;
   }

 private:
   u8 last_forward_byte;
   u8 last_backward_byte;
 };

 remember last decoded cp ???

 add text_codec::context_t * ctx = codec()->get_context();
 add bool codec()->release_context(text_codec::context_t * ctx);
 add text_codec::count_cp(begin, end, ctx);

*/


/*
class text_page_data : public buffer::node_meta_data
{
public:
    text_page_data() {
        nr_cp = 0;
        nr_new_line = 0;
    }
    u32 nr_cp;
    u32 nr_new_line;
};
*/



struct index_context {
	eedit::text_buffer * text_buffer;
	/* codec */
	u8  prev_byte = 0;
	s32 state = 0;
	s32 prev_cp = 0;

	s32 cp = 0;
};



/* update all parents of n */
bool on_node_indexed(buffer::node * n, index_context * ctx)
{
	//  app_log << "------------------------------------------\n";
	//  app_log << __FUNCTION__ << "\n";

	text_page_data * n_pi = static_cast<text_page_data *>(n->get_meta_data());
	if (n_pi == nullptr) {
		n_pi = new text_page_data;
		n->set_meta_data((buffer::node_meta_data *)n_pi);
	}

	// app_log << " node : n(" << n << ") : m_nr_new_line = " << n_pi->m_nr_new_line << "\n";

	auto p = n->parent();
	while (p) {
		text_page_data * pi = static_cast<text_page_data *>(p->get_meta_data());
		if (pi == nullptr) {
			pi = new text_page_data;
			p->set_meta_data((buffer::node_meta_data *)pi);
		}

		*pi += *n_pi;

		//     app_log << " UPDATE PARENT p(" << p << ") : m_nr_new_line = " << pi->m_nr_new_line << "\n";
		p = p->parent();
	}
	// app_log << "\n------------------------------------------\n";
	return true;
}


bool on_node_unlink(buffer::node * n, index_context * ctx)
{
	// app_log << "------------------------------------------\n";
	//   app_log << __FUNCTION__ << "\n";


	text_page_data * n_pi = static_cast<text_page_data *>(n->get_meta_data());

	//   app_log << " unlinked node : n_pi->m_nr_new_line " << n_pi->m_nr_new_line << "\n";

	/* the node was not indexed yet */
	if (n_pi) {
		buffer::node * p = n->parent();
		while (p) {
			text_page_data * pi = static_cast<text_page_data *>(p->get_meta_data());

			*pi -= *n_pi;
			//  app_log << " unlinked node : UPDATE PARENT p(" << p << ") : m_nr_new_line = " << pi->m_nr_new_line << "\n";

			p = p->parent();
		}

		n->set_meta_data((buffer::node_meta_data *)nullptr);
	}

	//  app_log << "\n\n------------------------------------------\n";
	return true;

}


void index_node(buffer::node * n, u64 * nr_lines, index_context * ctx,
		bool generate_on_node_index_event = true)
{
	// app_log << "------------------------------------------\n";
	// app_log << __FUNCTION__ << "\n";

	// setup counters
	text_page_data * pi = static_cast<text_page_data *>(n->get_meta_data());
	if (pi == nullptr) {
		pi = new text_page_data;
		n->set_meta_data((buffer::node_meta_data *)pi);
	} else {

		if (generate_on_node_index_event) {
			on_node_unlink(n, ctx);
		}
	}


	buffer::page_type    *   p = n->page();
	bool ret = p->map();
	{
		assert(ret == true);
		if (ret == false) {

		}

		u8 * b = p->begin();
		u8 * e = p->end();
		pi->m_nr_new_line = 0;
		// FIXME: must allocate codec_context for indexer
		u64 nr_new_line = pi->m_nr_new_line;
		ctx->text_buffer->codec()->count_new_line(/* codec_ctx */ b, e, ctx->prev_byte, nr_new_line);
		pi->m_nr_new_line = nr_new_line;
	}

	ret = p->unmap();
	assert(ret == true);

	*nr_lines += pi->m_nr_new_line;

	if (generate_on_node_index_event) {
		on_node_indexed(n, ctx);
	}
}

bool on_node_insert(buffer::node * n, index_context * ctx)
{
	u64 nr_lines = 0;
	index_node(n, &nr_lines, ctx);
	return true;
}

bool on_node_modified(buffer::node * n, index_context * ctx)
{
	text_buffer * text_buffer = ctx->text_buffer;
	app_log << "------------------------------------------\n";
	app_log << __FUNCTION__ << "\n";

	/* for ll_buffer each node */
	buffer * buff = text_buffer->buffer();


	text_page_data * pi = static_cast<text_page_data *>(n->get_meta_data());

	text_page_data before_pi = *pi;

	//   app_log << __FUNCTION__ << " n(" << n << ") pi->m_nr_new_line = " << pi->m_nr_new_line << "\n";

	u64 nr_lines = 0;
	index_node(n, &nr_lines, ctx, false);

	pi->m_nr_new_line = nr_lines;

	u64 diff = 0;

	app_log << " before_pi.m_nr_new_line " << before_pi.m_nr_new_line << "\n";
	app_log << " nr lines " << nr_lines << "\n";

	if (before_pi.m_nr_new_line < nr_lines) {

		// nothing add diff
		diff = nr_lines - before_pi.m_nr_new_line;

		//    app_log << " propagate : TO ADD diff = " << diff << "\n";

		// linear walk
		struct increment_node_size_t {
			size_t m_add_value;
			increment_node_size_t(size_t add_value)
				: m_add_value(add_value)
			{
			}

			bool operator()(buffer::node * n)
			{
				text_page_data * pi = static_cast<text_page_data *>(n->get_meta_data());
				pi->m_nr_new_line += m_add_value;
				return true;
			}
		};

		buff->update_hierarchy(n->parent(), increment_node_size_t(diff));
	}

	if (before_pi.m_nr_new_line > nr_lines) {
		// nothing
		diff = before_pi.m_nr_new_line - nr_lines;
		//   app_log << " propagate : TO SUB diff = " << diff << "\n";

		// linear walk
		struct decrement_node_size_t {
			size_t m_sub_value;
			decrement_node_size_t(size_t sub_value)
				: m_sub_value(sub_value)
			{
			}

			bool operator()(buffer::node * n)
			{
				text_page_data * pi = static_cast<text_page_data *>(n->get_meta_data());
				pi->m_nr_new_line -= m_sub_value;
				return true;
			}
		};

		buff->update_hierarchy(n->parent(), decrement_node_size_t(diff));
	}


	// app_log << "\n------------------------------------------\n";

	return true;
}

bool text_buffer_indexer::on_node_event(buffer::node * n, buffer::node_event ev, index_context * ctx)
{
	text_buffer * text_buffer = ctx->text_buffer;

	//  app_log << "------------------------------------------\n";
	//  app_log << __FUNCTION__ << "\n";

	bool ret;

	switch (ev) {

	case buffer::node_unlink: {
		ret = on_node_unlink(n, ctx);
	}
	break;

	case buffer::node_insert: {
		ret = on_node_insert(n, ctx);
	}
	break;

	case buffer::node_modified: {
		ret = on_node_modified(n, ctx);
	}
	break;

	default: {
		ret = false;
	}
	}


	// update global line conuter
	buffer * flex = text_buffer->buffer();
	buffer::node * root = flex->root_node();
	text_page_data * pi = static_cast<text_page_data *>(root->get_meta_data());
	if (pi) {
		//    app_log << " text_buffer->numberOfLines = " << (pi->m_nr_new_line + 1) << "\n";
		text_buffer->numberOfLines.set(pi->m_nr_new_line + 1);
	}

	return ret;
}


bool text_buffer_indexer::build_index(text_buffer * text_buffer)
{
	//  app_log << "------------------------------------------\n";
	//   app_log << __FUNCTION__ << "\n";

	using ew::core::objects::buffer;



	auto ctx = new index_context;
	ctx->text_buffer = text_buffer;
	text_buffer->index_ctx = ctx;


	/* for ll_buffer each node */
	buffer * buff = text_buffer->buffer();

	/* install cb */
	// TODO: register_node_event_callback

	buff->set_node_event_callback((buffer::node_event_cb)on_node_event, ctx);

	buffer::iterator it =  buff->begin();
	buffer::iterator it_end =  buff->end();

	buffer::node * n = it.get_node();;
	buffer::node * n_end = it_end.get_node();
	buffer::node * n_last = n_end;

	if (n_last) {
		n_last = n_last->next();
	}

	u64 total_nr_lines = 0;

	bool abort_flag = false;

	while (n != n_last) {
		u64 nr_lines = 0;
		index_node(n, &nr_lines, text_buffer->index_ctx);
		total_nr_lines += nr_lines;

		n = n->next();
	}

	if (abort_flag == false) {
		text_buffer->d.is_indexed = true;

		text_buffer->numberOfLines.increment(total_nr_lines + 1);

		app_log << "\n\rtotal_nr_lines = " << total_nr_lines << "\n";

		// TODO: push editor_refresh_ui event { +buffer }
	}

	return true;
}

} // ! namespace eedit
