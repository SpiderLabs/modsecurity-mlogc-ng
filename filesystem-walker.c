
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include "mlogc-ng.h"
#include "input_batch.h"

int inspect_file(unsigned char *full_path, void *audit_log_entry_cb)
{
    int res = 0;
    FILE *fp;
    struct stat s;
    char *file_content_buf = NULL;
    struct audit_log_entry_t audit_log;
    void (*ptrEntry)(struct audit_log_entry_t audit_log);
    ptrEntry = audit_log_entry_cb;


    d("inspecting: %s\n", full_path);

    res = stat(full_path, &s);
    if (res < 0)
    {
        e("Not able to get file size: %s\n", full_path);
        goto failed;
    }

    if (s.st_size > MAX_FILE_SIZE)
    {
        e("This file is too big for us. Limit is: %d bytes\n", MAX_FILE_SIZE);
        res = -1;
        goto failed;
    }

    file_content_buf = malloc(s.st_size + 1);
    if (file_content_buf == NULL)
    {
        e("Failed to allocate memory to read: %s\n", full_path);
        res = -1;
        goto failed;
    }

    /* Everything is ok, lets have the data inside a buffer. */
    p("open : %s\n", full_path);
    fp = fopen(full_path, "r");
    if (fp == NULL)
    {
        e("Failed to open the file: %s\n", full_path);
        res = -1;
        goto failed_fp;
    }

    fread(file_content_buf, sizeof(char), s.st_size, fp);
    fclose(fp);

    if (load_buffer(file_content_buf, &audit_log) == 0)
    {
        (*ptrEntry)(audit_log);
    }

failed_fp:
    free(file_content_buf);
failed:
    return res;
}

int open_directory_recursive(unsigned char *path, void *audit_log_entry_cb)
{
    DIR *d;
    struct dirent *entry;
    int res = 0;

    d = opendir(path);
    if (d == NULL)
    {
        e("Failed to open directory: %s\n", path);
        res = -1;
        goto failed;
    }

    entry = readdir(d);
    while (entry != NULL)
    {
        const char *d_name = NULL;
        int full_path_len = 0;
        char *full_path = NULL;

        d_name = entry->d_name;

        full_path_len = snprintf(NULL, 0, "%s/%s", path, d_name);
        full_path = malloc(sizeof(char) * (full_path_len + 1));
        if (full_path == NULL)
        {
            fprintf(stderr, "Failed: missing memory to continue.\n");
            return -1;
        }
        
        full_path_len = snprintf(full_path, full_path_len + 1, "%s/%s", path, d_name);

        /* if it is a dir (different from "." or ".." we want to jump in. */
        if (entry->d_type & DT_DIR)
        {
            if (strcmp (d_name, "..") != 0 && strcmp (d_name, ".") != 0)
            {
                // FIXME: we should not ignore this return value.
                open_directory_recursive(full_path, audit_log_entry_cb);
            }
        }

        /* if it is a file, let see inside. */
        if (!(entry->d_type & DT_DIR))
        {
            inspect_file(full_path, audit_log_entry_cb);
        }

        free(full_path);

        entry = readdir(d);
    }

failed_read_dir:
    closedir(d);
failed:
    return res;
}


