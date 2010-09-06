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
 
#include "nsp-feed-item-list.h"

#include <gtk/gtk.h>
#include <assert.h>

NspFeedItemList *
nsp_feed_item_list_new ()
{
	NspFeedItemList *list;
	list = malloc(sizeof(NspFeedItemList));
	
	assert(list != NULL);
	
	
	list->list_model = (GtkTreeModel *)gtk_tree_store_new(ITEM_LIST_COL_NUM, G_TYPE_STRING, G_TYPE_STRING);

	return list;
}

GtkTreeStore *
nsp_feed_item_list_get_model()
{
	GtkTreeStore *model;
	
	model = gtk_tree_store_new(ITEM_LIST_COL_NUM, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
	
	return model;
}

GtkWidget *
nsp_feed_item_list_get_view()
{
	GtkWidget *list_view;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "ellipsize", PANGO_ELLIPSIZE_NONE, "width-chars", -1, "wrap-mode", PANGO_WRAP_WORD, NULL);
	
	list_view = gtk_tree_view_new();
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(list_view), TRUE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list_view), FALSE);
	
	column = gtk_tree_view_column_new_with_attributes ("Date", renderer, "text", ITEM_LIST_COL_DATE, NULL);
	gtk_tree_view_insert_column (GTK_TREE_VIEW(list_view), column, -1);
	gtk_tree_view_column_set_sort_column_id (column, ITEM_LIST_COL_DATE);
	
	renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "ellipsize", PANGO_ELLIPSIZE_END, "width-chars", -1, "wrap-mode", PANGO_WRAP_WORD, NULL);
	
	column = gtk_tree_view_column_new_with_attributes ("Name", renderer, "markup", ITEM_LIST_COL_NAME, NULL);
	gtk_tree_view_insert_column (GTK_TREE_VIEW(list_view), column, -1);
	gtk_tree_view_column_set_sort_column_id (column, ITEM_LIST_COL_NAME);
	g_object_set (column, "resizable", TRUE, "expand", TRUE, NULL);
	
	gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(list_view), FALSE);
	
	return list_view;
}


void 
nsp_feed_item_list_update_iter(GtkTreeIter iter, GtkTreeStore *store, NspFeedItem *feed_item)
{
	char *col_name = NULL, *col_date = NULL;
		
	if ( feed_item->status == NSP_FEED_ITEM_UNREAD) {
		col_name = g_strdup_printf("<b>%s</b>", feed_item->title);
	} else {
		col_name = g_strdup_printf("%s", feed_item->title);
	}
	
	col_date = g_strdup_printf("%.2i-%.2i-%.2i %.2i:%.2i", feed_item->pubdate->tm_mday, feed_item->pubdate->tm_mon, feed_item->pubdate->tm_year-100, feed_item->pubdate->tm_hour, feed_item->pubdate->tm_min);
	
	gtk_tree_store_set (store, &iter,
					ITEM_LIST_COL_DATE, col_date,
					ITEM_LIST_COL_NAME, col_name,
					ITEM_LIST_COL_ITEM_REF, feed_item,
					-1);

	g_free(col_name);
	g_free(col_date);
}
