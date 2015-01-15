

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mlogc-ng.h"
#include "configuration.h"
#include "input_batch.h"
#include "pipeline.h"

/*
 * mlogc-ng
 *
 * TODO: write something about it.
 *
 */


// FIXME: getopt long should handle this in a near future.
#define PATH "logs-tmp"
// FIXME: Check if that value makes sense. Use bytes here.
#define MAX_FILE_SIZE 1024*1024
 

int verbose = 0;
int debug = 0;

void help (void) 
{
    p("Use: ./mgloc-ng /path/to/config/file\n\n");
}

int main (int argc, char **argv)
{
    int res = 0;
    struct configuration_t *config = NULL;
    char *config_file = NULL;

    if (argc < 2)
    {
        help();
        goto missing_arguments;
    }

    res = read_configuration_file(config_file, &config);
    if (res)
    {
        p("Problems reading the configuration " \
                "file: %s\n", config_file);
        goto bad_configuration;
    }

    /* pipe line chain should be created here. */
    res = pipeline_build(config);
    if (res)
    {
        goto bad_pipeline;
    }

    if (config->batch_operation)
    {
        v("Starting batch operation enumeration...\n");
        res = start_batch_operation(config);
    }
    else
    {
        v("Starting pipeline operation...\n");
        //res = start_pipeline_operation(config);
    }

bad_pipeline:
bad_configuration:
    clean_configuration(&config);
missing_arguments:
    return res;
}
