
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

#ifdef VERBOSE
int verbose = 1;
#else
int verbose = 0;
#endif

#ifdef DEBUG
int debug = 1;
#else
int debug = 0;
#endif 

void help (void) 
{
    p("Use: ./mgloc-ng /path/to/config/file\n\n");
}

int main (int argc, char **argv)
{
    int res = 0;
    char *config_file = NULL;
    struct pipeline_t pipeline;
    struct pipeline_element_t *input;

    if (argc < 2)
    {
        help();
        goto missing_arguments;
    }

    config_file = argv[1];
    res = read_configuration_file(config_file, &pipeline);
    if (res)
    {
        e("Problems reading the configuration " \
                "file: %s\n", config_file);
        goto bad_configuration;
    }

    input = pipeline.elements;
    if (input == NULL)
    {
        e("Missing pipeline.\n");
        goto bad_configuration;
    }

    p("Starting pipeline, input element: '%s'\n", input->name); 

    input->process(input, NULL);

bad_configuration:
missing_arguments:
    return res;
}

