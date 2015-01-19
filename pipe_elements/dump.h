

#include "mlogc-ng.h"
#include "configuration.h"

#ifndef __DUMP_H__
#define __DUMP_H__

struct dump_config_t {
    int id;
    int date;
    int dest;
};

int dump_process (struct pipeline_element_t *, struct audit_log_entry_t *);
struct dump_config_t *create_dump_instance(const yajl_val *,
		struct pipeline_t *);

#endif
