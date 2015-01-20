
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "yajl/yajl_tree.h"

#include "mlogc-ng.h"
#include "configuration.h"

#include "read_from_filesystem.h"


int read_from_filesystem_call_entry_cb(struct audit_log_entry_t *audit_log,
        struct read_from_filesystem_config_t *conf)
{
    pipeline_process(conf->pipeline, audit_log);
    return 0;
}


struct read_from_filesystem_config_t *create_read_from_filesystem_instance(
        const yajl_val *val, struct pipeline_t *p)
{
    const char *n_path[] = {"directory",(const char *)0};
    struct read_from_filesystem_config_t *inst = malloc(sizeof(
                struct read_from_filesystem_config_t));

    yajl_val n = yajl_tree_get(*val, n_path, yajl_t_string);

    inst->directory = YAJL_GET_STRING(n);
    inst->pipeline = p;

    d("Read file system element was created!\n");
    d("Reading data from: %s\n", inst->directory);

    return inst;
}


int read_from_filesystem_process(struct pipeline_element_t *e,
        struct audit_log_entry_t *audit_log)
{
    int res = 0;
    struct read_from_filesystem_config_t *conf = e->payload;

    d("Starting batch operation at the directory: %s\n", conf->directory);

    res = open_directory_recursive(conf->directory,
            read_from_filesystem_call_entry_cb, conf);

    if (res)
    {
        res = -1;
        e("Failed to open the directory: '%s' or one of its sub-directories\n",
                conf->directory);
        goto failed_opening_directory;
    }

failed_opening_directory:
    return 0;
}


