

#include "mlogc-ng.h"
#include "configuration.h"

#ifndef __PERSISTENCE_H__
#define __PERSISTENCE_H__

int persistence_init (struct configuration_t *config);
int persistence_process (struct audit_log_entry_t *audit_log);

#endif
