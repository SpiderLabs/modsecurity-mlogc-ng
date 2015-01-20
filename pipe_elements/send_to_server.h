

#include "mlogc-ng.h"
#include "configuration.h"

#ifndef __SEND_TO_SERVER_H__
#define __SEND_TO_SERVER_H__

struct send_to_server_config_t {
    char *host;
    char *username;
    char *password;
    char *sensor_id;
    int validate_certificate_chain;
    int number_of_clients;
};

int send_to_server_process (struct pipeline_element_t *,
		struct audit_log_entry_t *);

struct send_to_server_config_t *create_send_to_server_instance(const yajl_val *,
		struct pipeline_t *);

#endif
