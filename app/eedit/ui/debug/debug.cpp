#include "ew/core/time/time.hpp"

#include "ui.h"


#include "../../core/core.hpp"

namespace eedit
{

///////////////////////////////////////////////////////////////////////////////////////////////////

class application;



class debug_display : public ew::graphics::gui::display
{
public:

};


struct debug_ui_interface : public user_interface {
	virtual ~debug_ui_interface();
	virtual bool setup(application * app);
	virtual bool main_loop();
	virtual ew::graphics::gui::display * get_display();

	debug_display * dpy = nullptr;
};



debug_ui_interface::~debug_ui_interface()
{
	delete dpy;
}

bool debug_ui_interface::setup(application * app)
{
	return true;
}

bool debug_ui_interface::main_loop()
{
	auto msg       =  new eedit::core::application_event(EDITOR_QUIT_APPLICATION_DEFAULT);
	msg->src.kind  =  EDITOR_ACTOR_UI;
	msg->src.queue =  nullptr; // event_queue();  //  TODO: ctx ?
	msg->dst.kind  =  EDITOR_ACTOR_CORE;
	msg->view_id =  0;

	app_log << " send quit app event : ui -> core @" << ew::core::time::get_ticks() << "\n";
	eedit::core::push_event(msg);

	return true;
}

ew::graphics::gui::display * debug_ui_interface::get_display()
{
	return static_cast<ew::graphics::gui::display *>(dpy);
}


eedit::user_interface * new_debug_ui()
{
	return new debug_ui_interface();
}


}
