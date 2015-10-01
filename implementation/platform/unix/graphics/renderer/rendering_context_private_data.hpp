#pragma once


// X11 specific
#include <X11/X.h>
#include <X11/Xlib.h>

// GlX
#include <GL/glx.h>


// Ew
#include <mutex>

#include <ew/graphics/gui/widget/window/window.hpp>

namespace ew
{
namespace graphics
{
namespace rendering
{

class rendering_context::private_data : public std::mutex   // add lock ?
{

    friend class ew::graphics::gui::window;

public:
    private_data(const private_data &) = delete;
    private_data & operator = (const private_data &) = delete;

    private_data()
        :
        //ew::core::threading::mutex(RECURSIVE_MUTEX),
        window(0),
        pixmap(0),
        _glxCtx(0),
        _haveDoubleBuffer(false),
        _GlxCtxLocked(false),
        _glxDrawable(0),
        _th_owner(0),
        _nrLock(0) { }

    ew::graphics::gui::window * window; // attacched
    ew::graphics::gui::pixmap * pixmap; // mus be hidden ?
    // or pbuffer

    ::GLXContext _glxCtx;

    bool _haveDoubleBuffer: 1;
    bool _GlxCtxLocked: 1;

    ::GLXDrawable _glxDrawable; // the current ly bound drawable

    std::thread::id _th_owner;
    ew::core::types::u32 _nrLock;
};

}
}
}
