
#include "mlogc-ng.h"
#include "input_batch.h"
#include "filesystem-walker.h"
#include "pipeline.h"

#if defined(__APPLE__)

#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#define SHA1 CC_SHA1

#else

#include <openssl/md5.h>

#endif


char *md5sum(const char *str, int length) {
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}

int parse_section_a(struct audit_log_entry_t *a)
{
    int start = 0;
    int count = 0;
    char *date = NULL;
    char *id = NULL;
    char *origin = NULL;
    char *dest = NULL;
    char *origin_port = NULL;
    char *dest_port = NULL;
    char *buf = a->section_a;

    /* Date */
    while (strlen(buf) > count && buf[count] != ' ')
    {
        count++;
    }
    date = strndup(buf + 1, count - 1);
    a->logtime = date;


    while (strlen(buf) > count && buf[count] != ']')
        count++;

    /* ID */
    count++;
    count++;
    start = count;
    while (strlen(buf) > count && buf[count] != ' ')
    {
        count++;
    }
    id = strndup(buf + start, count - start);
    a->uniqueid = id;

    /* origin */
    count++;
    start = count;
    while (strlen(buf) > count && buf[count] != ' ')
    {
        count++;
    }

    origin = malloc(sizeof(char) * (count - start + 1));
    memset(origin, '\0', count - start + 1);
    memcpy(origin, buf + start, count - start);
    a->remote_addr = origin;

    /* origin port */
    count++;
    start = count;
    while (strlen(buf) > count && buf[count] != ' ')
    {
        count++;
    }

    origin_port = malloc(sizeof(char) * (count - start + 1));
    memset(origin_port, '\0', count - start + 1);
    memcpy(origin_port, buf + start, count - start);

    /* dest */
    count++;
    start = count;
    while (strlen(buf) > count && buf[count] != ' ')
    {
        count++;
    }

    dest = malloc(sizeof(char) * (count - start + 1));
    memset(dest, '\0', count - start + 1);
    memcpy(dest, buf + start, count - start);

    /* dest port */
    count++;
    start = count;
    while (strlen(buf) > count && buf[count] != ' ')
    {
        count++;
    }

    dest_port = malloc(sizeof(char) * (count - start + 1));
    memset(dest_port, '\0', count - start + 1);
    memcpy(dest_port, buf + start, count - start);


    a->id = id;
    a->date = date;
    a->origin = origin;
    /* a->origin_port = atoi(origin_port); */
    a->origin_port = origin_port;
    a->dest = dest;
    /* a->dest_port = atoi(dest_port); */
    a->dest_port = dest_port;

    return 0;
}

int parse_section_f(struct audit_log_entry_t *audit_log)
{
    char *buf = audit_log->section_f;
    char *line_start = buf;
    char *line_end = NULL;
    int z = 0;
    
    if (buf == NULL)
    {
        return 0;
    }

    line_end = strchr(buf, 10);

    while (line_end != NULL)
    {
        char *line = NULL;
        size_t len = line_end - line_start;

        line = strndup(line_start, len);

        // FIXME: check the sizes.
        z = z + strlen(line) + 1;
        line_start = line_end + 1;
        line_end = strchr(buf + z, 10);

        if (strncmp("HTTP", line, 4) == 0)
        {
            audit_log->response_status = strndup(strchr(line, 32) + 1, 3);
        }
        else if (strncasecmp(line, "Content-Length: ", 16) == 0)
        {
            audit_log->bytes_sent = strdup(line + 16);
        }
    }
    return 0;
}

int parse_section_b(struct audit_log_entry_t *audit_log)
{
    char *buf = audit_log->section_b;
    char *line_start = buf;
    char *line_end = NULL;
    int z = 0;
    
    line_end = strchr(buf, 10);

    while (line_end != NULL)
    {
        char *line = NULL;
        size_t len = line_end - line_start;

        line = strndup(line_start, len);

        // FIXME: check the sizes.
        z = z + strlen(line) + 1;
        line_start = line_end + 1;
        line_end = strchr(buf + z, 10);

        if (strncmp("GET ", line, 4) == 0)
        {
            audit_log->request = strndup(line + 4, strchr(line, 32) - line);
        }
        else if (strncmp("POST ", line, 5) == 0)
        {
            audit_log->request = strndup(line + 5, strchr(line, 32) - line);
        }
        else if (strncmp("HEAD ", line, 5) == 0)
        {
            audit_log->request = strndup(line + 5, strchr(line, 32) - line);
        }
        else if (strncasecmp(line, "refer: ", 7) == 0)
        {
            audit_log->refer = strdup(line + 7);
        }
        else if (strncasecmp(line, "host: ", 6) == 0)
        {
            audit_log->hostname = strdup(line + 6);
        }
        else if (strncasecmp(line, "user-agent: ", 12) == 0)
        {
            audit_log->user_agent = strdup(line + 12);
        }
    }
    return 0;
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

    return section_buf;
}

int load_buffer(char *buf, struct audit_log_entry_t *audit_log)
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
        res = -1;
        goto not_valid_audit_log;
    }

    mark = malloc(sizeof(char) * (count - 3 + 1));
    memset(mark, '\0', count - 3 + 1);
    mark = strncpy(mark, buf, count - 3);

    char number[20];
    memset(number, '\0', 20);
    snprintf(number, 20, "%d", strlen(buf));

    audit_log->mark = mark;
    audit_log->buf = buf;
    audit_log->buf_len = strlen(buf);
    audit_log->audit_size = strdup(number);
    audit_log->md5 = md5sum(buf, audit_log->buf_len);
    audit_log->uploaded = 0;

    audit_log->section_a = load_section('A', audit_log);
    audit_log->section_b = load_section('B', audit_log);
    audit_log->section_f = load_section('F', audit_log);
    audit_log->section_e = load_section('E', audit_log);
    audit_log->section_h = load_section('H', audit_log);

    res = parse_section_a(audit_log);
    res = parse_section_b(audit_log);
    res = parse_section_f(audit_log);

    size_t a = strlen(audit_log->hostname) + 1 +
        strlen(audit_log->remote_addr) + 1 + 
        strlen(audit_log->remote_user) + 1 + 
        strlen(audit_log->local_user) + 1 + 
        strlen(audit_log->logtime) + 1 + 
        strlen(audit_log->request) + 1 + 2 +
        strlen(audit_log->response_status) + 1 + 
        strlen(audit_log->bytes_sent) + 1 + 
        strlen(audit_log->refer) + 1 + 2 +
        strlen(audit_log->user_agent) + 1 + 2 +
        strlen(audit_log->uniqueid) + 1 + 
        strlen(audit_log->sessionid) + 1 + 2 +
        strlen(audit_log->audit_file) + 1 + 
        strlen(audit_log->extra) + 1 + 
        strlen(audit_log->md5) + 1 + 4 +
        strlen(audit_log->audit_size) + 1;

    audit_log->summary_line = malloc(a + 1);
    memset(audit_log->summary_line, '\0', a + 1);

    snprintf(audit_log->summary_line, a, "%s %s %s %s %s \"%s\" %s %s \"%s\" \"%s\" %s \"%s\" %s %s %s md5:%s\n",
        audit_log->hostname,
        audit_log->remote_addr,
        audit_log->remote_user,
        audit_log->local_user,
        audit_log->logtime,
        audit_log->request,
        audit_log->response_status,
        audit_log->bytes_sent,
        audit_log->refer,
        audit_log->user_agent,
        audit_log->uniqueid,
        audit_log->sessionid,
        audit_log->audit_file,
        audit_log->extra,
        audit_log->audit_size,
        audit_log->md5);

not_valid_audit_log:
    return res;
}

