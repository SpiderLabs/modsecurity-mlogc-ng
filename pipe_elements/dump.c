

#include "mlogc-ng.h"
#include "configuration.h"

int dump_init (struct configuration_t *config)
{
    /* dump does not have initialization */
    return 0;
}

int dump_process (struct audit_log_entry_t *audit_log)
{
    p("DUMP: %s\n", audit_log->id);
    return 0;
}
