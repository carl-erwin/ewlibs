#pragma once

#include "mywindow.hpp"

#include <thread>

namespace test
{
namespace graphics
{


class myMenu : public window
{
public:
    myMenu(window * parent, window_properties & properties);
    virtual ~myMenu();


    virtual bool on_draw(const widget_event * ev);
    virtual bool on_move(const widget_event * ev);

    virtual bool on_key_press(const keyboard_event * ev);
    virtual bool on_key_release(const keyboard_event * ev);

    virtual bool on_mouse_button_press(const button_event * ev);

    virtual bool on_mouse_button_release(const button_event * ev);

    virtual bool on_close(const widget_event * ev);

    virtual bool show();

    virtual bool hide();

    std::thread * anim_th;
    bool do_anim;

    s32 rel_x;
    s32 rel_y;
};


} // ! namespace graphics
} // ! namespace test
