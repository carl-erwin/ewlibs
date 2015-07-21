#pragma once

#include "ew/graphics/gui/widget/widget.hpp"
#include "ui/ew/text_view.hpp"


#include "font_utils.hpp"


namespace  eedit
{

class line_number_view : public ew::graphics::gui::widget
{
public:
    text_view * m_txt_view = nullptr;

    line_number_view(ew::graphics::gui::widget * parent)
    {
        set_parent(parent);
        set_name("line number view");
    }

    virtual ~line_number_view()
    {
        m_txt_view = nullptr;
    }

    void set_textview(text_view * txt_view)
    {
        m_txt_view = txt_view;
    }

    virtual bool render()
    {
        return true;

        // border
        ew_glColor4ub(0x77, 0x77, 0x77, 0xff);
        glBegin(GL_LINES);
        {
            glVertex2i(0 + width() - 1, 0);           // up
            glVertex2i(0 + width() - 1, height());    // down
        }
        glEnd();

        if (m_txt_view == nullptr) {
            return true;
        }

        char buffer[32];
        int buffer_len = 0;
        ew::graphics::colors::color3ub col(24, 24, 24); // TODO

        uint64_t current_line = 0;
        uint64_t prev_line = 0;

        uint32_t nr_lines = 1000;

        //uint64_t prev_offset = 0;
        uint64_t line_idx = 0;

        uint32_t max_line_index = m_txt_view->max_line_index();

        bool have_real_line = false;

        auto m_ft = get_font();
        for (int32_t y = 0;
             y + (int32_t)m_ft->height_in_pixels() < (int32_t)height() && (line_idx < max_line_index);
             y += m_ft->height_in_pixels(), ++line_idx) {

            if (line_idx == nr_lines)
                break;

            current_line = m_txt_view->line_num_by_index(line_idx);

            uint64_t current_offset;
//            bool have_offset = m_txt_view->line_start_offset_by_index(line_idx, &current_offset);
            bool have_offset = m_txt_view->line_first_cp_index(line_idx, &current_offset); // cp_count -1

            if (!have_offset)
                continue;

            uint64_t last_offset;
            bool have_last_offset = m_txt_view->line_last_offset_by_index(line_idx, &last_offset);
            if (!have_last_offset)
                continue;


            if ((current_line > 0) && prev_line != current_line) {
                buffer_len = snprintf(buffer, sizeof(buffer), "%ld", (long int)current_line);
                //have_real_line = true;
                assert(0);
            } else {
                buffer[0] = 0; // do not print wrapped line
            }

            if (have_real_line == false) {

#if 0 // temporary
                if (line_idx && (current_offset < prev_offset))
                    break;

                if (line_idx && (prev_offset == current_offset))
                    continue;
#endif
                if (current_line == 0) {
                    buffer_len = snprintf(buffer, sizeof(buffer), "@%ld",
                                          (unsigned long) current_offset
                                          /*(unsigned long) last_offset*/);
                }
            }


            y = m_txt_view->line_top_y_pos_by_index(line_idx);

            int32_t x = 0;
            // align text to right
            uint32_t text_width = 0;
            text_width = ft_compute_ascii_text_width(*m_ft, buffer, buffer_len);

            int32_t last_char_size = 0;
            if (buffer_len) {
                auto last_char = buffer[buffer_len - 1];
                last_char_size = m_ft->character_glyph_hori_advance(last_char);
            }

            x = width() - text_width - (last_char_size + 1);
            ew::graphics::fonts::font_print(*m_ft,
                                            x, y, width(), height(),
                                            col,
                                            buffer);

            //prev_offset = current_offset;
            prev_line = current_line;

        }

        return true;
    }

    virtual bool on_mouse_button_press(const ew::graphics::gui::events::button_event * ev)
    {

        app_log << __PRETTY_FUNCTION__ << "\n";
        return false;
    }

    virtual bool on_mouse_button_release(const ew::graphics::gui::events::button_event * ev)
    {
        app_log << __PRETTY_FUNCTION__ << "\n";
        return false;
    }

    virtual bool on_key_press(const  ew::graphics::gui::events::keyboard_event * ev)
    {
        // forward to view
//        ew::graphics::gui::events::keyboard_event new_ev = *ev;
        // new_ev.x -= m_txt_view->x();
        // return m_txt_view->on_key_press(&new_ev);
        return false;
    }

    virtual bool on_key_release(const ew::graphics::gui::events:: keyboard_event * ev)
    {
        // forward to view
        //      ew::graphics::gui::events::keyboard_event new_ev = *ev;
        //new_ev.x -= m_txt_view->x();
        //return m_txt_view->on_key_release(&new_ev);
        return false;
    }

    virtual bool on_mouse_wheel_up(const  ew::graphics::gui::events::button_event * ev)
    {
        //return m_txt_view->on_mouse_wheel_up(ev);
        return false;
    }

    virtual bool on_mouse_wheel_down(const  ew::graphics::gui::events::button_event * ev)
    {
        //return m_txt_view->on_mouse_wheel_down(ev);
        return false;
    }



};

} // ! namespace eedit
