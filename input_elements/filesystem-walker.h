
#ifndef __FILESYSTEM_WALKER_H__
#define __FILESYSTEM_WALKER_H__

#include "read_from_filesystem.h"

int inspect_file(unsigned char *, void *);
int open_directory_recursive(const unsigned char *, void *,
	struct read_from_filesystem_config_t *);

#endif
