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

#include "nsp-feed-item.h"
#include "nsp-db.h"

#include <assert.h>

NspFeedItem *
nsp_feed_item_new()
{
	NspFeedItem * item = malloc(sizeof(NspFeedItem));
	assert(item != NULL);
	
	item->id = item->feed_id = 0;
	item->status = 0 | NSP_FEED_ITEM_UNREAD;
	item->title = item->link = item->description = NULL;
	item->pubdate = NULL;
	
	return item;
}

void
nsp_feed_item_free(NspFeedItem * item)
{
	if ( item == NULL) {
		return;
	}
	
	item->status |= NSP_FEED_ITEM_DELETED;
	free(item->title);
	item->title = NULL;
	free(item->link);
	item->link = NULL;
	free(item->description);
	item->description = NULL;
	free(item->pubdate);
	item->pubdate = NULL;
}

int
nsp_feed_item_save_to_db(NspFeedItem *feed_item) 
{
	NspDb *db = nsp_db_get();
	char *query = NULL;
	char *error = NULL;
	int stat;
	
	time_t date = 0;
	if ( feed_item->pubdate ) {
		date = mktime(feed_item->pubdate);
	}
	
	nsp_db_transaction_begin(db);
	
	if ( feed_item->id != 0 ) {
		query = sqlite3_mprintf(
				"UPDATE nsp_feed_item SET title = '%q', url = '%q', description = '%q', date = %i, status = %i WHERE id = %i", 
				feed_item->title, 
				feed_item->link, 
				feed_item->description, 
				date, 
				feed_item->status & NSP_FEED_ITEM_UNREAD, 
				feed_item->id
			);
	} else {
		assert(feed_item->feed_id != 0);
		query = sqlite3_mprintf(
				"INSERT OR IGNORE INTO nsp_feed_item (id, feed_id, title, url, description, date, status) VALUES (NULL, %i, '%q', '%q', '%q', %i, %i)", 
				feed_item->feed_id, 
				feed_item->title, 
				feed_item->link, 
				feed_item->description, 
				date, 
				feed_item->status & NSP_FEED_ITEM_UNREAD
			);
	}
	
	stat = sqlite3_exec(db->db, query, NULL, NULL, &error);
	sqlite3_free(query);
	
	nsp_db_transaction_end(db);
	
	if ( stat != SQLITE_OK ) {
		if ( error == NULL) {
			g_warning("Error: %s\n", sqlite3_errmsg(db->db));
		} else {
			g_warning("Error: %s\n", error);
			sqlite3_free(error);
		}

		return 1;
	}
	
	if ( feed_item->id == 0 ) {
		feed_item->id = sqlite3_last_insert_rowid(db->db);
	}
	
	return 0;
}

int
nsp_feed_item_save_status_to_db(NspFeedItem *feed_item) 
{
	NspDb *db = nsp_db_get();
	char *query = NULL;
	char *error = NULL;
	int stat;
	
	if ( feed_item->status & NSP_FEED_ITEM_DELETED || feed_item->id == 0 ) {
		return 0;
	}
	
	nsp_db_transaction_begin(db);
	
	query = sqlite3_mprintf(
			"UPDATE nsp_feed_item SET status = %i WHERE id = %i", 
			feed_item->status & NSP_FEED_ITEM_UNREAD, 
			feed_item->id
		);
	
	stat = sqlite3_exec(db->db, query, NULL, NULL, &error);
	sqlite3_free(query);
	
	nsp_db_transaction_end(db);
	
	if ( stat != SQLITE_OK ) {
		if ( error == NULL) {
			g_warning("Error: %s\n", sqlite3_errmsg(db->db));
		} else {
			g_warning("Error: %s\n", error);
			sqlite3_free(error);
		}

		return 1;
	}
	
	return 0;
}
