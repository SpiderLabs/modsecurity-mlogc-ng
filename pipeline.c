
#include "yajl/yajl_tree.h"

#include "pipeline.h"
#include "input_batch.h"
#include "dump.h"
#include "persistence.h"
#include "read_from_filesystem.h"
#include "send_to_server.h"

struct pipeline_element_t *create_pipe_element(const char *name, const yajl_val *val, struct pipeline_t *p)
{
    struct pipeline_element_t *element;
    /* 
     * We should have a dynamic list here, doing it hard coded
     * to have it running ASAP.
     *
     */

    if (strcmp(name, "read_from_filesystem") == 0)
    {
        v("Reading form file system element is now being created\n");
        element = malloc(sizeof(struct pipeline_element_t));
        element->name = "read_from_filesystem";
        element->payload = (void *)create_read_from_filesystem_instance(val, p);
        element->process = read_from_filesystem_process;
    }
    if (strcmp(name, "dump") == 0)
    {
        v("dump element is now being created\n");
        element = malloc(sizeof(struct pipeline_element_t));
        element->name = "dump";
        element->payload = (void *)create_dump_instance(val, p);
        element->process = dump_process;
    }
    if (strcmp(name, "persistence") == 0)
    {
        v("persistence element is now being created\n");
        element = malloc(sizeof(struct pipeline_element_t));
        element->name = "persistence";
        element->payload = (void *)create_persistence_instance(val, p);
        element->process = persistence_process;
    }
    if (strcmp(name, "send_to_server") == 0)
    {
        v("send_to_server element is now being created\n");
        element = malloc(sizeof(struct pipeline_element_t));
        element->name = "send_to_server";
        element->payload = (void *)create_send_to_server_instance(val, p);
        element->process = send_to_server_process;
    }
    
    return element;
}

int add_pipe_element(struct pipeline_t *p, struct pipeline_element_t *e)
{
    int res = 0;
    struct pipeline_element_t *last = NULL;

    e->prev = NULL;
    e->next = NULL;

    if (p->elements == NULL)
    {
        p->elements = (struct pipeline_element_t *)e;
        goto done;
    }

    last = p->elements;
    while (last->next)
    {
        last = last->next;
    }

    e->prev = last;
    last->next = e;
done:
    return res;
}

int pipeline_process(struct pipeline_t *p, struct audit_log_entry_t *t)
{
    struct pipeline_element_t *e = NULL;

    v("Sending %s over our pipeline\n", t->id);
    
    e = p->elements->next;

    while (e)
    {
        int res = 0;
        v("Pipe: %s\n", e->name);
        res = (int)(*(e->process))(e, t);
        if (res)
        {
            break;
        }
        e = e->next;
    }

    return 0;
}

