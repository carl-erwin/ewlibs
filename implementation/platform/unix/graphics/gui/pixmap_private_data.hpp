#pragma once

// X11 specific
#include <X11/X.h>
#include <X11/Xlib.h>

// Gl
#include <GL/gl.h>
#include <GL/glu.h>

// gl ext ?

// GlX
#include <GL/glx.h>

#include <ew/graphics/gui/widget/pixmap/pixmap.hpp>

// include  renderer internal here
namespace ew
{
namespace graphics
{
namespace gui
{

class pixmap::private_data
{
private:
	private_data(const private_data &);
	private_data & operator=(const private_data &);

public:
	private_data() {}
	ew::graphics::gui::window * _owner;

	::Pixmap _x11_pixmap;
	::GLXPixmap _glx_pixmap;
	bool was_init;
};

}
}
}
