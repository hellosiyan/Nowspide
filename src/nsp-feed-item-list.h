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
 
#ifndef __NSP_FEED_ITEM_LIST_H__
#define __NSP_FEED_ITEM_LIST_H__ 1

#include <gtk/gtk.h>

#include "nsp-feed.h"

enum
{
	ITEM_LIST_COL_DATE = 0,
	ITEM_LIST_COL_NAME,
	ITEM_LIST_COL_ITEM_REF,
	ITEM_LIST_ROW_WEIGHT,
	ITEM_LIST_COL_NUM
};

typedef struct _NspFeedItemList NspFeedItemList;

struct _NspFeedItemList {
	GtkTreeModel *list_model;
};

NspFeedItemList * nsp_feed_item_list_new();

GtkWidget * nsp_feed_item_list_get_view();
GtkTreeStore * nsp_feed_item_list_get_model();
void nsp_feed_item_list_update_iter(GtkTreeIter iter, GtkTreeStore *store, NspFeedItem *feed_item);


#endif /* __NSP_FEED_ITEM_LIST__ */
