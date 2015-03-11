#pragma once

namespace eedit
{

namespace core
{

struct event;

bool buffer_undo(event * msg);
bool buffer_redo(event * msg);

}

}
