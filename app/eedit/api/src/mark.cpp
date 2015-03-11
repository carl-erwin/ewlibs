#include <assert.h>
#include <stdint.h>
#include <cstring>
#include <algorithm>

#include "../include/mark.h"

#ifdef __cplusplus
extern "C" {
#endif


#define MARK_NAME_MAX_SIZE ((63+1) - sizeof(uint64_t))


struct mark_s {
	uint64_t offset;
	char     name[MARK_NAME_MAX_SIZE];
};

size_t mark_size()
{
	return sizeof (struct mark_s);
}

size_t mark_name_max_size()
{
	return MARK_NAME_MAX_SIZE;
}


void mark_set_name(mark_t m,   const char * name)
{
	int name_size = ::strlen(name);
	int len = std::min<int>(name_size, sizeof (m->name) - 1);
	std::copy(name, name + len, m->name);
	m->name[len] = 0;
}

const char * mark_get_name(mark_t m)
{
	return m->name;
}

void mark_set_offset(mark_t m, const uint64_t offset)
{
	m->offset = offset;
}

uint64_t mark_get_offset(mark_t m)
{
	return m->offset;
}


#ifdef __cplusplus
}
#endif
