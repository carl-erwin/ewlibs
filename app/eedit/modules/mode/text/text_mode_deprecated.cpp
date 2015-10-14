////////////////////////////////////////////////////////////////////////////////

// old implementation

#if 0


////////////////////////////////////////////////////////////////////////////////
/*
 * TODO: use the user screen
 */
int it_to_previous_line(text_buffer::iterator & ref)
{
    text_buffer::iterator old_point = ref;
    text_buffer::iterator & it = ref;

    uint64_t it_off    = it.offset();
    if (it_off == 0)
        return EDITOR_STATUS_OK;

    int column = 1;
    it.toBeginningOfLine();
    while (it != old_point) {
        ++column;
        ++it;
    }


    // goto previous line
    it.toBeginningOfLine();
    --it;
    it.toBeginningOfLine();

    for (auto new_column = 1; new_column < column; ++new_column) {
        if ((*it == '\r') || (*it == '\n'))
            break;
        ++it;
    }

    it_off    = it.offset();

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


/*
  TODO:

  we need a graphic context / view screen(L, C)
  to :
  - render font glyph
  - scroll etc ...
  - the event must contain layout helpers that mask this ????
  - msg->view->screen->lock() // get ctx for font ... ?
  - or else must be mono font based ... :-(

  - the screen can provide a "hidden" gfx/ctx to the the core
  - acquired/locked with  screen->lock()/ screen->unlock()
  - auto ftmgr = screen->get_font_manager();
  - ftmgt->get_font(bold|italic)->glyp_info()->xadvance() ... the layout code is there ?
  -

  //
  uint32_t nr is the number of repetition ... (to-previous-line N &args) a la emacs --> lisp ?

*/


int to_previous_line(struct editor_message_s * _msg)
{
    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";

    input_event * msg = (input_event *)_msg;
    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    auto view = buffer->get_buffer_view_by_screen_id(msg->view_id);


    text_buffer::iterator old_point = view->cursor_it();
    it_to_previous_line(old_point);

    uint64_t rdr_begin_off  = buffer->rdr_begin()->offset();
    // uint64_t rdr_end_off   = d->rdr_end()->offset();
    uint64_t cursor_off    = buffer->cursor_it()->offset();

    if (cursor_off < rdr_begin_off) {
        it_to_previous_line(*buffer->rdr_begin());
        it_to_previous_line(*buffer->rdr_end());
    }

    set_ui_change_flag(msg->byte_buffer_id, msg->view_id);

    return EDITOR_STATUS_OK;
}


///////////////////////////////////////////////////////////////////////////////

int it_to_next_physical_line(text_buffer::iterator & ref)
{
    text_buffer::iterator old_point = ref;
    text_buffer::iterator & it = ref;

    // FIXME: always precompute start of line ...
    // compute current column : use indexer etc ...
    int column = 1;
    it.toBeginningOfLine();
    while (it != old_point) {
        ++column;
        ++it;
    }

    // goto next line
    it.toEndOfLine();
    ++it;

    // FIXME: save mark_motion_context.target_column = column

    for (auto new_column = 1; new_column < column; ++new_column) {
        if ((*it == '\r') || (*it == '\n'))
            break;
        ++it;
    }

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


/* physical line */
int to_next_line(struct editor_message_s * _msg)
{
    input_event * msg = (input_event *)_msg;
    auto d = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

    it_to_next_physical_line(*d->cursor_it());

    uint64_t rdr_end_off   = d->rdr_end()->offset();
    uint64_t cursor_off    = d->cursor_it()->offset();

    if (cursor_off > rdr_end_off) {
        it_to_next_physical_line(*d->rdr_begin());
        //        it_to_next_physical_line(*d->rdr_end()); // not needed
    }

    set_ui_change_flag(process_ev_ctx);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


int scroll_down(editor_buffer_id_t ed_buffer, uint32_t n, screen_t * screen)
{
    return EDITOR_STATUS_ERROR;
}


////////////////////////////////////////////////////////////////////////////////



/* TODO: check offset on screen
 * if the cursor is not on last line -> restart the screen with the 2nd line 1st offset
 *
 * compare the start of line offsets
 *
 * avoid layout recomputations by updating the last screen directly ? and send a clone ?
 *
*/
int to_next_screen_line_by_offset(struct editor_message_s * _msg, const uint64_t screen_offset, uint64_t & new_screen_offset, codepoint_info_s & next_start_cpi)
{
    new_screen_offset = screen_offset;

    input_event * msg = (input_event *)_msg;
    auto ed_buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    auto previous_screen = get_previous_screen_by_id(_msg->view_id);

    auto last_screen = previous_screen;

    // 1) need re-layout ?
    screen_line_t * first_l = nullptr;

    screen_line_t * prev_line = nullptr;

    screen_line_t * line = nullptr;
    codepoint_info_s * cpi = nullptr;
    size_t saved_line;
    size_t saved_column;
    size_t column;
    bool b;

    next_start_cpi = previous_screen->first_cpinfo;
    next_start_cpi.check_invariant();

    b = previous_screen->get_line(0, &first_l);
    if (!b) {
        assert(0);
        return EDITOR_STATUS_ERROR;
    }

    b = previous_screen->get_line_by_offset(screen_offset, &prev_line, saved_line, saved_column);
    if (!b) {

        // FIXME:   define editor_log() like printf // app_log << " cannot find offset " << screen_offset << " on screen\n";
        // FIXME:   define editor_log() like printf // app_log << " screen->first_cpinfo.offset = " << previous_screen->first_cpinfo.offset << "\n";
        // FIXME:   define editor_log() like printf // app_log << " screen->last_cpinfo.offset = " << previous_screen->last_cpinfo.offset << "\n";
        // FIXME:   define editor_log() like printf // app_log << " cannot find offset " << screen_offset << "on screen\n";

        // resync marks ?
        // assert(0);
        return EDITOR_STATUS_ERROR;
    }

    // FIXME:   define editor_log() like printf // app_log << " scr_line_idx " << saved_line << "\n";
    // FIXME:   define editor_log() like printf // app_log << " scr_col_idx  " << saved_column << "\n";
    // FIXME:   define editor_log() like printf // app_log << " screen->number_of_used_lines()  " <<  previous_screen->number_of_used_lines() << "\n";

    size_t scr_new_line_idx = saved_line + 1;
    auto number_of_used_lines = previous_screen->number_of_used_lines();
    if (scr_new_line_idx >= number_of_used_lines) {

        // have eof ?
        if (number_of_used_lines == 1) {
            // FIXME:   define editor_log() like printf // app_log << " END OF BUFFER DETECTED\n";
            return EDITOR_STATUS_ERROR;
        }

        // FIXME: if rdr_end on screen return
        // rebuild layout scroll down 1
        // FIXME : add function : scroll_down(screen_in, &screen_out);
        // FIXME:   define editor_log() like printf // app_log << "MUST SCROLL\n";
        b = previous_screen->get_line(1, &line);
        if (b == false) {
            // FIXME:   define editor_log() like printf // app_log << "cannot get line index 1\n";
            assert(0);
            return EDITOR_STATUS_ERROR;
        }

        b = line->get_first_cpinfo(&cpi, column);
        if (b == false) {
            assert(0);
            return EDITOR_STATUS_ERROR;
        }

        uint64_t saved_rdr_begin = ed_buffer->rdr_begin()->offset();

        cpi->check_invariant();
        next_start_cpi = *cpi;


        ed_buffer->rdr_begin()->move_to_offset(cpi->offset);

        // FIXME:   define editor_log() like printf // app_log << "cpi->offset " << cpi->offset << "\n";
        // FIXME:   define editor_log() like printf // app_log << "cpi->split_flag  = " << cpi->split_flag << "\n";
        // FIXME:   define editor_log() like printf // app_log << "cpi->split_count = " << cpi->split_count << "\n";

        auto new_screen = new screen_t(*previous_screen);
        build_screen_layout(ed_buffer, cpi, new_screen);

        bool b = new_screen->get_line_by_offset(ed_buffer->cursor_it()->offset(), &line, saved_line, saved_column);
        if (!b) {
            assert(0);
            ed_buffer->rdr_begin()->move_to_offset(saved_rdr_begin);
            delete new_screen;
            return EDITOR_STATUS_ERROR;
        }

        // FIXME:   define editor_log() like printf // app_log << " new scr_line_idx " << line << "\n";
        // FIXME:   define editor_log() like printf // app_log << " new scr_col_idx  " << column << "\n"; // FIXME
        // FIXME:   define editor_log() like printf // app_log << " new_screen->number_of_used_lines()  " <<  new_screen->number_of_used_lines() << "\n";

        scr_new_line_idx = saved_line + 1;

        if (scr_new_line_idx >= new_screen->number_of_used_lines()) {
            // FIXME:   define editor_log() like printf // app_log << "ERROR\n";
            ed_buffer->rdr_begin()->move_to_offset(saved_rdr_begin);
            delete new_screen;
            return EDITOR_STATUS_ERROR;
        }

        last_screen = new_screen;

    } else {

        // on screen
        last_screen->first_cpinfo.check_invariant();
        next_start_cpi = last_screen->first_cpinfo;
    }

    b = last_screen->get_line(scr_new_line_idx, &line);
    if (!b) {
        delete last_screen;
        return EDITOR_STATUS_OK;
    }

    column = std::min<uint32_t>(line->number_of_used_columns() - 1, saved_column);
    b = line->get_cpinfo((uint32_t)column, &cpi, screen_line_t::fix_column_overflow);
    if (!b) {
        delete last_screen;
        return EDITOR_STATUS_OK;
    }

    // FIXME: apply to to_previous_screen_line
    while ((prev_line->last_offset()) == cpi->offset && (column < line->number_of_used_columns() - 1)) {
        ++column;
        b = line->get_cpinfo((uint32_t)column, &cpi, screen_line_t::fix_column_overflow);
        if (!b) {
            break;
        }
    }

    new_screen_offset = cpi->offset;

    // FIXME:   define editor_log() like printf // app_log << "start_cpi->split_flag  = " << next_start_cpi.split_flag << "\n";
    // FIXME:   define editor_log() like printf // app_log << "start_cpi->split_count = " << next_start_cpi.split_count << "\n";

    next_start_cpi.check_invariant();

    set_ui_next_screen_start_cpi(process_ev_ctx, &next_start_cpi);
    set_ui_change_flag(process_ev_ctx);

    if (last_screen != previous_screen) {
        // FIXME:   define editor_log() like printf // app_log << "delete last_screen {\n";
        delete last_screen;
        // FIXME:   define editor_log() like printf // app_log << "}\n\n";
    }

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

// FIXME: remove cursor_it -> replace by mark list
int to_next_screen_line_internal(struct editor_message_s * _msg, codepoint_info_s & next_start_cpi)
{
    auto buffer = get_buffer_info_by_ll_bid(_msg->byte_buffer_id);

    uint64_t new_offset = buffer->cursor_it()->offset();
    auto b = to_next_screen_line_by_offset(_msg, new_offset, new_offset, next_start_cpi);
    if (b) {
        next_start_cpi.check_invariant();
    }

    buffer->cursor_it()->move_to_offset(new_offset);
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


// FIXME: must sync screen arround mark
// remove mark dep ?
int to_next_screen_line(struct editor_message_s * _msg)
{
    codepoint_info_s cpi;
    auto b = to_next_screen_line_internal(_msg, cpi);
    if (b) {
        cpi.check_invariant();
    }
    return b;
}



////////////////////////////////////////////////////////////////////////////////


/*
 *  FIXME: memory leaks screen list
 *  must update start cp info for next build layout call
 *  pass mark offset as parameter
 *  and add : cons uint64_t in_offset, uint64_t & out_offset
*/
int to_previous_screen_line_internal(struct editor_message_s * msg, codepoint_info_s ** next_start_cpi)
{
    // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : Enter\n";

    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    auto screen = get_previous_screen_by_id(msg->view_id);

    uint64_t offset = buffer->cursor_it()->offset();
    screen_line_t * l = nullptr;
    size_t scr_line_index = 0;
    size_t scr_col_index  = 0;
    bool found = screen->get_line_by_offset(offset, &l, scr_line_index, scr_col_index);
    if (!found) {
        return EDITOR_STATUS_OK;
    }

    // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : target col " << scr_col_index << "\n";
    // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : current scr_line_index " << scr_line_index << "\n";
    // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : current scr_col_index " << scr_col_index << "\n";


    // inside of screen move ?
    if (scr_line_index > 0) {

        scr_line_index--;
        found = screen->get_line(scr_line_index, &l);
        if (!found) {
            return EDITOR_STATUS_OK;
        }

        codepoint_info_s * cpi = nullptr;
        scr_col_index = std::min<uint32_t>(l->number_of_used_columns() - 1, scr_col_index);
        found = l->get_cpinfo((uint32_t)scr_col_index, &cpi);
        buffer->cursor_it()->move_to_offset(cpi->offset);

        // FIXME: copy screen first cpi, in ctx
        set_ui_change_flag(process_ev_ctx);
        set_ui_next_screen_start_cpi(process_ev_ctx, &screen->first_cpinfo);
        return EDITOR_STATUS_OK;
    }


    // outside of screen move
    {
        // FIXME: must fix reverse utf8 decoding

        if (buffer->rdr_begin()->offset() == 0) {
            return EDITOR_STATUS_OK;
        }

        --(*buffer->rdr_begin());
        buffer->rdr_begin()->toBeginningOfLine();;

        size_t list_total_lines;

        std::list<screen_t *> scr_list;
        build_screen_list(buffer,
                          buffer->rdr_begin()->offset(),
                          offset,
                          build_screen_no_hints,
                          screen->dimension(),
                          scr_list,
                          list_total_lines);

        size_t screen_index;
        size_t abs_line_index;

        // FIXME : pass walk order to this function, find_forward find_reverse
        screen_list_find_offset(scr_list,
                                offset,
                                screen_index,
                                abs_line_index,
                                scr_line_index,
                                &l,
                                scr_col_index);

        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : screen_index " << screen_index << "\n";
        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : abs_line_index " << abs_line_index << "\n";
        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : scr_line_index " << scr_line_index << "\n";
        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : scr_col_index " << scr_col_index << "\n";

        screen_t * scr = nullptr;
        auto it  = scr_list.begin();
        auto end = scr_list.end();
        for (size_t count = 0; count != screen_index; ++count) {
            ++it;
            if (it == end)
                return EDITOR_STATUS_ERROR;
        }
        scr = *it;

        found = scr->get_line_by_offset(offset, &l, scr_line_index, scr_col_index);
        if (!found) {
            return EDITOR_STATUS_OK;
        }

        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : new scr_line_index " << scr_line_index << "\n";
        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : new scr_col_index " << scr_col_index << "\n";

        if (scr_line_index)
            scr_line_index--;
        found = scr->get_line(scr_line_index, &l);
        if (!found) {
            return EDITOR_STATUS_OK;
        }

        codepoint_info_s * cpi = nullptr;
        scr_col_index = std::min<uint32_t>(l->number_of_used_columns() - 1, scr_col_index);
        found = l->get_cpinfo((uint32_t)scr_col_index, &cpi);
        buffer->cursor_it()->move_to_offset(cpi->offset);

        found = l->get_cpinfo((uint32_t)0, &cpi);
        buffer->rdr_begin()->move_to_offset(cpi->offset);

        set_ui_change_flag(process_ev_ctx);
        set_ui_next_screen_start_cpi(process_ev_ctx, cpi);
    }

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


int to_previous_screen_line(struct editor_message_s * msg)
{
    return to_previous_screen_line_internal(msg, nullptr);
}




////////////////////////////////////////////////////////////////////////////////

/* if end - start > screen set to begin */
int goto_beginning_of_line(struct editor_message_s * msg)
{
    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";

    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    auto screen = get_previous_screen_by_id(msg->view_id);

    screen_line_t * l;
    size_t scr_line_index;
    size_t scr_col_index;

    text_buffer::iterator & rdr_it = *buffer->rdr_begin();
    //  uint64_t old_rdr_begin = rdr_it.offset();

    text_buffer::iterator & it = *buffer->cursor_it();

    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";
    it.toBeginningOfLine();
    assert(it.column() == 0);
    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";

    uint64_t cur_off = it.offset();

    codepoint_info_s start_cpi;

    bool found = screen->get_line_by_offset(cur_off, &l, scr_line_index, scr_col_index);
    if (!found) {
        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : ! found on screen\n";
        rdr_it = it;
    } else {
        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : found cursor @ (l="<<scr_line_index<< ", c=" << scr_col_index<< ")\n";
        start_cpi = screen->first_cpinfo;
    }

    while (!found) {
        found = screen->get_line_by_offset(cur_off, &l, scr_line_index, scr_col_index);
        if (found) {
            // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " found cursor_offset("<<cur_off<<") on screen\n";
            start_cpi = screen->first_cpinfo;
            break;
        }
        page_up_internal(msg, start_cpi);
        build_screen_layout(msg, &start_cpi, screen);
    }

    assert(it.column() == 0);

    // need centering ?
    // while !found offset page_up();

    assert(start_cpi.cp_index != uint64_t(-1));

    set_mark_changed_flag(process_ev_ctx);
    set_ui_next_screen_start_cpi(process_ev_ctx, &start_cpi);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

int goto_end_of_line(struct editor_message_s * msg)
{
    // // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";

    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    auto screen = get_previous_screen_by_id(msg->view_id);

    text_buffer::iterator & it = *buffer->cursor_it(); // take cursor ref

    auto t0 = ew::core::time::get_ticks();
    it.toEndOfLine();
    uint64_t cur_off = it.offset();
    auto t1 = ew::core::time::get_ticks();
    // FIXME:   define editor_log() like printf // app_log << "tb = " << (t1 - t0) << " ms\n";

    codepoint_info_s * start_cpi = nullptr;

    int count = 0;
    bool found = false;
    while (!found) {
        screen_line_t * l;
        size_t scr_line_index;
        size_t scr_col_index;
        found = screen->get_line_by_offset(cur_off, &l, scr_line_index, scr_col_index);
        if (found) {
            // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " found cursor_offset("<<cur_off<<") on screen\n";
            break;
        }
        bool ret = fast_page_down(buffer, screen, &start_cpi);
        if (ret != true)
            break;

        build_screen_layout(buffer, start_cpi, screen);
        ++count;
    }

    auto t2 = ew::core::time::get_ticks();

    // FIXME:   define editor_log() like printf // app_log << "toEndOfLine = " << (t1 - t0) << " ms\n";
    // FIXME:   define editor_log() like printf // app_log << "page down count(" << count << ") = " << (t2 - t1) << " ms\n";

    // need centering ?
    // while !found offset page_up();

    set_mark_changed_flag(process_ev_ctx);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

int goto_beginning_of_screen_line(struct editor_message_s * _msg)
{
    // // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

int goto_end_of_screen_line(struct editor_message_s * _msg)
{
    // // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

// move to buffer ?
struct {
    uint64_t bid;

    int b_x;
    int b_y;
    uint64_t b_offset;
    bool use = false;

    int e_x;
    int e_y;
    uint64_t e_offset;

    codepoint_info_s start_cpi;

} selection_record;

////////////////////////////////////////////////////////////////////////////////


int begin_selection(struct editor_message_s * msg)
{
    // // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";

    selection_record.use = true;

    auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);


    switch (msg->type & EDITOR_EVENT_TYPE_FAMILY_MASK) {

    case EDITOR_POINTER_BUTTON_EVENT_FAMILY: {
        //compute target offset
        auto scr = get_previous_screen_by_id(msg->view_id);

        selection_record.start_cpi = scr->first_cpinfo;
        set_ui_next_screen_start_cpi(process_ev_ctx, &selection_record.start_cpi);

        button_event * bev = static_cast<button_event *>(msg);

        codepoint_info_s * cpi = nullptr;
        scr->get_codepoint_by_coords(bev->ev->x, bev->ev->y, &cpi);
        // FIXME:   define editor_log() like printf // app_log << "cpi = " << cpi << "\n";
        if (cpi) {
            selection_record.e_offset = selection_record.b_offset = cpi->offset;


            // FIXME:   define editor_log() like printf // app_log << "cpi->offset = " << cpi->offset << "\n";
            buff->cursor_it()->move_to_offset(selection_record.e_offset);
            set_ui_change_flag(process_ev_ctx);
            ;
        } else {
            selection_record.e_offset = selection_record.b_offset = buff->cursor_it()->offset();

        }

        // save cursor _pose
    }
    break;


    case EDITOR_KEYBOARD_EVENT: {
        // save cursor pos
        selection_record.e_offset = selection_record.b_offset = buff->cursor_it()->offset();
    }
    break;


    default: {

    }
    }


    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


int end_selection(struct editor_message_s * msg)
{
    // // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";
    auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    auto scr = get_previous_screen_by_id(msg->view_id);
    selection_record.start_cpi = scr->first_cpinfo;
    set_ui_next_screen_start_cpi(process_ev_ctx, &selection_record.start_cpi);


    if (selection_record.b_offset == selection_record.e_offset) {
        // move cursor to

        buff->cursor_it()->move_to_offset(selection_record.e_offset);

        set_ui_change_flag(process_ev_ctx);
        set_ui_next_screen_start_cpi(process_ev_ctx, &selection_record.start_cpi);
    }

    selection_record.use = false;
    return EDITOR_STATUS_OK;
}

///////////////////////////////////////////////////////////////////////////////

int insert_codepoint(struct editor_message_s * _msg)
{
    input_event * msg = static_cast<input_event *>(_msg);

    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

    int32_t codepoint = msg->ev->start_value;

    text_buffer::iterator & it = *buffer->cursor_it();

    // save the view "important" marks cursor
    // should provide an atomatic refresh for registered iterators...
    uint64_t rdr_begin_off =  buffer->rdr_begin()->offset();
    uint64_t rdr_end_off   =  buffer->rdr_end()->offset();
    uint64_t cursor_off    =  buffer->cursor_it()->offset();

    // not needed ?
    //    std::lock_guard < decltype(*buffer->txt_buffer()) > lock(*buffer->txt_buffer());

    // the actual insert
    buffer->txt_buffer()->insert_codepoint(msg->byte_buffer_id, codepoint, it);

    // update view cursors/marks
    // should provide an atomatic refresh for registered iterators...
    *(buffer->rdr_begin()) = buffer->txt_buffer()->get_iterator(rdr_begin_off);
    *(buffer->rdr_end())   = buffer->txt_buffer()->get_iterator(rdr_end_off);
    *(buffer->cursor_it()) = buffer->txt_buffer()->get_iterator(cursor_off);

    if (cursor_off > rdr_end_off) {
        it_to_next_physical_line(*buffer->rdr_begin());
        it_to_next_physical_line(*buffer->rdr_end());
    }

    set_buffer_changed_flag(process_ev_ctx);

    // update cursor : put cursor after last inserted byte
    to_next_char(msg);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

int insert_newline(struct editor_message_s * _msg)
{
    // FIXME: CR LF ?
    // input_event * msg = (input_event *)_msg;
    // msg->ev->start_value = '\r';
    // insert_codepoint(_msg);

    input_event * msg = (input_event *)_msg;
    msg->ev->start_value = '\n';
    return insert_codepoint(_msg);
}

////////////////////////////////////////////////////////////////////////////////

/* general expected behavior

  -) update low level buffer
  -) refresh all higher structs that depends on the updated range(s)
  -) set ctx flags to indicate the change (+ the changed range )

  fundamental-mode : refresh text layout

  send the new computed "screen" to the registered ui (found in msg/ ctx->viewers ?)

  the new screen is computed using the ctx->current_screen (remmeber that keyboard event is from )
*/
int  remove_current_char(struct editor_message_s * msg)
{
    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

    std::lock_guard < decltype(*buffer->txt_buffer()) > lock(*buffer->txt_buffer());

    buffer->cursor_it()->check_invariants();
    buffer->rdr_begin()->check_invariants();
    buffer->rdr_end()->check_invariants();

    // save cursors
    uint64_t rdr_begin_off = buffer->rdr_begin()->offset();
    uint64_t rdr_end_off   = buffer->rdr_end()->offset();
    uint64_t cursor_off    = buffer->cursor_it()->offset();

    // the actual remove
    buffer->txt_buffer()->remove_current_char(msg->byte_buffer_id, *buffer->cursor_it());

    // restore cursors
    *(buffer->rdr_begin()) = buffer->txt_buffer()->get_iterator(rdr_begin_off);
    *(buffer->rdr_end())   = buffer->txt_buffer()->get_iterator(rdr_end_off);
    *(buffer->cursor_it()) = buffer->txt_buffer()->get_iterator(cursor_off);

    buffer->cursor_it()->check_invariants();
    buffer->rdr_begin()->check_invariants();
    buffer->rdr_end()->check_invariants();

    set_buffer_changed_flag(process_ev_ctx);


    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

// TODO: gfx_ctx->center_screen arround cursor = true;
int remove_previous_char(struct editor_message_s * msg)
{
    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

    text_buffer::iterator & it = *buffer->cursor_it();

    if (it.offset() != 0) {
        --it;
        remove_current_char(msg);
    }

    set_buffer_changed_flag(process_ev_ctx);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

// TODO: use intermediate cursor class, pass cursor as parameter
int to_beginning_of_line(struct editor_message_s * msg)
{
    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

    // linear move
    buffer->cursor_it()->toBeginningOfLine();


    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

int to_end_of_line(struct editor_message_s * msg)
{
    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

    std::lock_guard < decltype(*buffer->txt_buffer()) > lock(*buffer->txt_buffer());

    // linear move
    uint64_t off = buffer->cursor_it()->offset();
    buffer->cursor_it()->toEndOfLine();
    uint64_t off2 = buffer->cursor_it()->offset();
    assert(off <= off2);
    if (off == off2) {
        // FIXME:   define editor_log() like printf // app_log << __FUNCTION__  << "(off == off2)\n";
        return EDITOR_STATUS_OK;
    }

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

// need unicode character class

// dump version : what behavior in fold ?

int belong_to_word(const int32_t c)
{
    // need char class
    if ((c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c == '_')) {
        return EDITOR_STATUS_OK;
    }

    return EDITOR_STATUS_ERROR;
}


// dump version : what behavior in fold ?
int right_word(struct editor_message_s * msg)
{
    auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    uint64_t save_off = buff->cursor_it()->offset(); // push_mark // pop_mark -> mark_stack

    auto end_off = buff->raw_buffer()->size();
    if (save_off == end_off) {
        return EDITOR_STATUS_OK;
    }

    auto & it = *buff->cursor_it();

    bool start_on_word = belong_to_word(*it);

    if (start_on_word == false ) {
        while (!belong_to_word(*it)) {
            ++it;
        }
    }

    while (belong_to_word(*it)) {
        ++it;
    }

    set_ui_change_flag(process_ev_ctx);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

// dump version : what behavior in fold ?
int left_word(struct editor_message_s * msg)
{
    auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    uint64_t save_off = buff->cursor_it()->offset(); // push_mark // pop_mark -> mark_stack
    if (save_off == 0)
        return EDITOR_STATUS_OK;

    auto & it = *buff->cursor_it();

    // get word start
    while (belong_to_word(*it)) {
        --it;
        if (it.offset() == 0) {
            set_ui_change_flag(process_ev_ctx);
            return EDITOR_STATUS_OK;
        }
    }

    // get prev word end
    while (!belong_to_word(*it)) {
        --it;
        if (it.offset() == 0) {
            set_ui_change_flag(process_ev_ctx);
            return EDITOR_STATUS_OK;
        }
    }

    // get word start
    while (belong_to_word(*it)) {
        --it;
        if (it.offset() == 0) {
            set_ui_change_flag(process_ev_ctx);
            return EDITOR_STATUS_OK;
        }
    }

    // ensure word start
    ++it;

    set_ui_change_flag(process_ev_ctx);
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


int mouse_wheel_up(struct editor_message_s * msg)
{
    auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    uint64_t save_off = buff->cursor_it()->offset(); // push_mark // pop_mark -> mark_stack

    for (int i = 0; i < 3; ++i) {
        buff->cursor_it()->move_to_offset( buff->rdr_begin()->offset() );

        codepoint_info_s * next_start_cpi = nullptr;
        to_previous_screen_line_internal(msg, &next_start_cpi);

        // this update last last ref: FIXME: remove shared pointers and use copy operator, we really mean it
        // AND ALWAYS send a copy to the ui
        notify_buffer_changes(msg, next_start_cpi, false);

    }

    buff->cursor_it()->move_to_offset(save_off);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


int mouse_wheel_down(struct editor_message_s * msg)
{
    auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    uint64_t save_off = buff->cursor_it()->offset(); // push_mark // pop_mark -> mark_stack

    for (int i = 0; i < 3; ++i) {
        buff->cursor_it()->move_to_offset( buff->rdr_end()->offset() );
        codepoint_info_s next_start_cpi;
        to_next_screen_line_internal(msg, next_start_cpi);
        next_start_cpi.check_invariant();
        notify_buffer_changes(msg, &next_start_cpi, false);
    }

    buff->cursor_it()->move_to_offset(save_off);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

// text-mode
// FIXME: move to text-mode init
/*
 every module function takes
 id(0)
 will not be allowed
 { buffer_id: id, screen_id: id, repeat }
*/

#endif
