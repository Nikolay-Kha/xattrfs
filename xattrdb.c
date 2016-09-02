#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "xattrdb.h"

static sqlite3 *db = NULL;

#define ENSURE_TABLE_EXISTS "CREATE TABLE IF NOT EXISTS '%s'(name unique, value);"

typedef struct {
    char *value;
    unsigned int len;
    unsigned int *num;
} cb_data;

bool xattrdb_open(const char *file) {
    int res = sqlite3_open(file, &db);
    return res == SQLITE_OK;
}

bool xattrdb_close() {
    const int res = sqlite3_close(db);
    return res == SQLITE_OK;
}

bool xattrdb_set(const char *path, const char *name, const char *value, unsigned int len) {
    char req[32768];
    snprintf(req, sizeof(req),
        ENSURE_TABLE_EXISTS
        "INSERT OR REPLACE INTO '%s'(name, value) VALUES ('%s', '%.*s');",
        path, path, name, len, value);
    const int res = sqlite3_exec(db, req, NULL, NULL, NULL);
    return res == SQLITE_OK;
}

static int get_callback(void *data, int argc, char **argv, char **azColName){
    cb_data *cbdata = (cb_data*)data;
    if(argc < 1) {
        return 0;
    }
    if(cbdata->value) {
        *cbdata->num = snprintf(cbdata->value, cbdata->len, "%s", argv[0]);
    } else {
        *cbdata->num = strlen(argv[0]) + 1;
    }
    return 0;
}

bool xattrdb_get(const char *path, const char *name, unsigned int *out_num, char *out_value, unsigned int out_len) {
    char req[32768];
    snprintf(req, sizeof(req),
        ENSURE_TABLE_EXISTS
        "SELECT value FROM '%s' WHERE name = '%s';",
        path, path, name);
    cb_data data;
    data.value = out_value;
    data.len = out_len;
    data.num = out_num;
    *data.num = 0;
    if(data.len == 0) {
        data.value = NULL;
    }
    const int res = sqlite3_exec(db, req, get_callback, (void *)&data, NULL);
    return res == SQLITE_OK;
}

static int list_callback(void *data, int argc, char **argv, char **azColName){
    cb_data *cbdata = (cb_data*)data;
    int i;
    for(i = 0; i < argc; i++) {
        int l = 0;
        if(cbdata->value) {
            l = snprintf(cbdata->value, cbdata->len, "%s", argv[i]);
        } else {
            l = strlen(argv[i]);
        }
        l++; // zero terminated character
        *cbdata->num += l;
        if(cbdata->value) {
            cbdata->value += l;
            if(cbdata->len <= l) {
                break;
            }
            cbdata->len -= l;
        }
    }
    return 0;
}

bool xattrdb_list(const char *path, unsigned int *out_num, char *out_buf, unsigned int out_len) {
    char req[32768];
    snprintf(req, sizeof(req),
        ENSURE_TABLE_EXISTS
        "SELECT name FROM '%s';",
        path, path);
    cb_data data;
    data.value = out_buf;
    data.len = out_len;
    data.num = out_num;
    *data.num = 0;
    if(data.len == 0) {
        data.value = NULL;
    }
    const int res = sqlite3_exec(db, req, list_callback, (void *)&data, NULL);
    return res == SQLITE_OK;
}

bool xattrdb_removename(const char *path, const char *name) {
    char req[32768];
    snprintf(req, sizeof(req),
        "DELETE FROM '%s' WHERE name = '%s';",
        path, name);
    const int res = sqlite3_exec(db, req, NULL, NULL, NULL);
    return res == SQLITE_OK;
}

bool xattrdb_removepath(const char *path) {
    char req[32768];
    snprintf(req, sizeof(req),
        "DROP TABLE '%s';",
        path);
    const int res = sqlite3_exec(db, req, NULL, NULL, NULL);
    return res == SQLITE_OK;
}

bool xattrdb_renamepath(const char *oldpath, const char *newpath) {
    char req[32768];
    snprintf(req, sizeof(req),
        "ALTER TABLE '%s' RENAME TO '%s';",
        oldpath, newpath);
    const int res = sqlite3_exec(db, req, NULL, NULL, NULL);
    return res == SQLITE_OK;
}
