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

static GtkCellRenderer *
nsp_feed_list_renderer_text()
{	
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	
	g_object_set(renderer, "ellipsize", PANGO_ELLIPSIZE_END, "width-chars", -1, "wrap-mode", PANGO_WRAP_WORD, NULL);
	
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
	
	if ( list->on_select != NULL && feed != NULL ) {
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
	list->icon_load = gtk_icon_theme_load_icon(gtk_icon_theme_get_default(), "gtk-refresh", 16, 0, NULL);
	
	
	list->list_model = (GtkTreeModel *)gtk_tree_store_new(LIST_COL_NUM, G_TYPE_STRING, G_TYPE_POINTER, GDK_TYPE_PIXBUF);
	
	list->list_view = gtk_tree_view_new();
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(list->list_view), TRUE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list->list_view), FALSE);
	
	// Name column
	column = gtk_tree_view_column_new_with_attributes ("Name", renderer, "markup", LIST_COL_NAME, NULL);
	gtk_tree_view_insert_column (GTK_TREE_VIEW(list->list_view), column, 1);
	gtk_tree_view_column_set_sort_column_id (column, LIST_COL_NAME);
	g_object_set (column, "resizable", TRUE, "expand", TRUE, NULL);
	
	// Icon column
	renderer = gtk_cell_renderer_pixbuf_new();
	column = gtk_tree_view_column_new_with_attributes ("Icon", renderer, "pixbuf", LIST_COL_ICON, NULL);
	gtk_tree_view_insert_column (GTK_TREE_VIEW(list->list_view), column, 0);
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(list->list_view), list->list_model);
	gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(list->list_view), FALSE);
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list->list_view));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
	
	g_signal_connect(selection, "changed", G_CALLBACK(nsp_feed_list_sel), list);
	
	return list;
}

GtkTreeIter
nsp_feed_list_add(NspFeedList *list, NspFeed *feed)
{
	GtkTreeIter iter;
	
	gtk_tree_store_append (GTK_TREE_STORE(list->list_model), &iter, NULL);
	
	gtk_tree_store_set (GTK_TREE_STORE(list->list_model), &iter,
					LIST_COL_FEED_REF, feed, LIST_COL_ICON, feed->icon,
					-1);
					
	nsp_feed_list_update_entry(list, feed);
	
	return iter;
}

void
nsp_feed_list_remove(NspFeedList *list, NspFeed *feed)
{
	GtkTreeIter iter;
	
	if( !nsp_feed_list_search(list, feed, &iter) ) {
		return;
	}
	
	gtk_tree_store_remove(GTK_TREE_STORE(list->list_model), &iter);
}

gboolean
nsp_feed_list_search(NspFeedList *list, NspFeed *feed, GtkTreeIter *it)
{
	NspFeed *f = NULL;
	GtkTreeIter iter;
	gboolean valid;
	
	assert(feed != NULL);
	
	valid = gtk_tree_model_get_iter_first (list->list_model, &iter);
	while (valid)
	{
		gtk_tree_model_get (list->list_model, &iter,
				LIST_COL_FEED_REF, &f,
				-1
			);
		
		if ( f != NULL && f == feed ) {
			break;
		}
		
		valid = gtk_tree_model_iter_next (list->list_model, &iter);
	}
	
	if ( valid ) 
		*it = iter;
	
	return valid;
}

void 
nsp_feed_list_update_entry(NspFeedList *list, NspFeed *feed)
{
	GtkTreeIter iter;
	char *col_name = NULL;
	
	if( !nsp_feed_list_search(list, feed, &iter) ) {
		return;
	}
	
	if ( feed->unread_items > 0 ) {
		col_name = g_strdup_printf("<b>%s (%i)</b>", feed->title, feed->unread_items);
	} else {
		col_name = g_strdup_printf("%s", feed->title);
	}
	
	if ( feed->description != NULL && *(feed->description) != '\0' ) {
		char *old_col_name = col_name;
		col_name = g_strdup_printf("%s\n<small>%s</small>", col_name, feed->description);
		g_free(old_col_name);
	}
	
	
	gtk_tree_store_set (GTK_TREE_STORE(list->list_model), &iter,
					LIST_COL_NAME, col_name,
					LIST_COL_FEED_REF, feed,
					LIST_COL_ICON, feed->icon,
					-1);
	
	g_free(col_name);
}
