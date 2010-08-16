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

#include "nsp-feed.h"
#include "nsp-feed-parser.h"
#include "nsp-db.h"
#include "nsp-net.h"
#include "nsp-feed-item-list.h"
#include "nsp-feed-list.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>


static int 
nsp_feed_load_feed_items_callback(void *user_data, int argc, char **argv, char ** azColName)
{
	GList **feed_items = (GList**) user_data;
	NspFeedItem *feed_item = nsp_feed_item_new();
	
	feed_item->id = atoi(argv[0]);
	feed_item->feed_id = atoi(argv[1]);
	feed_item->title = g_strdup(argv[2]);
	feed_item->link = g_strdup(argv[3]);
	time_t time = (time_t)atol(argv[5]);
	
	feed_item->pubdate = malloc(sizeof(struct tm));
	memcpy( feed_item->pubdate, localtime(&time), sizeof(struct tm));
	
	*feed_items = g_list_prepend(*feed_items, (gpointer) feed_item);
	
	return 0;
}

static int 
nsp_feed_load_feeds_callback(void *user_data, int argc, char **argv, char ** azColName)
{
	GList **feeds = (GList**) user_data;
	NspFeed *f = nsp_feed_new();
	
	f->id = atoi(argv[0]);
	f->title = g_strdup(argv[1]);
	f->url = g_strdup(argv[2]);
	f->description = g_strdup(argv[3]);
	
	*feeds = g_list_prepend(*feeds, (gpointer) f);
	
	return 0;
}

static gint
nsp_feed_sort_date (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data)
{
	if ( model == NULL || a == NULL || b == NULL) {
		return 0;
	}
	NspFeedItem *item_a, *item_b;
	time_t t_a, t_b;
	
	gtk_tree_model_get(model, a, ITEM_LIST_COL_ITEM_REF, &item_a, -1);
	gtk_tree_model_get(model, b, ITEM_LIST_COL_ITEM_REF, &item_b, -1);
	
	if ( item_a == NULL || item_b == NULL || item_a->pubdate == NULL || item_b->pubdate == NULL) {
		return 1;
	}
	t_a = mktime(item_a->pubdate);
	t_b = mktime(item_b->pubdate);
	
	return t_a - t_b;
}

NspFeedItem *
nsp_feed_item_new()
{
	NspFeedItem * item = malloc(sizeof(NspFeedItem));
	assert(item != NULL);
	
	item->id = item->feed_id = 0;
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
	
	free(item->title);
	free(item->link);
	free(item->description);
	free(item->pubdate);
}

NspFeed * 
nsp_feed_new()
{
	NspFeed * feed = malloc(sizeof(NspFeed));
	assert(feed != NULL);
	
	feed->type = NSP_FEED_UNKNOWN;
	feed->items = NULL;
	feed->title = feed->url = feed->description = NULL;
	feed->id = 0;
	feed->items_store = nsp_feed_item_list_get_model();
	feed->items_sorter = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(feed->items_store));
	
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (feed->items_sorter), ITEM_LIST_COL_DATE, GTK_SORT_DESCENDING);
	gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE (feed->items_sorter), ITEM_LIST_COL_DATE, (GtkTreeIterCompareFunc)nsp_feed_sort_date, NULL, NULL);
	
	
	return feed;
}

GtkTreeModel *
nsp_feed_get_items_model (NspFeed *feed)
{
	return feed->items_sorter;
}

NspFeed * 
nsp_feed_new_from_url(const char *url)
{
	NspFeed * feed = nsp_feed_new();
	feed->url = (char*) url;
	
	if ( nsp_feed_update_items(feed) ) {
		nsp_feed_free(feed);
		return NULL;
	}
	
	return feed;
}

void
nsp_feed_free (NspFeed *feed)
{
	if ( feed == NULL)
		return;
	
	nsp_feed_clear_items(feed);
	
	free(feed->title);
	free(feed->url);
	free(feed->description);
}

int
nsp_feed_clear_items(NspFeed *feed)
{
	assert(feed != NULL);
	while (feed->items != NULL) {
		nsp_feed_item_free((NspFeedItem*) feed->items->data);
		feed->items = feed->items->next;
	}
	
	return 0;
}

int
nsp_feed_update_items(NspFeed *feed)
{
	NspNetData *data;
	xmlDoc *xml_doc;
	
	data = nsp_net_new();
	if ( nsp_net_load_url(feed->url, data) ) {
		g_warning("ERROR: %s\n", data->error);
		nsp_net_free(data);
		return 1;
	}
	
	xml_doc =  xmlReadMemory(data->content, data->size, NULL, NULL, 0);
	if ( xml_doc == NULL ) {
		g_warning("Error parsing xml!\n");
		nsp_net_free(data);
		return 1;
	}
	
	nsp_feed_clear_items(feed);
	nsp_feed_parse(xml_doc, feed);
	
	xmlFreeDoc(xml_doc);
	nsp_net_free(data);
	
	if ( feed->id != 0 ) {
		nsp_feed_save_to_db(feed);
		nsp_feed_load_items_from_db(feed);
	}
	
	return 0;
}

void
nsp_feed_update_model(NspFeed *feed) {
	GtkTreeIter iter;
	NspFeedItem *item;
	GList *items = feed->items;
	char *col_name = NULL, *col_date = NULL;
	
	gtk_tree_store_clear(GTK_TREE_STORE(feed->items_store));
	while( items != NULL ) {
		item = (NspFeedItem*) items->data;
		col_name = g_strdup_printf("%s", item->title);
		col_date = g_strdup_printf("%.2i-%.2i-%.2i %.2i:%.2i", item->pubdate->tm_mday, item->pubdate->tm_mon, item->pubdate->tm_year-100, item->pubdate->tm_hour, item->pubdate->tm_min);
	
		gtk_tree_store_append (GTK_TREE_STORE(feed->items_store), &iter, NULL);
		gtk_tree_store_set (GTK_TREE_STORE(feed->items_store), &iter,
						ITEM_LIST_COL_DATE, col_date,
						ITEM_LIST_COL_NAME, col_name,
						ITEM_LIST_COL_ITEM_REF, item,
						-1);
	
		g_free(col_name);
		g_free(col_date);
		
		items = items->next;
	}
}

int
nsp_feed_load_items_from_db(NspFeed *feed)
{
	NspDb *db = nsp_db_get();
	char *error = NULL;
	int stat;
	
	char *query = sqlite3_mprintf("SELECT id, feed_id, title, url, description, date FROM nsp_feed_item WHERE feed_id=%i ORDER BY id DESC", feed->id);
	
	nsp_feed_clear_items(feed);
	
	stat = sqlite3_exec(db->db, query, nsp_feed_load_feed_items_callback, &(feed->items), &error);
	sqlite3_free(query);
	
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

GList * 
nsp_feed_load_feeds_from_db()
{
	NspDb *db = nsp_db_get();
	char *error = NULL;
	GList *feed_list = NULL;
	int stat;
	
	stat = sqlite3_exec(db->db, "SELECT id, title, url, description FROM nsp_feed", nsp_feed_load_feeds_callback, &feed_list, &error);
	if ( stat != SQLITE_OK ) {
		if ( error == NULL) {
			g_warning("Error: %s\n", sqlite3_errmsg(db->db));
		} else {
			g_warning("Error: %s\n", error);
			sqlite3_free(error);
		}
		
		return NULL;
	}
	
	return feed_list;
}


GList * 
nsp_feed_load_feeds_with_items_from_db()
{
	GList *feeds = nsp_feed_load_feeds_from_db();
	GList *tmp = feeds;
	NspFeed *feed = NULL;
	
	while ( tmp != NULL ) {
		feed = (NspFeed *) tmp->data;
		nsp_feed_load_items_from_db(feed);
		tmp = tmp->next;
	}
	
	return feeds;
}

int
nsp_feed_save_to_db(NspFeed *feed)
{
	NspDb *db = nsp_db_get();
	NspFeedItem *tmp = NULL;
	GList *tmp_list = NULL;
	char *error = NULL;
	char *feed_id = g_strdup_printf("%i", feed->id);
	int stat;
	
	char *query = sqlite3_mprintf("INSERT %s INTO nsp_feed (id, title, url, description) VALUES (%s, '%q', '%q', '%q')", ((feed->id==0) ? "" : "OR REPLACE " ), ((feed->id==0) ? "NULL" : feed_id),feed->title, feed->url, feed->description);
	
	nsp_db_transaction_begin(db);
	
	stat = sqlite3_exec(db->db, query, NULL, NULL, &error);
	sqlite3_free(query);
	g_free(feed_id);
	
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
	
	tmp_list = feed->items;
	while ( tmp_list != NULL ) {
		tmp = (NspFeedItem *) tmp_list->data;
		time_t date = 0;
		if ( tmp->pubdate ) {
			date = timegm(tmp->pubdate);
		}
		
		query = sqlite3_mprintf("INSERT OR IGNORE INTO nsp_feed_item (id, feed_id, title, url, description, date) VALUES (NULL, %i, '%q', '%q', '%q', %i)", feed->id,tmp->title, tmp->link, tmp->description, date);
		
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
		
		tmp_list = tmp_list->next;
	}
	
	nsp_db_transaction_end(db);
	
	return 0;
	
}

