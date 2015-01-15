
#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

#ifndef __JSON_CONVERSION_H__
#define __JSON_CONVERSION_H__

struct audit_log_entry_t
{
    char *id;
    char *date;
    char *origin;
    char *origin_port;
    char *dest;
    char *dest_port;

    char *section_a;
    char *section_b;
    char *section_f;
    char *section_e;
    char *section_h;

    char *mark;
    char *buf;
    int buf_len;

};

#endif
