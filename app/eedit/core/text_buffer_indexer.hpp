#pragma once

#include <ew/core/types/types.hpp>
using namespace ew::core::types;

#include <ew/core/object/buffer.hpp>
using namespace ew::core::objects;


#include <ew/filesystem/file.hpp>
using namespace ew::filesystem;

namespace  eedit
{

class text_buffer;



class text_buffer_indexer
{
public:
	static bool build_index(text_buffer * text_buffer);
	static bool on_node_event(buffer::node * n, buffer::node_event ev, struct index_context * ctx);

};


} // ! namespace  eedit
