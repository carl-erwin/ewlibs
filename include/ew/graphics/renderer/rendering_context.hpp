#pragma once


#include <ew/ew_config.hpp>

#include <ew/core/types/types.hpp>
#include <ew/core/interface/lock_interface.hpp>

// we must have
// ew/graphics/gui/forward.hpp to avoid duplicates
namespace ew
{
namespace graphics
{
namespace gui
{

class widget;
class window;
class pixmap;

}
}
}

namespace ew
{
namespace graphics
{
namespace rendering
{

/*
 renderer should generated them
 so call rendering::init()
*/

class  EW_GRAPHICS_EXPORT rendering_context : public ew::core::object
{
public:
	rendering_context(ew::graphics::gui::window * window);
	virtual ~rendering_context();

	virtual bool lock();
	virtual bool unlock();
	virtual bool trylock();

	//  virtual bool isAvailable();

	virtual bool attach(ew::graphics::gui::window * window);
	// virtual bool attach(ew::graphics::gui::Pixmap * pixmap);// TODO:: add this ?
	virtual bool detach();
	// return Widget ?
	virtual ew::graphics::gui::window * isAttachedTo();

private:
	class private_data;
	private_data * d;

	friend class ew::graphics::gui::window;
	friend class ew::graphics::gui::pixmap;
};

}
}
}
