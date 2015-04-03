#pragma once

namespace eedit
{

namespace core
{

struct event;

bool buffer_undo(struct editor_event_s * msg);
bool buffer_redo(struct editor_event_s * msg);

}

}
