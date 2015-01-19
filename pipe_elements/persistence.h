

#include "mlogc-ng.h"
#include "configuration.h"

#ifndef __PERSISTENCE_H__
#define __PERSISTENCE_H__

struct persistence_config_t {
    char *directory;
};

struct persistence_config_t *create_persistence_instance(const yajl_val *,
		struct pipeline_t *);

int persistence_init (struct configuration_t *config);
int persistence_process (struct pipeline_element_t *,
		struct audit_log_entry_t *);

#endif
