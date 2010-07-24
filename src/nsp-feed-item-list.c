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

enum
{
	LIST_COL_NAME = 0,
	LIST_COL_NUM
};

NspFeedItemList *
nsp_feed_item_list_new ()
{
	NspFeedItemList *list;
	GtkTreeViewColumn *column;
	list = malloc(sizeof(NspFeedItemList));
	
	assert(list != NULL);
	
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "ellipsize", PANGO_ELLIPSIZE_END, "width-chars", -1, "wrap-mode", PANGO_WRAP_WORD, NULL);
	
	
	list->list_model = (GtkTreeModel *)gtk_list_store_new(LIST_COL_NUM, G_TYPE_STRING);
	
	list->list_view = gtk_tree_view_new();
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(list->list_view), TRUE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list->list_view), FALSE);
	
	column = gtk_tree_view_column_new_with_attributes ("Name", renderer, "markup", LIST_COL_NAME, NULL);
	gtk_tree_view_insert_column (GTK_TREE_VIEW(list->list_view), column, -1);
	gtk_tree_view_column_set_sort_column_id (column, LIST_COL_NAME);
	g_object_set (column, "resizable", TRUE, "expand", TRUE, NULL);
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(list->list_view), list->list_model);

	return list;
}


void
nsp_feed_item_list_add (NspFeedItemList *list, NspFeedItem *item)
{
	GtkTreeIter iter;
	char *col_name = NULL;
	
	col_name = g_strdup_printf("%s", item->title);
	
	
	gtk_list_store_append (GTK_LIST_STORE(list->list_model), &iter);
	gtk_list_store_set (GTK_LIST_STORE(list->list_model), &iter,
					LIST_COL_NAME, col_name,
					-1);
	
	g_free(col_name);
	
	printf("Add feed item\n");
}

