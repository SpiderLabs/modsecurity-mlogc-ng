
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "mlogc-ng.h"
#include "configuration.h"

#define BASE "/tmp/test"

inline int persistence_file_exists(const char *file)
{
    struct stat buffer;
    return (stat (file, &buffer) == 0);
}

int persistence_init (struct configuration_t *config)
{
    /* dump does not have initialization */
    return 0;
}

int persistence_process (struct audit_log_entry_t *audit_log)
{
    int res = 0;
    char *id = audit_log->id;
    char paths[6];

    size_t file_size = strlen(id) + strlen(BASE) + 6 + 1;
    char *file = malloc(file_size);
    snprintf(file, file_size,
            "%s/%.4s/%.4s/%.4s/%.4s/%.4s/%.4s", BASE,
            id, id + 4, id + 8, id + 12, id + 16, id + 20);

    p("checking if file exist: %s\n", file);
    res = persistence_file_exists(file);

    if (!res || 1 == 1)
    {
        int i = 0;
        snprintf(file, file_size, "%s", BASE);

        p("File base: %s\n", file);

        for (i = 0; i < 5; i++)
        {
            snprintf(file + strlen(file), file_size - strlen(file), "/%.4s", id + (i*4));
            p("File: %s\n",  file);
            mkdir(file, 0777);
        }
        snprintf(file + strlen(file), file_size - strlen(file), "/%.4s", id + 20);
        p("Creating file: %s\n", file);
        FILE *fd = fopen(file, "w");
        fprintf(fd, ".");
        fclose(fd);

        goto end;
    }

end:
    free (file);
    return res;
}
