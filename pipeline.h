
#include "mlogc-ng.h"
#include "configuration.h"

#ifndef __PIPELINE_H__
#define __PIPELINE_H__

struct pipe_element_t {
    char *name;
    void *init;
    int (*process)(struct audit_log_entry_t *audit_log);

    struct pipe_element_t *next;
    struct pipe_element_t *prev;
};

struct pipeline_t {
    struct configuration_t *config;
    struct pipe_element_t *elements;
};

int start_pipeline_opration(struct configuration_t *);

int pipeline_process(struct audit_log_entry_t *t);


#endif
