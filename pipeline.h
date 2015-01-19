
#include "yajl/yajl_tree.h"

#include "mlogc-ng.h"

#ifndef __PIPELINE_H__
#define __PIPELINE_H__

struct pipeline_element_t {
    char *name;
    void *payload;

    void *init;
    int (*process)(struct pipeline_element_t *, struct audit_log_entry_t *);

    struct pipeline_element_t *next;
    struct pipeline_element_t *prev;
};

struct pipeline_t {
    struct configuration_t *config;
    struct pipeline_element_t *elements;
};

struct pipeline_element_t *create_pipe_element(const char *, const yajl_val *,
		struct pipeline_t *);

#endif
