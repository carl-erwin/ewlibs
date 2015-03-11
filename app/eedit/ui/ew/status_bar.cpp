#include "status_bar.hpp"

#include <assert.h>
#include <ew/graphics/gui/event/push.hpp>

///
// TODO: #include "<ew/graphics/opengl.hpp>"
#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"

//using namespace ew::core::threading;
//using namespace ew::filesystem;
using namespace ew::implementation::graphics::rendering::opengl;


namespace eedit
{

status_bar::status_bar(ew::graphics::gui::widget * parent)
{
	set_parent(parent);
	set_name("status bar");
}

status_bar::~status_bar()
{

}

bool status_bar::set_content(const s32 * str, size_t nr_char)
{
	if (str == nullptr)
		return false;

	if (nr_char == 0) {
		return false;
	}

	if (m_content != nullptr)
		delete [] m_content;
	m_content = new s32 [nr_char];
	std::copy(str, str + nr_char, m_content);

	m_content_sz = nr_char;

	return true;
}


bool status_bar::render()
{
	// status bar delim
	u8 r = 0x2D;
	u8 g = 0x2D;
	u8 b = 0x2D;
	u8 a = 0xFF;
	ew_glColor4ub(r, g, b, a);
	glBegin(GL_LINES);
	{
		//up
		glVertex2i(0          , 0);    // left
		glVertex2i(0 + width(), 0);    // right

		//down
		glVertex2i(0 + width(), height());    // right
		glVertex2i(0          , height());    // left
	}
	glEnd();

	if (m_content_sz == 0) {
		return true;
	}

	ew::graphics::colors::color3ub col(0, 24, 0);

	if (m_content_sz == 0)
		return true;

	auto ft = get_font();
	assert(ft);
	ew::graphics::fonts::font_print(*ft,
					0, 0, width(), height(),
					col,
					m_content, m_content_sz);

	return true;
}

}
