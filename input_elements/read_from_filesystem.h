
#include "yajl/yajl_tree.h"

#include "mlogc-ng.h"
#include "configuration.h"

#ifndef __READ_FROM_FILESYSTEM_H__
#define __READ_FROM_FILESYSTEM_H__

struct read_from_filesystem_config_t
{
    const char *directory;
    struct pipeline_t *pipeline;
};

struct read_from_filesystem_config_t *create_read_from_filesystem_instance(
		const yajl_val *, struct pipeline_t *);

int read_from_filesystem_process(struct pipeline_element_t *e,
		struct audit_log_entry_t *);

#endif
