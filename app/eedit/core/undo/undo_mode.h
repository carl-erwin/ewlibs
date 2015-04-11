#pragma once

namespace eedit
{

namespace core
{

struct event;

bool buffer_undo(struct editor_message_s * msg);
bool buffer_redo(struct editor_message_s * msg);

}

}
