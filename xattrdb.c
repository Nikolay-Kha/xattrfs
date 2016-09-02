#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "xattrdb.h"

static sqlite3 *db = NULL;

#define SQL_REQUEST_MAX_SIZE 16768

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

static void ensure_table_exists(const char *path) {
    char req[SQL_REQUEST_MAX_SIZE];
    snprintf(req, sizeof(req), "CREATE TABLE IF NOT EXISTS '%s'(name unique, value);", path);
    sqlite3_exec(db, req, NULL, NULL, NULL);
}

bool xattrdb_set(const char *path, const char *name, const char *value, unsigned int len) {
    ensure_table_exists(path);
    char req[SQL_REQUEST_MAX_SIZE];
    int reqsize = snprintf(req, sizeof(req),
        "INSERT OR REPLACE INTO '%s'(name, value) VALUES ('%s', ?);",
        path, name);
    sqlite3_stmt *stmt;
    int res = sqlite3_prepare_v2(db, req, reqsize + 1, &stmt, NULL);
    if(res == SQLITE_OK) {
        res = sqlite3_bind_blob(stmt, 1, value, len, SQLITE_STATIC);
        if(res == SQLITE_OK) {
            res = sqlite3_step(stmt);
        }
    }
    sqlite3_finalize(stmt);
    return res == SQLITE_DONE;
}

bool xattrdb_get(const char *path, const char *name, unsigned int *out_num, char *out_value, unsigned int out_len) {
    ensure_table_exists(path);
    char req[SQL_REQUEST_MAX_SIZE];
    int reqsize = snprintf(req, sizeof(req),
        "SELECT value FROM '%s' WHERE name = '%s';",
        path, name);
    *out_num = 0;
    sqlite3_stmt *stmt;
    int res = sqlite3_prepare_v2(db, req, reqsize + 1, &stmt, NULL);
    if(res == SQLITE_OK) {
        res = sqlite3_step(stmt);
        if (res == SQLITE_ROW) {
            int len = sqlite3_column_bytes(stmt, 0);
            if(out_value != NULL && out_len != 0) {
                if(len > out_len) {
                    len = out_len;
                }
                char *data = (char *)sqlite3_column_blob(stmt, 0);
                memcpy(out_value, data, len);
            }
            *out_num = len;
            res = SQLITE_DONE;
        } else if(res == SQLITE_DONE) {
            *out_num = 0;
        }
    }
    sqlite3_finalize(stmt);

    return res == SQLITE_DONE;
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
    ensure_table_exists(path);
    char req[SQL_REQUEST_MAX_SIZE];
    snprintf(req, sizeof(req),
        "SELECT name FROM '%s';",
        path);
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
    char req[SQL_REQUEST_MAX_SIZE];
    snprintf(req, sizeof(req),
        "DELETE FROM '%s' WHERE name = '%s';",
        path, name);
    const int res = sqlite3_exec(db, req, NULL, NULL, NULL);
    return res == SQLITE_OK;
}

bool xattrdb_removepath(const char *path) {
    char req[SQL_REQUEST_MAX_SIZE];
    snprintf(req, sizeof(req),
        "DROP TABLE '%s';",
        path);
    const int res = sqlite3_exec(db, req, NULL, NULL, NULL);
    return res == SQLITE_OK;
}

bool xattrdb_renamepath(const char *oldpath, const char *newpath) {
    char req[SQL_REQUEST_MAX_SIZE];
    snprintf(req, sizeof(req),
        "ALTER TABLE '%s' RENAME TO '%s';",
        oldpath, newpath);
    const int res = sqlite3_exec(db, req, NULL, NULL, NULL);
    return res == SQLITE_OK;
}
