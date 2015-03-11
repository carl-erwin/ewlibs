#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "ew/ew_config.hpp"

#include "core/log.hpp"

#include "../include/codec.h"


codec_id_t          next_id = 1;

struct codec_id_info_s {
	codec_id_t          id;
	std::string         name;
	editor_codec_type_e type;
	void *              ops;
};


inline std::ostream & operator<<(std::ostream & stream, const codec_id_info_s & info)
{
	app_log << "info{id = " << info.id  << ", ";
	app_log << "name = '" << info.name << "', ";
	app_log << "type = " << info.type << ", ";
	app_log << "ops = " << info.ops << "}";
	return stream;
}



// codec registration
// when a codec is initialized it must call

std::map<std::string, codec_id_info_s> codec_by_name_map;
std::map<codec_id_t,  codec_id_info_s> codec_by_id_map;



extern "C"
SHOW_SYMBOL codec_id_t codec_register(const char * name, enum editor_codec_type_e type, void * ops)
{
	// invalid check type ? return 0

	// name collision ? return 0;
	codec_id_t id = codec_get_by_name(name);
	if (id) {
		return 0;
	}


	id = next_id++;
	codec_id_info_s info {id,name,type,ops};

	app_log << __PRETTY_FUNCTION__ << " " << info << "\n";

	// if codec_map.find(ops) -> error
	codec_by_name_map[std::string(name)] = info;
	codec_by_id_map[id] = info;

	return id;
}

extern "C"
SHOW_SYMBOL codec_id_t codec_get_by_name(const char * name)
{

	auto it = codec_by_name_map.find(std::string(name));
	if (it == codec_by_name_map.end()) {
		//not found
		return 0;
	}

	auto & info = it->second;
	app_log << __PRETTY_FUNCTION__ << " " << info << "\n";

	return info.id;
}


extern "C"
SHOW_SYMBOL enum editor_codec_type_e codec_get_type(codec_id_t id)
{
	auto it = codec_by_id_map.find(id);
	if (it == codec_by_id_map.end())
		return EDITOR_INVALID_CODEC;

	return it->second.type;
}

extern "C"
SHOW_SYMBOL void * codec_get_ops(codec_id_t id)
{
	auto it = codec_by_id_map.find(id);
	if (it == codec_by_id_map.end())
		return nullptr;

	return it->second.ops;
}


// the codec MUST provides this functions
extern "C"
SHOW_SYMBOL int codec_context_create(codec_id_t id,  codec_context_id_t ** ctx)
{
	abort();
	return 0;
}


extern "C"
SHOW_SYMBOL int codec_context_destroy(codec_id_t id, codec_context_id_t * ctx)
{
	abort();
	return 0;
}



extern "C"
SHOW_SYMBOL int codec_get_name(codec_id_t id,  char buffer[], size_t buffer_size)
{
	abort();
	return 0;
}
