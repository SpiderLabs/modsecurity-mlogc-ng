
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#ifndef __MLOGC_NG_H__
#define __MLOGC_NG_H__

struct audit_log_entry_t
{
    /* buf len */
    char *buf;
    size_t buf_len;

    char *id;
    char *date;
    char *origin;
    char *origin_port;
    char *dest;
    char *dest_port;

    /* Sections */
    char *mark;
    char *section_a;
    char *section_b;
    char *section_f;
    char *section_e;
    char *section_h;

    /* summary line */
    char *summary_line;
    char *hostname;
    char *remote_addr;
    char *remote_user;
    char *local_user;
    char *logtime;
    char *request;
    char *response_status;
    char *bytes_sent;
    char *refer;
    char *user_agent;
    char *uniqueid;
    char *sessionid;
    char *audit_file;
    char *extra;
    char *audit_size;
    char *md5;

    /* internal */
    size_t uploaded;
};

/* Verbose, Debug and Print definitions */
extern int verbose;
extern int debug;

/* FIXME: Check if that value makes sense. Use bytes here. */
#define MAX_FILE_SIZE 1024*1024


#ifndef WIN32

#define d(fmt, ARGS...) do { if (debug) printf("%s:%d:%s(): " fmt, __FILE__, \
		        __LINE__, __func__, ## ARGS); } while (0)

#define v(fmt, ARGS...) do { if (verbose) printf("%s:%d:%s(): " fmt, __FILE__, \
		        __LINE__, __func__, ## ARGS); } while (0)

#define e(fmt, ARGS...) do { fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
		        __LINE__, __func__, ## ARGS); } while (0)


#define p(fmt, ARGS...) do { printf(fmt, ## ARGS); } while (0)

#else

#define p  printf
#define e  printf
#define v  if (verbose) printf
#define d  if (debug) printf

#endif

#endif
