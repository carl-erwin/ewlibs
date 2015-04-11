#define _X_OPEN_SOURCE_EXTENDED

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <ncurses.h>

#include "ew/core/time/time.hpp"

#include "ui.h"
#include "editor_message.h"
#include "byte_buffer.h"
#include "editor_buffer.h"
#include "editor_view.h"
#include "screen.h"

#include "../../core/core.hpp"
#include "../../core/input/event/input_event_map.hpp"


// simple enough, no size checks
bool utf8_put_cp(const int32_t codepoint, u8 * utf8)
{
    if (codepoint < 0x80) {
        utf8[0] = codepoint & 0x7F;
        utf8[1] = 0;
        return true;
    }

    if (codepoint < 0x800) {
        utf8[0] = 0xC0 | (codepoint >> 6);
        utf8[1] = 0x80 | (codepoint & 0x3F);
        utf8[2] = 0;
        return true;
    }

    if (codepoint < 0xFFFF) {
        utf8[0] = 0xE0 | (codepoint >> 12);
        utf8[1] = 0x80 | (codepoint >> 6);
        utf8[2] = 0x80 | (codepoint & 0x3F);
        utf8[3] = 0;
        return true;
    }

    if (codepoint < 0x10FFFF) {
        utf8[0] = 0xF0 | (codepoint >> 18);
        utf8[1] = 0x80 | (codepoint >> 12);
        utf8[2] = 0x80 | (codepoint >> 6);
        utf8[3] = 0x80 | (codepoint & 0x3F);
        utf8[4] = 0;
        return true;
    }

    utf8[0] = 0;
    return false;
}

namespace eedit
{

///////////////////////////////////////////////////////////////////////////////////////////////////

class application;


class ncurses_display : public ew::graphics::gui::display
{
public:

};


enum ui_state_e {
    request_buffer_id_list,
    ui_ready
};


struct ncurses_ui_interface : public user_interface {
    virtual ~ncurses_ui_interface();
    virtual bool setup(application * app);
    virtual bool main_loop();
    virtual ew::graphics::gui::display * get_display();

    ncurses_display * dpy = nullptr; // FIXME: not allocted

    struct editor_event_queue_s * m_event_queue = nullptr;

    bool process_editor_ui_event(struct editor_message_s * msg);
    bool process_editor_new_layout_ui_event(struct editor_message_s * msg);
    bool process_editor_new_rpc_answer_ui_event(struct editor_message_s * msg);
    bool quit();

    bool send_rpc_event(const int ac,  const char ** av, editor_buffer_id_t ebid, byte_buffer_id_t buffer_id, uint64_t screen_id, const screen_dimension_t & screen_dim);
    bool send_build_layout_event(uint32_t w, uint32_t h) const;
    struct editor_input_event_s ncurses_keycode_to_eedit_event(int keycode);

    byte_buffer_id_t m_cur_ebid = 0; // current buffer;
    editor_view_id_t m_cur_view_id = 0;

    ui_state_e ui_state = request_buffer_id_list;

    bool m_quit = false;
    // TODO: get_number_of_openned_buffers() -> size_t;
    // TODO: int get_openned_buffers_ids(byte_buffer_id_t bids[], size_t max)
};


static ncurses_ui_interface * ncurses_ui = nullptr;

ncurses_ui_interface::~ncurses_ui_interface()
{
    delete dpy;
    endwin();
}

bool ncurses_ui_interface::setup(application * app)
{
    return true;
}



bool ncurses_ui_interface::send_rpc_event(const int ac,  const char ** av, editor_buffer_id_t ebid, byte_buffer_id_t buffer_id, uint64_t screen_id, const screen_dimension_t & screen_dim)
{
    struct editor_message_s * msg = editor_rpc_call_new(ac, av);
    msg->src.kind  =  EDITOR_ACTOR_UI;
    msg->src.queue =  m_event_queue;  //  TODO: ctx ?
    msg->dst.kind  =  EDITOR_ACTOR_CORE;
    msg->editor_buffer_id  =  ebid;
    msg->byte_buffer_id    =  buffer_id;
    msg->view_id           =  screen_id;
    msg->screen_dim        =  screen_dim;

    assert(screen_dim.w);
    assert(screen_dim.h);

    app_log << " send_rcp_event : ui -> core @" << ew::core::time::get_ticks() << "\n";

    eedit::core::push_event(msg);
    return true;
}


void sigint_handler(int sig)
{
    // TODO: build state machine
    int row;
    int col;
    getmaxyx(stdscr, row, col);

    struct editor_message_s * msg = editor_event_alloc();
    msg->type = EDITOR_KEYBOARD_EVENT;

    msg->id               = 0; // FIXME: id++
    msg->editor_buffer_id = ncurses_ui->m_cur_ebid;
    msg->byte_buffer_id   = 0;
    msg->view_id          = ncurses_ui->m_cur_view_id;

    msg->screen_dim.w = col * get_application()->font_width();
    msg->screen_dim.h = row * get_application()->font_height();
    msg->screen_dim.c = col;
    msg->screen_dim.l = row;

    msg->src.kind  = EDITOR_ACTOR_UI;
    msg->src.queue = ncurses_ui->m_event_queue;
    msg->dst.kind  = EDITOR_ACTOR_CORE;

    msg->input.ev = ncurses_ui->ncurses_keycode_to_eedit_event(3);

    // display current key on console
    editor_input_event_dump(&msg->input.ev, __PRETTY_FUNCTION__);

    app_log << __PRETTY_FUNCTION__ << " send quit app event : ui -> core @" << ew::core::time::get_ticks() << "\n";
    eedit::core::push_event(msg);
}

bool ncurses_ui_interface::main_loop()
{
    ncurses_ui = this;

    m_event_queue = editor_event_queue_new();

    signal(SIGINT, sigint_handler);

    // init
    initscr();
    m_cur_view_id = (editor_view_id_t)stdscr;
    curs_set(0); 		// disable cursor
    keypad(stdscr, TRUE); /* for F1, arrow etc ... */
    clear();
    noecho();
    raw();
    halfdelay(1); // 20ms

    // TODO: build state machine
    int row;
    int col;
    getmaxyx(stdscr, row, col);

    app_log << " screen row = " << row << "\n";
    app_log << " screen col = " << col << "\n";
    app_log << " vscreen w = " << col * get_application()->font_width()  << "\n";
    app_log << " vscreen h = " << row * get_application()->font_height() << "\n";

    // FIXME: depending on ui the font must be monospace...

    const char * func = "get_buffer_id_list";

    screen_dimension_t scr_dim {
        uint32_t(row),
        uint32_t(col),
        uint32_t(col * get_application()->font_width()),
        uint32_t(row * get_application()->font_height())
    };

    ui_state = request_buffer_id_list;
    send_rpc_event(1,  &func, 0, 0, (uint64_t)stdscr, scr_dim);

    auto q = m_event_queue;

    while (!m_quit) {

        editor_event_queue_wait(q, 1);
        auto nr = editor_event_queue_size(q);
        struct editor_message_s * core_msg = nullptr;
        while (nr) {
            core_msg = editor_event_queue_get(q);
            process_editor_ui_event(core_msg);
            --nr;
        }

        if (m_quit)
            break;

        int keycode = wgetch(stdscr);			/* Wait for user input */ // FIXE: use timer to check core event
        if (keycode == ERR)
            continue;

        app_log << " keycode = " << keycode << "\n";


        const char *name = keyname( keycode );
        app_log << "ncurses : you entered: code(" << keycode << ") -> '" << name <<"'\n";

        auto msg              = editor_event_alloc();
        msg->type             = EDITOR_KEYBOARD_EVENT;
        msg->id               = 0; // FIXME: id++
        msg->editor_buffer_id = m_cur_ebid;
        assert(msg->editor_buffer_id);
        msg->byte_buffer_id   = 0;
        msg->view_id          = m_cur_view_id;             //
        assert(msg->view_id);

        msg->screen_dim.w = col * get_application()->font_width();
        msg->screen_dim.h = row * get_application()->font_height();
        msg->screen_dim.c = col;
        msg->screen_dim.l = row;

        msg->src.kind  = EDITOR_ACTOR_UI;
        msg->src.queue = m_event_queue;
        msg->dst.kind  = EDITOR_ACTOR_CORE;

#if 0
        if (ev->ctrl != false) mod_mask |= input_event_s::mod_ctrl;
        if (ev->altL != false) mod_mask |= input_event_s::mod_altL;
        if (ev->altR != false) mod_mask |= input_event_s::mod_altR;
        // if (ev->ctrl != false) mod_mask |= keymap_key::mod_oskey;
#endif
        msg->input.ev = ncurses_keycode_to_eedit_event(keycode);

        // display current key on console
        editor_input_event_dump(&msg->input.ev, __PRETTY_FUNCTION__);

        app_log << "\n";

        app_log << " send quit app event : ui -> core @" << ew::core::time::get_ticks() << "\n";
        eedit::core::push_event(msg);
    }
#if 0
    while (win->loop() == true) {
        // TODO: abstract for all registered ui
        // here application is tied to ew/ui
        win->process_event_queue();
        process_events();
    }

    gui_dpy->lock();
    delete win;
    gui_dpy->unlock();

    gui_dpy->close();
    delete gui_dpy;
    gui_dpy = nullptr;
#endif

    endwin();
    resetty();

    return true;
}

ew::graphics::gui::display * ncurses_ui_interface::get_display()
{
    return static_cast<ew::graphics::gui::display *>(dpy);
}


eedit::user_interface * new_ncurses_ui()
{
    return new ncurses_ui_interface();
}


/*
 * Pseudo-character tokens outside ASCII range.  The curses wgetch() function
 * will return any given one of these only if the corresponding k- capability
 * is defined in your terminal's terminfo entry.
 *
 * Some keys (KEY_A1, etc) are arranged like this:
 *	a1     up    a3
 *	left   b2    right
 *	c1     down  c3
 *
 * A few key codes do not depend upon the terminfo entry.
 */


struct editor_input_event_s ncurses_ui_interface::ncurses_keycode_to_eedit_event(int keycode)
{

    editor_input_event_s iev;

    memset(&iev, 0, sizeof (iev));

    iev.key = NO_KEY;

    uint32_t mod_mask = 0;
    uint32_t unicode  = 0;

    switch (keycode) {
    case KEY_DOWN:
        iev.key =  Down;
        break;
    case KEY_UP:
        iev.key =  Up;
        break;
    case KEY_LEFT:
        iev.key =  Left;
        break;
    case KEY_RIGHT:
        iev.key =  Right;
        break;

    case 0 ... 26: {
        iev.key =  UNICODE;
        switch (keycode + 'a' - 1) {
        case 'j': {
            unicode = '\n';
        }
        break;
        case 'i': {
            unicode = '\t';
        }
        break;
        default: {
            unicode = keycode + 'a' - 1;
            mod_mask |= mod_ctrl;
        }
        break;
        }
    }
    break;

    case 27 ... 127: {
        iev.key =  UNICODE;
        unicode = keycode;
    }
    break;

    case KEY_HOME:
        iev.key =  Home;
        break;

    case KEY_BACKSPACE:
        iev.key =  BackSpace;
        break;

    case KEY_NPAGE:
        iev.key =  PageDown;
        break;
    case KEY_PPAGE:
        iev.key =  PageUp;
        break;


    case KEY_END:
        iev.key =  End;
        break;

    case KEY_SEND: {
        iev.key =  End;
        mod_mask |= mod_shift;
    }
    break;


    case KEY_SHOME: {
        iev.key =  Home;
        mod_mask |= mod_shift;
    }
    break;

    case KEY_DC: {
        /* delete-character key */
        iev.key =  Delete;
    }
    break;

    default: {
        abort();

#if 0
        //  KEY_CODE_YES:		// 0400	       /* A wchar_t contains a key code */
        //  KEY_MIN: //		0401		/* Minimum curses key */
        //  KEY_BREAK: //		0401		/* Break key (unreliable) */
        //  KEY_SRESET: //		0530		/* Soft (partial) reset (unreliable) */
        //  KEY_RESET: //		0531		/* Reset or hard reset (unreliable) */
#endif

#if 0

        // KEY_F0		0410		/* Function keys.  Space for 64 */
        // KEY_F(n)	(KEY_F0+(n))	/* Value of function key n */
        // KEY_DL		0510		/* delete-line key */
        // KEY_IL		0511		/* insert-line key */
        // KEY_DC		0512		/* delete-character key */
        // KEY_IC		0513		/* insert-character key */
        // KEY_EIC		0514		/* sent by rmir or smir in insert mode */
        // KEY_CLEAR	0515		/* clear-screen or erase key */
        // KEY_EOS		0516		/* clear-to-end-of-screen key */
        // KEY_EOL		0517		/* clear-to-end-of-line key */
        // KEY_SF		0520		/* scroll-forward key */
        // KEY_SR		0521		/* scroll-backward key */
#endif

#if 0
        // KEY_STAB	0524		/* set-tab key */
        // KEY_CTAB	0525		/* clear-tab key */
        // KEY_CATAB	0526		/* clear-all-tabs key */
        // KEY_ENTER	0527		/* enter/send key */
        // KEY_PRINT	0532		/* print key */
        // KEY_LL		0533		/* lower-left key (home down) */
        // KEY_A1		0534		/* upper left of keypad */
        // KEY_A3		0535		/* upper right of keypad */
        // KEY_B2		0536		/* center of keypad */
        // KEY_C1		0537		/* lower left of keypad */
        // KEY_C3		0540		/* lower right of keypad */
        // KEY_BTAB	0541		/* back-tab key */
        // KEY_BEG		0542		/* begin key */
        // KEY_CANCEL	0543		/* cancel key */
        // KEY_CLOSE	0544		/* close key */
        // KEY_COMMAND	0545		/* command key */
        // KEY_COPY	0546		/* copy key */
        // KEY_CREATE	0547		/* create key */
#endif
#if 0
        // KEY_EXIT	0551		/* exit key */
        // KEY_FIND	0552		/* find key */
        // KEY_HELP	0553		/* help key */
        // KEY_MARK	0554		/* mark key */
        // KEY_MESSAGE	0555		/* message key */
        // KEY_MOVE	0556		/* move key */
        // KEY_NEXT	0557		/* next key */
        // KEY_OPEN	0560		/* open key */
        // KEY_OPTIONS	0561		/* options key */
        // KEY_PREVIOUS	0562		/* previous key */
        // KEY_REDO	0563		/* redo key */
        // KEY_REFERENCE	0564		/* reference key */
        // KEY_REFRESH	0565		/* refresh key */
        // KEY_REPLACE	0566		/* replace key */
        // KEY_RESTART	0567		/* restart key */
        // KEY_RESUME		0570		/* resume key */
        // KEY_SAVE		0571		/* save key */
        // KEY_SBEG		0572		/* shifted begin key */
        // KEY_SCANCEL	0573		/* shifted cancel key */
        // KEY_SCOMMAND	0574		/* shifted command key */
        // KEY_SCOPY		0575		/* shifted copy key */
        // KEY_SCREATE	0576		/* shifted create key */
        // KEY_SDC		0577		/* shifted delete-character key */
        // KEY_SDL		0600		/* shifted delete-line key */
        // KEY_SELECT		0601		/* select key */

#endif

#if 0
        // KEY_SEOL		0603		/* shifted clear-to-end-of-line key */
        // KEY_SEXIT		0604		/* shifted exit key */
        // KEY_SFIND		0605		/* shifted find key */
        // KEY_SHELP		0606			/* shifted help key */
#endif

#if 0
        // KEY_SIC		0610		/* shifted insert-character key */
        // KEY_SLEFT		0611		/* shifted left-arrow key */
        // KEY_SMESSAGE	0612		/* shifted message key */
        // KEY_SMOVE		0613		/* shifted move key */
        // KEY_SNEXT		0614		/* shifted next key */
        // KEY_SOPTIONS	0615		/* shifted options key */
        // KEY_SPREVIOUS	0616		/* shifted previous key */
        // KEY_SPRINT		0617		/* shifted print key */
        // KEY_SREDO		0620		/* shifted redo key */
        // KEY_SREPLACE	0621		/* shifted replace key */
        // KEY_SRIGHT		0622		/* shifted right-arrow key */
        // KEY_SRSUME		0623		/* shifted resume key */
        // KEY_SSAVE		0624		/* shifted save key */
        // KEY_SSUSPEND	0625		/* shifted suspend key */
        // KEY_SUNDO		0626		/* shifted undo key */
        // KEY_SUSPEND	0627		/* suspend key */
        // KEY_UNDO		0630		/* undo key */
        // KEY_MOUSE:
        // KEY_RESIZE:
        break;
        // KEY_EVENT:
        break;
        // KEY_MAX:
        break;
#endif

        iev.type = keypress;
        iev.key = NUL;
        iev.is_range = false;
        iev.button_press_mask = 0;
        iev.start_value = unicode;
        iev.end_value = unicode;
        return iev;

    }
    break;


    } // ! switch(keycode)

    iev.type = keypress;
    iev.is_range = false;
    iev.button_press_mask = 0;
    iev.start_value = unicode;
    iev.end_value = unicode;

    iev.ctrl = mod_mask & mod_ctrl;
    iev.shift = mod_mask & mod_shift;

    return iev;
}



////////
bool ncurses_ui_interface::process_editor_new_layout_ui_event(struct editor_message_s * msg)
{
    int row;
    int col;
    getmaxyx(stdscr, row, col);
    clear();				/* clear the screen */


    const auto scr = msg->layout.screen;
    uint32_t limax = std::min((uint32_t)row, screen_get_number_of_used_lines(scr));
    for (uint32_t li = 0; li < limax; li++) {

        const screen_line_t * l = nullptr;
        screen_get_line(scr, li, &l);
        for (uint32_t c = 0; c < screen_line_get_number_of_used_columns(l); c++) {
            const codepoint_info_s * cpi;
            screen_line_get_cpinfo(l, c, &cpi, screen_line_hint_fix_column_overflow);

            if (cpi->is_selected) {
                attron(A_REVERSE);
            }
            if (cpi->codepoint < 128) {
                mvaddch(li, c, cpi->codepoint);
            } else {
                u8 buff[8];
                utf8_put_cp(cpi->codepoint, buff);
                mvaddstr(li, c, (const char *)buff);
            }
            if (cpi->is_selected) {
                attroff(A_REVERSE);
            }

        }
    }

    refresh();

    screen_release(msg->layout.screen);
    return true;
}

// FIXME: pass col,row
bool ncurses_ui_interface::send_build_layout_event(uint32_t w, uint32_t h) const
{
    app_log << __PRETTY_FUNCTION__ << " ui -> core @" << ew::core::time::get_ticks() << "\n";

    if (m_cur_ebid ==  0) {
        app_log << __PRETTY_FUNCTION__ << " no buffer selected found" << "\n";
        return false;
    }

    // ask for new layout
    auto msg             = editor_event_alloc();
    msg->type            = EDITOR_BUILD_LAYOUT_EVENT;
    msg->id              = 7; // TODO
    msg->src.kind        = EDITOR_ACTOR_UI;
    msg->src.queue       = m_event_queue;
    msg->dst.kind        = EDITOR_ACTOR_CORE;

    msg->editor_buffer_id  = m_cur_ebid;
    msg->byte_buffer_id  = 0;
    msg->view_id         = m_cur_view_id;

    int row;
    int col;
    getmaxyx(stdscr, row, col);
    msg->screen_dim = {
        uint32_t(row),
        uint32_t(col),
        uint32_t(col * get_application()->font_width()),
        uint32_t(row * get_application()->font_height())
    };

    app_log << " send_build_layout_event : ui -> core @" << ew::core::time::get_ticks() << "\n";

    eedit::core::push_event(msg);
    return true;
}


bool ncurses_ui_interface::process_editor_new_rpc_answer_ui_event(struct editor_message_s * msg)
{
    app_log << __FUNCTION__ << "  recv " << msg->rpc.av[0] << "\n";

    if (msg->rpc.ac == 0) {
        assert(0);
        return false;
    }

    std::string cmd(msg->rpc.av[0]);

    switch (ui_state) {
    case request_buffer_id_list: {
        if (cmd ==  "get_buffer_id_list") {
            if (msg->rpc.ac < 2) {
                return false;
            }

            this->m_cur_ebid = atoi(msg->rpc.av[1]);

            assert(m_cur_ebid);

            app_log << __PRETTY_FUNCTION__ << " select buffer_id " <<  m_cur_ebid <<  "\n";
            send_build_layout_event(0,0);
            ui_state = ui_ready; // FIXME: do this when the first layout is received
        }
    }

    default:
        break;
    }



    return true;
}

bool ncurses_ui_interface::quit()
{
    m_quit = true;
    return true;
}


bool ncurses_ui_interface::process_editor_ui_event(struct editor_message_s * msg)
{
    bool ret = false;

    switch (msg->type) {
    case EDITOR_LAYOUT_NOTIFICATION_EVENT: {
        ret = process_editor_new_layout_ui_event(msg);
    }
    break;

    case EDITOR_RPC_ANSWER_EVENT: {
        ret = process_editor_new_rpc_answer_ui_event(msg);
    }
    break;

    case EDITOR_QUIT_APPLICATION_DEFAULT:
    case EDITOR_QUIT_APPLICATION_FORCED: {
        quit();
    }
    break;

    default: {
        app_log << __PRETTY_FUNCTION__ << " unhandled event msg->type(" << msg->type << ")\n";
        assert(0);
    }

    }

    editor_event_free(msg);
    return ret;
}

} // ! namespace eedit
