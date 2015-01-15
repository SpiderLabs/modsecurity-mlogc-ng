


#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

struct configuration_t {
    /* Operation type could be pipelined or batch */
    int batch_operation;
    int pipe_line_operation;
    /* Path for batch operation */
    char *batch_path;

    int pipeline_amount;
    char *pipeline[];
};


int read_configuration_file(char *, struct configuration_t **);
int clean_configuration(struct configuration_t **);

#endif
