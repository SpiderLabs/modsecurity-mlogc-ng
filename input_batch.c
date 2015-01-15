
#include "mlogc-ng.h"
#include "input_batch.h"
#include "filesystem-walker.h"
#include "pipeline.h"

void *new_entry(struct audit_log_entry_t audit_log)
{
    pipeline_process(&audit_log);
}

int start_batch_operation (struct configuration_t *config)
{
    int res = 0;
    d("Starting batch operation at the directory: %s", config->batch_path);

    res = open_directory_recursive(config->batch_path, new_entry);
    if (res)
    {
        res = -1;
        e("Failed to open the directory: '%s' or one of its sub-directories\n",
                config->batch_path);
        goto failed_opening_directory;
    }

failed_opening_directory:
    return 0;
}


// [20/Jun/2014:08:15:11 --0500] U6Qz3cCo8AoAAD@WnCEAAAAV 37.58.100.181 51557 192.168.240.10 80
//
int parse_section_a(unsigned char *buf, struct audit_log_entry_t *a)
{
    int start = 0;
    int count = 0;
    char *date = NULL;
    char *id = NULL;
    char *origin = NULL;
    char *dest = NULL;
    char *origin_port = NULL;
    char *dest_port = NULL;
    int buf_len = strlen(buf);

    /* Date */
    while (buf_len > count && buf[count] != ']')
    {
        count++;
    }
    
    date = malloc(sizeof(char) * (count - 1 + 1));
    memset(date, '\0', count - 1 + 1);
    memcpy(date, buf + 1, count - 1);

    /* ID */
    count++;
    count++;
    start = count;
    while (buf_len > count && buf[count] != ' ')
    {
        count++;
    }
    
    id = malloc(sizeof(char) * (count - start + 1));
    memset(id, '\0', count - start + 1);
    memcpy(id, buf + start, count - start);

    /* origin */
    count++;
    start = count;
    while (buf_len > count && buf[count] != ' ')
    {
        count++;
    }

    origin = malloc(sizeof(char) * (count - start + 1));
    memset(origin, '\0', count - start + 1);
    memcpy(origin, buf + start, count - start);

    /* origin port */
    count++;
    start = count;
    while (buf_len > count && buf[count] != ' ')
    {
        count++;
    }

    origin_port = malloc(sizeof(char) * (count - start + 1));
    memset(origin_port, '\0', count - start + 1);
    memcpy(origin_port, buf + start, count - start);

    /* dest */
    count++;
    start = count;
    while (buf_len > count && buf[count] != ' ')
    {
        count++;
    }

    dest = malloc(sizeof(char) * (count - start + 1));
    memset(dest, '\0', count - start + 1);
    memcpy(dest, buf + start, count - start);

    /* dest port */
    count++;
    start = count;
    while (buf_len > count && buf[count] != ' ')
    {
        count++;
    }

    dest_port = malloc(sizeof(char) * (count - start + 1));
    memset(dest_port, '\0', count - start + 1);
    memcpy(dest_port, buf + start, count - start);


    a->id = id;
    a->date = date;
    a->origin = origin;
    //a->origin_port = atoi(origin_port);
    a->origin_port = origin_port;
    a->dest = dest;
    //a->dest_port = atoi(dest_port);
    a->dest_port = dest_port;

    return 0;
}


int load_buffer(unsigned char *buf, struct audit_log_entry_t *audit_log)
{
    char *mark = NULL;
    int res = 0;
    int count = 0;
    int buf_len = strlen(buf);
    
    /* finding the separator */
    while (buf_len > count && buf[count] != '\n')
    {
        count++;
    }

    if (buf[count] != '\n')
    {
        e("Not an valid audit log entry");
        goto not_valid_audit_log;
    }

    mark = malloc(sizeof(char) * (count - 3 + 1));
    memset(mark, '\0', count - 3 + 1);
    mark = strncpy(mark, buf, count - 3);

    audit_log->buf = buf;
    res = parse_section_a(buf, audit_log);

not_valid_audit_log:
    return res;
}

