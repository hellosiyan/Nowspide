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


#ifndef __NSP_FEED_ITEM_H_
#define __NSP_FEED_ITEM_H_ 1

#include <time.h>
#include <gtk/gtk.h>

#include "nsp-typedefs.h"

typedef enum {
	NSP_FEED_ITEM_UNREAD = 	0b00000001,
	NSP_FEED_ITEM_DELETED =	0b00000010,
} NspFeedItemStatus;

typedef struct _NspFeedItem NspFeedItem;

struct _NspFeedItem
{
	int id;
	int feed_id;
	unsigned char status;
	char *title;
	char *link;
	char *description;
	struct tm *pubdate;
};

NspFeedItem	* nsp_feed_item_new();
void 	nsp_feed_item_free(NspFeedItem *item);

int 	nsp_feed_item_save_to_db(NspFeedItem *feed_item);
int 	nsp_feed_item_save_status_to_db(NspFeedItem *feed_item);

GList * nsp_feed_items_search(char *exp);

#endif /* __NSP_FEED_ITEM_H_ */
