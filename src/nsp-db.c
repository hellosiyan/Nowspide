/*
 * Copyright © 2009-2010 Siyan Panayotov <xsisqox@gmail.com>
 *
 * This file is part of Nowspide.
 *
 * Nowspide is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nowspide is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nowspide.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nsp-db.h"

static NspDb *db = NULL;

static NspDb *
nsp_db_new()
{
	NspDb * nsp_db = malloc(sizeof(NspDb));
	assert(nsp_db != NULL);
	
	nsp_db->db = NULL;
	nsp_db->transaction_started = 0;
	nsp_db->mutex = g_mutex_new();
	
	return nsp_db;
}

static sqlite3 *
nsp_db_create()
{
	const char * dir = g_build_filename (g_get_user_data_dir(), PACKAGE, NULL);
	const char * path = g_build_filename(dir, "nsp.db", NULL);
	char *error = NULL;
	sqlite3 * db_file;
	int stat;

    if(g_mkdir_with_parents (dir, 0700) != 0)
        g_warning("Error creating database's parent directory (%s)\n", dir);
        
    if ( creat(path, S_IRWXU) == -1 )
        g_warning("Error creating database file (%s)\n", path);
     
	stat = sqlite3_open(path, &(db_file));
	if ( stat ) {
		g_warning("Error: %s\n", sqlite3_errmsg(db_file));
	}
	
	stat = sqlite3_exec(db_file," \
		BEGIN TRANSACTION; \
			CREATE TABLE nsp_feed ( id INTEGER PRIMARY KEY, title TEXT, url TEXT UNIQUE, site_url TEXT, description TEXT); \
			CREATE TABLE nsp_feed_item ( id INTEGER PRIMARY KEY, feed_id INTEGER, title TEXT, url TEXT,description TEXT, date INTEGER, status INTEGER, UNIQUE (feed_id, url)); \
		COMMIT; \
		",
		NULL, NULL, &error
	);
	
	if ( error != NULL) {
		g_warning("Error: %s", error);
		sqlite3_free(error);
	}
	
	return db_file;
}

NspDb *
nsp_db_get()
{
	if ( db != NULL ) {
		return db;
	}
	
	char *path = g_build_filename( g_get_user_data_dir(), PACKAGE, "/nsp.db", NULL);
	int stat;
	
	db = nsp_db_new();
	
	if ( !g_file_test(path, G_FILE_TEST_IS_REGULAR) ) {
		db->db = nsp_db_create();
	} else {
		stat = sqlite3_open(path, &(db->db));
		if ( stat ) {
			g_warning("Error: %s\n", sqlite3_errmsg(db->db));
		}
	}
	g_free(path);
	
	return db;
}

void
nsp_db_close(NspDb *nsp_db)
{
	if (nsp_db == NULL)
		return;
	
	sqlite3_close(nsp_db->db);
	free(nsp_db);
}


void
nsp_db_transaction_begin(NspDb *nsp_db)
{
	char *error = NULL;
	
	g_mutex_lock(nsp_db->mutex);
	nsp_db->transaction_started ++;
	
	if ( nsp_db->transaction_started != 1 ) {
		g_mutex_unlock(nsp_db->mutex);
		return;
	}
	
	sqlite3_exec(nsp_db->db, "BEGIN;", NULL, NULL, &error);
	if ( error != NULL) {
		g_warning("Error: %s\n", error);
		sqlite3_free(error);
		nsp_db->transaction_started = 0;
	} else {
		nsp_db->transaction_started = 1;
	}
	
	g_mutex_unlock(nsp_db->mutex);
}
void
nsp_db_transaction_end(NspDb *nsp_db)
{
	char *error = NULL;
	
	g_mutex_lock(nsp_db->mutex);
	nsp_db->transaction_started --;
	
	if ( !nsp_db->transaction_started != 1 ) {
		g_mutex_unlock(nsp_db->mutex);
		return;
	}
	
	sqlite3_exec(nsp_db->db, "COMMIT;", NULL, NULL, &error);
	if ( error != NULL) {
		g_warning("Error: %s\n", error);
		sqlite3_free(error);
	}
	
	nsp_db->transaction_started = 0;
	g_mutex_unlock(nsp_db->mutex);
}


int 
nsp_db_atom_int(void *user_data, int argc, char **argv, char ** azColName)
{
	int *value = (int*) user_data;
	
	*value = atoi(argv[0]);
	
	return 0;
}





