

#include <stdlib.h>
#include <string.h>

#include "mlogc-ng.h"
#include "configuration.h"

#include "yajl/yajl_tree.h"

#include "pipeline.h"

int read_configuration_file(const char *config_file, struct pipeline_t *p)
{
    int res = 0;
    static unsigned char fileData[65536];
    size_t rd;
    yajl_val node;
    char errbuf[1024];
    FILE *fp;
    const char * path[] = { "mlogc_pipeline", (const char *) 0 };
    size_t len;
    int i;

    fileData[0] = errbuf[0] = 0;

    /* read the entire config file */
    fp = fopen(config_file, "r");
    if (!fp)
    {
        res = -1;
        goto failed_to_open_config_file;
    }

    rd = fread((void *) fileData, 1, sizeof(fileData) - 1, fp);
    fclose(fp);

    /* we have the whole config file in memory.  let's parse it ... */
    node = yajl_tree_parse((const char *) fileData, errbuf, sizeof(errbuf));

    if (node == NULL) {
        e("Config parser error: ");
        if (strlen(errbuf))
        {
            e("%s", errbuf);
        }
        else
        {
            e("unknown error");
        }

        e("\n");
        res = -1;
        goto parser_error;
    }


    yajl_val v = yajl_tree_get( node, path, yajl_t_array );
    if ( !v || !YAJL_IS_ARRAY( v ) ) {
        e("Bad configuration format\n");
        res = -1;
        goto parser_error;
    }

    
    len = v->u.array.len;
    for ( i = 0; i < len; ++i ) {
        yajl_val obj = v->u.array.values[ i ];
        size_t nelem = obj->u.object.len;
        int ii;
        for ( ii = 0; ii < nelem; ++ii ) {
            const char *n_path[] = {"directory",(const char *)0};

            const char *key = obj->u.object.keys[ ii ];
            yajl_val val = obj->u.object.values[ ii ];
            d("Creating pipeline element: %s\n", key);
            struct pipeline_element_t *e = create_pipe_element(key, &val, p);
            if (e == NULL)
            {
                e("Problem creating pipe element: %s\n", key);
                res = -1;
                goto pipe_problem;
            }
            add_pipe_element(p, e);
        }
    }

pipe_problem:
parser_error:
failed_to_open_config_file:
failed_allocate_memory:
    return res;
}

