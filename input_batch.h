
#include "mlogc-ng.h"
#include "configuration.h"

#ifndef __INPUT_BATCH_H__
#define __INPUT_BATCH_H__

int start_batch_opration(struct configuration_t *);


int load_buffer(unsigned char *, struct audit_log_entry_t *);


#endif
