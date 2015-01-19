
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#ifndef __MLOGC_NG_H__
#define __MLOGC_NG_H__

struct audit_log_entry_t
{
    char *buf;
    char *id;
    char *date;
    char *origin;
    char *origin_port;
    char *dest;
    char *dest_port;
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
