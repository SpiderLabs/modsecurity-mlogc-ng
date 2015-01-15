

#include <stdlib.h>
#include <string.h>

#include "mlogc-ng.h"
#include "configuration.h"


int read_configuration_file(char *config_file, struct configuration_t **config)
{
    int res = 0;

    *config = malloc(sizeof(struct configuration_t));
    if (*config == NULL)
    {
        res = -1;
        e("Failed to allocate memory to store configuration information\n");
        goto failed_allocate_memory;
    }

    memset(*config, '\0', sizeof(struct configuration_t));


    /* hard coded for test only, configure parser is not ready. */
    {
    (*config)->batch_operation = 1;
    (*config)->batch_path = "logs-for-test/";
    }

failed_allocate_memory:
    return res;
}

int clean_configuration(struct configuration_t **config)
{
    if (*config)
    {
        free(*config);
    }

    return 0;
}
