#pragma once


#include <ew/video/ivideo.hpp>
#include <ew/audio/iaudio.hpp>
#include <ew/user/iuser_input.hpp>

#include <ew/graphics/gui/gui.hpp>
#include <ew/graphics/gui/display/display.hpp>


namespace ew
{
namespace core
{
namespace application
{

using namespace ew::core::types;

class Simple_Application
{
private:
	Simple_Application(const Simple_Application &);
	Simple_Application & operator=(const Simple_Application &);

public:
	Simple_Application();

	virtual ~Simple_Application();

	virtual const char * getName() const;

	virtual bool init(u32 width, u32 height, u32 bpp, bool resize, bool fullscreen);

	virtual void run(void);
	virtual s32 quit(void);

	/* sub initializer here ?? */
protected:
	//    virtual bool setupGui( void );
	virtual bool setupUserInputs(void);
	virtual bool setupVideo(void);
	// virtual  bool  setupAudio( void );

protected:
	u32 _width;
	u32 _height;
	u32 _bpp;
	bool _resize;
	bool _fullscreen;

	// IGui
	ew::graphics::gui::gui * _gui;

	// IDisplay
	ew::graphics::gui::display * _display;

	// IUserInputInterface
	ew::user::user_input_interface * _iinput;

	u32 nb_joysticks;
	ew::user::devices::keyboard  * _ikeyboard;
	ew::user::devices::mouse   *   _imouse;
	ew::user::devices::joystick ** _ijoystick;

	// IVideoInterface
	ew::video::video_interface * _ivideo;
	ew::video::devices::graphic_card * _igraphiccard;

	bool loop;
};

}
}
} // ! ew::core::Application
