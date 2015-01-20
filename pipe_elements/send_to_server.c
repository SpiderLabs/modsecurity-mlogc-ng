
#include <curl/curl.h>

#include "mlogc-ng.h"
#include "configuration.h"
#include "send_to_server.h"



static size_t send_to_server_read_cb (void *ptr, size_t size, size_t nmemb, void *data)
{
    struct audit_log_entry_t *audit = (struct audit_log_entry_t *)data;
    size_t left = strlen(audit->buf) - audit->uploaded;
    size_t max_chunk = size * nmemb;
    size_t retcode = left < max_chunk ? left : max_chunk;

    memcpy(ptr, audit->buf + audit->uploaded, retcode);

    audit->uploaded += retcode;

    return retcode;
}

size_t sent_to_server_write_cb(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    return size*nmemb;
}


int send_to_server_process (struct pipeline_element_t *e,
        struct audit_log_entry_t *audit_log)
{
    struct send_to_server_config_t *conf = e->payload;
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();

    if (curl)
    {
        char *credentials = malloc((strlen(conf->username) +
            strlen(conf->password) + 1 + 1) * sizeof(char));
        char *summary = malloc(strlen("X-ForensicLog-Summary: ") + strlen(audit_log->summary_line) + 1);
        struct curl_slist *headerlist = NULL;


        snprintf(summary, strlen("X-ForensicLog-Summary: ") + strlen(audit_log->summary_line) + 1, 
                "X-ForensicLog-Summary: %s", audit_log->summary_line);

        snprintf(credentials, strlen(conf->username) +
            strlen(conf->password) + 1 + 1, "%s:%s", conf->username,
            conf->password);

        curl_easy_setopt(curl, CURLOPT_URL, conf->host);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_USERPWD, credentials);

        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, send_to_server_read_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, sent_to_server_write_cb);
        curl_easy_setopt(curl, CURLOPT_READDATA, audit_log);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, strlen(audit_log->buf));

        //headerlist = curl_slist_append(headerlist, "Expect:");
        //headerlist = curl_slist_append(headerlist, hash);
        headerlist = curl_slist_append(headerlist, summary);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);


        if (conf->validate_certificate_chain == 0)
        {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }
        res = curl_easy_perform(curl);

        if(res != CURLE_OK)
        {
          e("failed to send: %s\n%s\n", audit_log->audit_file,
                  curl_easy_strerror(res));
        }
     
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}

struct send_to_server_config_t *create_send_to_server_instance(
        const yajl_val *val, struct pipeline_t *p)
{
    const char *n_path_host[] = {"host",(const char *)0};
    const char *n_path_username[] = {"username",(const char *)0};
    const char *n_path_password[] = {"password",(const char *)0};
    const char *n_path_sensor_id[] = {"sensor_id",(const char *)0};
    const char *n_path_validate_cert_chain[] = {"validate_certificate_chain",
        (const char *)0};
    const char *n_path_number_of_clients[] = {"number_of_clients",
        (const char *)0};
    char *validate_certificate_chain;
    int number_of_clients;

    struct send_to_server_config_t *inst = malloc(sizeof(
                struct send_to_server_config_t));

    inst->host = NULL;
    inst->username = NULL;
    inst->password = NULL;
    inst->sensor_id = NULL;
    inst->validate_certificate_chain = 0;
    inst->number_of_clients = 2;

    yajl_val host_ = yajl_tree_get(*val, n_path_host, yajl_t_string);
    inst->host = YAJL_GET_STRING(host_);

    yajl_val username_ = yajl_tree_get(*val, n_path_username, yajl_t_string);
    inst->username = YAJL_GET_STRING(username_);

    yajl_val password_ = yajl_tree_get(*val, n_path_password, yajl_t_string);
    inst->password = YAJL_GET_STRING(password_);

    yajl_val sensor_id_ = yajl_tree_get(*val, n_path_sensor_id, yajl_t_string);
    inst->sensor_id = YAJL_GET_STRING(sensor_id_);

    yajl_val chain_ = yajl_tree_get(*val, n_path_validate_cert_chain,
            yajl_t_string);
    validate_certificate_chain = YAJL_GET_STRING(chain_);

    yajl_val number_ = yajl_tree_get(*val, n_path_number_of_clients,
            yajl_t_number);
    number_of_clients = YAJL_GET_INTEGER(number_);

    if (validate_certificate_chain &&
            strcmp("true", validate_certificate_chain) != 0)
    {
        inst->validate_certificate_chain = 0;
    }

    if (number_of_clients > 0)
    {
        inst->number_of_clients = number_of_clients;
    }

    return inst;
}

