
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "mlogc-ng.h"
#include "configuration.h"
#include "persistence.h"
#include "pipeline.h"

inline int persistence_file_exists(const char *file)
{
    struct stat buffer;
    return (stat (file, &buffer) == 0);
}

int persistence_process (struct pipeline_element_t *e,
        struct audit_log_entry_t *audit_log)
{
    int res = 0;
    char *id = audit_log->id;
    char paths[6];
    struct persistence_config_t *conf = e->payload;

    size_t file_size = strlen(id) + strlen(conf->directory) + 6 + 1;
    char *file = malloc(file_size);
    snprintf(file, file_size,
            "%s/%.4s/%.4s/%.4s/%.4s/%.4s/%.4s", conf->directory,
            id, id + 4, id + 8, id + 12, id + 16, id + 20);

    res = persistence_file_exists(file);

    if (!res || 1 == 1)
    {
        int i = 0;
        snprintf(file, file_size, "%s", conf->directory);

        for (i = 0; i < 5; i++)
        {
            snprintf(file + strlen(file), file_size - strlen(file), "/%.4s",
                    id + (i*4));
            mkdir(file, 0777);
        }
        snprintf(file + strlen(file), file_size - strlen(file), "/%.4s",
                id + 20);
        FILE *fd = fopen(file, "w");
        fprintf(fd, ".");
        fclose(fd);

        goto end;
    }

end:
    free (file);
    return res;
}

struct persistence_config_t *create_persistence_instance(const yajl_val *val,
        struct pipeline_t *p)
{
    const char *n_path[] = {"directory",(const char *)0};
    struct persistence_config_t *inst = malloc(sizeof(
                struct persistence_config_t));

    yajl_val n = yajl_tree_get(*val, n_path, yajl_t_string);

    inst->directory = YAJL_GET_STRING(n);

    return inst;
}

