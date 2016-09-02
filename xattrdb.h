/*
 * xattrdb.h
 *
 *  Created on: Sep 1, 2016
 *      Author: Nikolay Khabarov
 */

#ifndef XATTRDB_H_
#define XATTRDB_H_

#include <stdbool.h>

bool xattrdb_open(const char *);
bool xattrdb_close();

bool xattrdb_set(const char *, const char *, const char *, unsigned int);
bool xattrdb_get(const char *, const char *, unsigned int *, char *, unsigned int);
bool xattrdb_list(const char *, unsigned int *, char *, unsigned int);
bool xattrdb_removename(const char *, const char *);
bool xattrdb_removepath(const char *);
bool xattrdb_renamepath(const char *, const char *);

#endif /* XATTRDB_H_ */
