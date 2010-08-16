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

#include "nsp-feed-list.h"

#include <stdlib.h>
#include <assert.h>
#include <gtk/gtk.h>

static GtkCellRenderer *
nsp_feed_list_renderer_text()
{	
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	
	g_object_set(renderer, "ellipsize", PANGO_ELLIPSIZE_END, "width-chars", -1, "wrap-mode", PANGO_WRAP_WORD, NULL);
	
	return renderer;
}

static GtkCellRenderer *
nsp_feed_list_renderer_pixbuf()
{	
	GtkCellRenderer *renderer = gtk_cell_renderer_pixbuf_new();
	
	g_object_set(renderer, "stock-size", GTK_ICON_SIZE_BUTTON, NULL);
	
	return renderer;
}

static gboolean
nsp_feed_list_sel (GtkTreeSelection *selection, gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	NspFeedList *list = (NspFeedList *)user_data;
	NspFeed *feed = NULL;
	
	if ( gtk_tree_selection_get_selected(selection, &model, &iter) ) {
		gtk_tree_model_get(model, &iter, LIST_COL_FEED_REF, &feed, -1);
	}
	
	if ( list->on_select != NULL ) {
		list->on_select(feed);
	}
	
	return TRUE;
}

NspFeedList *
nsp_feed_list_new()
{
	NspFeedList * list = malloc(sizeof(NspFeedList));
	GtkTreeViewColumn *column;
	GtkTreeSelection *selection;
	assert(list != NULL);
	
	list->on_select = NULL;
	
	GtkCellRenderer *renderer = nsp_feed_list_renderer_text();
	
	
	list->list_model = (GtkTreeModel *)gtk_tree_store_new(LIST_COL_NUM, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
	
	list->list_view = gtk_tree_view_new();
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(list->list_view), TRUE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list->list_view), FALSE);
	
	// Icon column
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(list->list_view), -1, "Icon", nsp_feed_list_renderer_pixbuf(), "stock-id", LIST_COL_FAVICO, NULL);
	
	// Name columns
	column = gtk_tree_view_column_new_with_attributes ("Name", renderer, "markup", LIST_COL_NAME, NULL);
	gtk_tree_view_insert_column (GTK_TREE_VIEW(list->list_view), column, -1);
	gtk_tree_view_column_set_sort_column_id (column, LIST_COL_NAME);
	g_object_set (column, "resizable", TRUE, "expand", TRUE, NULL);
	
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(list->list_view), list->list_model);
	gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(list->list_view), FALSE);
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list->list_view));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
	
	g_signal_connect(selection, "changed", G_CALLBACK(nsp_feed_list_sel), list);
	
	return list;
}


void 
nsp_feed_list_add(NspFeedList *list, NspFeed *feed)
{
	GtkTreeIter iter;
	char *col_name = NULL;
	
	col_name = g_strdup_printf("%s\n<span color='#666666'><small>%s</small></span>", feed->title, feed->description);
	
	
	gtk_tree_store_append (GTK_TREE_STORE(list->list_model), &iter, NULL);
	gtk_tree_store_set (GTK_TREE_STORE(list->list_model), &iter,
					LIST_COL_FAVICO, GTK_STOCK_INFO,
					LIST_COL_NAME, col_name,
					LIST_COL_FEED_REF, feed,
					-1);
	
	g_free(col_name);
}


