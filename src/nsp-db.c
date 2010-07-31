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

#include <sqlite3.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include "config.h"
#include "nsp-db.h"
#include "nsp-feed.h"

static NspDb *db = NULL;

static NspDb *
nsp_db_new()
{
	NspDb * nsp_db = malloc(sizeof(NspDb));
	assert(nsp_db != NULL);
	
	nsp_db->db = NULL;
	nsp_db->transaction_started = 0;
	
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
			CREATE TABLE nsp_feed ( id INTEGER PRIMARY KEY, title TEXT, url TEXT UNIQUE, description TEXT); \
			CREATE TABLE nsp_feed_item ( id INTEGER PRIMARY KEY, feed_id INTEGER, title TEXT, url TEXT,description TEXT); \
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
	char *path = g_build_filename( g_get_user_data_dir(), PACKAGE, "/nsp.db", NULL);
	int stat;
	
	if ( db != NULL ) {
		return db;
	}
	
	db = nsp_db_new();
	
	if ( !g_file_test(path, G_FILE_TEST_IS_REGULAR) ) {
		db->db = nsp_db_create();
	} else {
		stat = sqlite3_open(path, &(db->db));
		if ( stat ) {
			g_warning("Error: %s\n", sqlite3_errmsg(db->db));
		}
	}
	
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
	
	nsp_db->transaction_started ++;
	if ( nsp_db->transaction_started != 1 ) {
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
}
void
nsp_db_transaction_end(NspDb *nsp_db)
{
	char *error = NULL;
	
	nsp_db->transaction_started --;
	if ( !nsp_db->transaction_started != 1 ) {
		return;
	}
	
	sqlite3_exec(nsp_db->db, "COMMIT;", NULL, NULL, &error);
	if ( error != NULL) {
		g_warning("Error: %s\n", error);
		sqlite3_free(error);
	}
	nsp_db->transaction_started = 0;
}

int 
nsp_db_add_feed(NspDb *db, NspFeed *feed)
{
	char *query = sqlite3_mprintf("INSERT INTO nsp_feed (id, title, url, description) VALUES (NULL, '%q', '%q', '%q')", feed->title, feed->url, feed->description);
	char *error = NULL;
	int stat;
	
	nsp_db_transaction_begin(db);
	
	stat = sqlite3_exec(db->db, query, NULL, NULL, &error);
	sqlite3_free(query);
	
	if ( stat != SQLITE_OK ) {
		if ( error == NULL) {
			g_warning("Error: %s\n", sqlite3_errmsg(db->db));
		} else {
			g_warning("Error: %s\n", error);
			sqlite3_free(error);
		}
	
		nsp_db_transaction_end(db);
		return 1;
	}
	
	feed->id = sqlite3_last_insert_rowid(db->db);
	
	nsp_db_transaction_end(db);

	return 0;
}

int 
nsp_db_add_feed_with_items (NspDb *db, NspFeed *feed)
{
	NspFeedItem *tmp = NULL;
	char *query= NULL;
	char *error = NULL;
	int stat;
	
	nsp_db_transaction_begin(db);
	
	if( nsp_db_add_feed(db, feed) ) {
		return 1;
	}
	
	while ( feed->items != NULL ) {
		tmp = (NspFeedItem *) feed->items->data;
		query = sqlite3_mprintf("INSERT INTO nsp_feed_item (id, feed_id, title, url, description) VALUES (NULL, %i, '%q', '%q', '%q')", feed->id,tmp->title, tmp->link, tmp->description);
		
		stat = sqlite3_exec(db->db, query, NULL, NULL, &error);
		sqlite3_free(query);
		
		if ( stat != SQLITE_OK ) {
			if ( error == NULL) {
				g_warning("Error: %s\n", sqlite3_errmsg(db->db));
			} else {
				g_warning("Error: %s\n", error);
				sqlite3_free(error);
			}

			nsp_db_transaction_end(db);
			return 1;
		}
		
		feed->items = feed->items->next;
	}
	
	nsp_db_transaction_end(db);
	
	return 0;
}






