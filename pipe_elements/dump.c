
#include "mlogc-ng.h"
#include "configuration.h"
#include "dump.h"

int dump_process (struct pipeline_element_t *e,
        struct audit_log_entry_t *audit_log)
{
    struct dump_config_t *conf = e->payload;

    if (conf->id == 1)
    {
        p("ID: %s\n", audit_log->id);
    }

    if (conf->date == 1)
    {
        p("DATE: %s\n", audit_log->date);
    }

    if (conf->dest == 1)
    {
        p("DEST: %s\n", audit_log->dest);
    }

    return 0;
}

struct dump_config_t *create_dump_instance(const yajl_val *val,
        struct pipeline_t *p)
{
    const char *n_path_id[] = {"id",(const char *)0};
    const char *n_path_date[] = {"date",(const char *)0};
    const char *n_path_dest[] = {"dest",(const char *)0};
    char *id, *date, *dest;

    struct dump_config_t *inst = malloc(sizeof(struct dump_config_t));

    inst->id = 0;
    inst->date = 0;
    inst->dest = 0;

    yajl_val id_ = yajl_tree_get(*val, n_path_id, yajl_t_string);
    id = YAJL_GET_STRING(id_);
    yajl_val date_ = yajl_tree_get(*val, n_path_date, yajl_t_string);
    date = YAJL_GET_STRING(date_);
    yajl_val dest_ = yajl_tree_get(*val, n_path_dest, yajl_t_string);
    dest = YAJL_GET_STRING(dest_);
    if (id && strcmp("true", id) == 0)
    {
        inst->id = 1;
    }
    if (date && strcmp("true", date) == 0)
    {
        inst->date = 1;
    }
    if (dest && strcmp("true", dest) == 0)
    {
        inst->dest = 1;
    }

    return inst;
}

