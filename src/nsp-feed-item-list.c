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
	
	
	list->list_model = (GtkTreeModel *)gtk_list_store_new(LIST_COL_NUM, G_TYPE_STRING);

	return list;
}

GtkListStore *
nsp_feed_item_list_get_model()
{
	GtkListStore *model;
	
	model = gtk_list_store_new(LIST_COL_NUM, G_TYPE_STRING);
	
	return model;
}

GtkWidget *
nsp_feed_item_list_get_view()
{
	GtkWidget *list_view;
	GtkTreeViewColumn *column;
	
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "ellipsize", PANGO_ELLIPSIZE_END, "width-chars", -1, "wrap-mode", PANGO_WRAP_WORD, NULL);
	
	list_view = gtk_tree_view_new();
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(list_view), TRUE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list_view), FALSE);
	
	column = gtk_tree_view_column_new_with_attributes ("Name", renderer, "markup", LIST_COL_NAME, NULL);
	gtk_tree_view_insert_column (GTK_TREE_VIEW(list_view), column, -1);
	gtk_tree_view_column_set_sort_column_id (column, LIST_COL_NAME);
	g_object_set (column, "resizable", TRUE, "expand", TRUE, NULL);
	
	return list_view;
}

