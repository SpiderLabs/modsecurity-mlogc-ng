

#include "mlogc-ng.h"
#include "configuration.h"

#ifndef __DUMP_H__
#define __DUMP_H__

int dump_init (struct configuration_t *config);
int dump_process (struct audit_log_entry_t *audit_log);

#endif
