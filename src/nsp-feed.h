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


#ifndef __NSP_FEED_H_
#define __NSP_FEED_H_ 1

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <gtk/gtk.h>
#include <time.h>

typedef enum {
	NSP_FEED_UNKNOWN,
	NSP_FEED_RSS_0_9,
	NSP_FEED_RSS_1_0,
	NSP_FEED_RSS_2_0,
	NSP_FEED_ATOM_0_3,
	NSP_FEED_ATOM_1_0
} NspFeedType;

typedef struct _NspFeedItem NspFeedItem;

struct _NspFeedItem
{
	int id;
	int feed_id;
	char *title;
	char *link;
	char *description;
	struct tm *pubdate;
};

typedef struct _NspFeed NspFeed;

struct _NspFeed
{
	NspFeedType type;
	int id;
	char *title;
	char *url;
	char *description;
	GList * items;
	GtkListStore *items_store;
};

NspFeedItem	* nsp_feed_item_new();
void nsp_feed_item_free(NspFeedItem *item);

NspFeed * nsp_feed_new();
NspFeed * nsp_feed_new_from_url(const char *xml);
GtkTreeModel *nsp_feed_get_items_model(NspFeed *feed);

int nsp_feed_load_items_from_db(NspFeed *feed);

void nsp_feed_update_model(NspFeed *feed);

void	nsp_feed_free 		(NspFeed *feed);

#endif /* __NSP_FEED_H_ */
