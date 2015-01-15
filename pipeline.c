
#include "pipeline.h"
#include "input_batch.h"

#include "dump.h"
#include "persistence.h"

struct pipeline_t pipeline;

int pipeline_build(struct configuration_t *config)
{
    char *ar[] = {"persistence", "dump", NULL};
    struct pipe_element_t *prev = NULL;

    pipeline.elements = NULL;

    v("Building pipeline with the following elements: \n");
    char **i = ar;
    while (*i) {
        v("\t-%s\n", *i);
        struct pipe_element_t *e = malloc(sizeof(struct pipe_element_t));
        e->name = *i;
 
        if (strncmp("dump", *i, strlen(*i)) == 0)
        {
            e->init = dump_init;
            e->process = dump_process;
        }
        else if (strncmp("persistence", *i, strlen(*i)) == 0)
        {
            e->init = persistence_init;
            e->process = persistence_process;
        }
        else
        {
            e("error building the pipeline\n");
            return -1;
        }
 
        e->prev = prev;
        e->next = NULL;

        if (e->prev)
        {
            e->prev->next = e;
        }

        if (!pipeline.elements)
        {
            pipeline.elements = e;
        }

        prev = e;

        i++;
    }

    pipeline.config = config;
    return 0;
}

int pipeline_process(struct audit_log_entry_t *t)
{
    struct pipe_element_t *e = NULL;

    v("Sending %s over our pipeline\n", t->id);
    
    e = pipeline.elements;

    while (e)
    {
        int res = 0;
        v("Pipe: %s\n", e->name);
        res = (int)(*(e->process))(t);
        if (res)
        {
            break;
        }
        e = e->next;
    }

    return 0;
}

