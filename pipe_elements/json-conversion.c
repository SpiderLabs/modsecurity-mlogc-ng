


int dump_audit_log(struct audit_log_entry_t *a)
{
    yajl_gen g;
    const unsigned char * buf;
    size_t len;

    g = yajl_gen_alloc(NULL);
    yajl_gen_config(g, yajl_gen_beautify, 1);

    yajl_gen_array_open(g);

    yajl_gen_map_open(g);

    yajl_gen_string(g, "id", strlen("id"));
    yajl_gen_string(g, a->id, strlen(a->id));

    yajl_gen_string(g, "date", strlen("date"));
    yajl_gen_string(g, a->date, strlen(a->date));

    yajl_gen_string(g, "origin", strlen("origin"));
    yajl_gen_map_open(g);
    yajl_gen_string(g, "ip", strlen("ip"));
    yajl_gen_string(g, a->origin, strlen(a->origin));
    yajl_gen_string(g, "port", strlen("port"));
    yajl_gen_number(g, a->origin_port, strlen(a->origin_port));
    yajl_gen_map_close(g);

    yajl_gen_string(g, "dest", strlen("dest"));
    yajl_gen_map_open(g);
    yajl_gen_string(g, "ip", strlen("ip"));
    yajl_gen_string(g, a->dest, strlen(a->dest));
    yajl_gen_string(g, "port", strlen("port"));
    yajl_gen_number(g, a->dest_port, strlen(a->dest_port));
    yajl_gen_map_close(g);

    if (a->section_a != NULL)
    {
        yajl_gen_string(g, "Section A", strlen("Section A"));
        yajl_gen_string(g, a->section_a, strlen(a->section_a));
    }

    if (a->section_b != NULL)
    {
        yajl_gen_string(g, "Section B", strlen("Section B"));
        yajl_gen_string(g, a->section_b, strlen(a->section_b));
    }

    if (a->section_f != NULL)
    {
        yajl_gen_string(g, "Section F", strlen("Section F"));
        yajl_gen_string(g, a->section_f, strlen(a->section_f));
    }

    if (a->section_e != NULL)
    {
        yajl_gen_string(g, "Section E", strlen("Section E"));
        yajl_gen_string(g, a->section_e, strlen(a->section_e));
    }

    if (a->section_h != NULL)
    {
        yajl_gen_string(g, "Section H", strlen("Section H"));
        yajl_gen_string(g, a->section_h, strlen(a->section_h));
    }

    yajl_gen_map_close(g);

    yajl_gen_array_close(g);

    yajl_gen_get_buf(g, &buf, &len);
    fwrite(buf, 1, len, stdout);

    yajl_gen_free(g);

#if 0
    fprintf(stdout, "Section A: %s\n", a->section_a);
    fprintf(stdout, "Section B: %s\n", a->section_b);
    fprintf(stdout, "Section F: %s\n", a->section_f);
    fprintf(stdout, "Section E: %s\n", a->section_e);
    fprintf(stdout, "Section H: %s\n", a->section_h);
#endif

}


char *load_section(const char section, struct audit_log_entry_t *a)
{
    int count = 0;
    int start = 0;
    int end = 0;
    char *section_buf = NULL;

    char *marking = NULL;
    
    marking = malloc(sizeof(char) * (strlen(a->mark) + 1 + 4));
    memset(marking, '\0', strlen(a->mark) + 1 + 4);
    snprintf(marking, strlen(a->mark) + 4, "%s%c--", a->mark, section);
    
    // Find the start point.
    while (a->buf_len > count + strlen(marking) &&
            strncmp(a->buf + count, marking, strlen(marking)) != 0)
    {
        count++;
    }

    if (!(strlen(a->buf + count) > strlen(marking)))
    {
        return NULL;
    }
 
    start = count + strlen(marking) + 1;

    // Find the end of the section
    count = start;
    while (a->buf_len > count + strlen(a->mark) &&
            strncmp(a->buf + count, a->mark, strlen(a->mark)) != 0)
    {
        count++;
    }

    if (!(a->buf_len > count + strlen(a->mark)))
    {
        return NULL;
    }

    end = count;

    section_buf = malloc(sizeof(char) * (end - start + 1));
    memset(section_buf, '\0', end - start + 1);
    strncpy(section_buf, a->buf + start, end - start);

    //fprintf(stdout, "* wheee: section: %c start: %d -- end: %d \n", section, start, end);
    return section_buf;
}

int parse_section_a(unsigned char *buf, struct audit_log_entry_t *a)
{
    // [20/Jun/2014:08:15:11 --0500] U6Qz3cCo8AoAAD@WnCEAAAAV 37.58.100.181 51557 192.168.240.10 80
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

}

int load_buffer(unsigned char *buf, struct audit_log_entry_t *audit_log)
{
    int count = 0;
    int buf_len = strlen(buf);
    char *mark = NULL;

    audit_log->section_a = NULL;
    audit_log->section_b = NULL;
    audit_log->section_f = NULL;
    audit_log->section_e = NULL;
    audit_log->section_h = NULL;

    /* finding the separator */
    while (buf_len > count && buf[count] != '\n')
    {
        count++;
    }

    if (buf[count] != '\n')
    {
        fprintf(stderr, "Not an valid audit log entry");
        return -1;
    }

    mark = malloc(sizeof(char) * (count - 3 + 1));
    memset(mark, '\0', count - 3 + 1);
    mark = strncpy(mark, buf, count - 3);

    audit_log->mark = mark;
    audit_log->buf = buf;
    audit_log->buf_len = buf_len;

    audit_log->section_a = load_section('A', audit_log);
    audit_log->section_b = load_section('B', audit_log);
    audit_log->section_f = load_section('F', audit_log);
    audit_log->section_e = load_section('E', audit_log);
    audit_log->section_h = load_section('H', audit_log);

    parse_section_a(audit_log->section_a, audit_log);

    return 0;
}

int clean_audit_log_entry(struct audit_log_entry_t *audit_log)
{
    free(audit_log->mark);

    if (audit_log->section_a)
        free(audit_log->section_a);
    if (audit_log->section_a)
        free(audit_log->section_b);
    if (audit_log->section_a)
        free(audit_log->section_f);
    if (audit_log->section_a)
        free(audit_log->section_e);
    if (audit_log->section_a)
        free(audit_log->section_h);
}


