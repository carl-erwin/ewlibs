#pragma once

#include <thread>
#include <mutex>


// X11 specific
#include <X11/X.h>
#include <X11/Xlib.h>

// Gl
#include <GL/gl.h>
#include <GL/glu.h>

// gl ext ?

// GlX
#include <GL/glx.h>


// Ew

#include <ew/maths/functions.hpp>


#include <ew/graphics/gui/widget/widget.hpp>
#include <ew/graphics/gui/widget/widget_properties.hpp>
#include <ew/graphics/gui/widget/window/window.hpp>
#include <ew/graphics/gui/widget/window/window_properties.hpp>


#include "ew/graphics/renderer/rendering_context.hpp"

// include  renderer internal here

namespace ew
{
namespace graphics
{
namespace gui
{

// move elsewhere
class pixel_format_descriptor
{
public:
    s32 bpp = 0;
    s32 depthSize = 0;
    s32 redSize = 0;
    s32 greenSize = 0;
    s32 blueSize = 0;
    s32 alphaSize = 0;
};


class window::window_private_data : public std::mutex
{
private:
    window_private_data(const window_private_data &);
    window_private_data & operator=(const window_private_data &);

public:
    ~window_private_data();
    window_private_data(window * parent);
    bool build_Window(ew::graphics::gui::display * guiDpy, ew::graphics::gui::window * parent, ew::graphics::gui::window * owner, window_properties & properties);

public:
    ew::graphics::gui::window * _parent; // class_name , getWidgetClassID
    ew::graphics::gui::window * _owner; // same family
    bool _isInitialized: 1;
    bool _isAvailable: 1;
    bool _destroyAck: 1;
    bool _inDestroyProcess: 1;
    bool _manual_grab: 1;

    // class X11_Internal
    ::Display * _x11_dpy; // should be available as _guiDpy->d->_x1_dpy;
    ::Display * getX11Display()
    {
        return _x11_dpy;  // remove this ugly
    }

    ::Window _x11_rootWindow; // parent
    ::Window _x11_drawable;
    ::Colormap _x11_colorMap;
    ::XVisualInfo * _x11_visualInfo;

    // input
    ::XIM _x11_InputMethod = 0;
    ::XIC _x11_InputContext = 0;

    rendering_context * _rdr_ctx;
    ew::graphics::gui::pixmap * _offscreen_pixmap = nullptr;
    ew::graphics::gui::pixmap * _background_pixmap = nullptr; // TODO: add Window:setBackgroundPixmap(Pixmap *);

    pixel_format_descriptor pfd;
    int haveDoubleBuffer = 0; // glxSwapBuffers

    window_properties _properties;

    ew::graphics::gui::display * _guiDpy;

    u32 _lastResize_time = 0;
};

}
}
}
