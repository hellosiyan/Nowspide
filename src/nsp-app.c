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
 
#include "nsp-app.h"

static NspApp *_app = NULL;

static void
nsp_app_feed_item_list_sel (GtkTreeSelection *selection, gpointer user_data)
{
	NspApp *app = nsp_app_get();
	GtkTreeIter iter, child_iter;
	GtkTreeModel *model;
	NspFeedItem *feed_item = NULL;
	
	if ( gtk_tree_selection_get_selected(selection, &model, &iter) ) {
		gtk_tree_model_get(model, &iter, ITEM_LIST_COL_ITEM_REF, &feed_item, -1);
	}
	app->current_feed_item = feed_item;
	
	if ( feed_item == NULL ) {
		return;
	}
	
	g_mutex_lock(app->current_feed->mutex);
	gtk_tree_model_sort_convert_iter_to_child_iter(GTK_TREE_MODEL_SORT(app->current_feed->items_sorter), &child_iter, &iter);
	
	if ( feed_item->status & NSP_FEED_ITEM_UNREAD ) {
		feed_item->status ^= NSP_FEED_ITEM_UNREAD;
		nsp_jobs_queue(app->jobs, nsp_job_new((NspCallback*)nsp_feed_item_save_status_to_db, (void*)feed_item));
		nsp_feed_item_list_update_iter(child_iter, app->current_feed->items_store, feed_item);
		
		app->current_feed->unread_items --;
		nsp_feed_list_update_entry(app->window->feed_list,  app->current_feed);
	}
	g_mutex_unlock(app->current_feed->mutex);
	
	return;
}

static void 
nsp_app_feed_update_real (void* user_data)
{
	NspApp *app = nsp_app_get();
	
	nsp_feed_update_items((NspFeed*)user_data);
	
	GDK_THREADS_ENTER();
	nsp_feed_update_model((NspFeed*)user_data);
	nsp_feed_list_update_entry(app->window->feed_list, (NspFeed*)user_data);
	GDK_THREADS_LEAVE();
}

static void 
nsp_app_feed_update(void* user_data)
{
	NspApp *app = nsp_app_get();
	nsp_jobs_queue(app->jobs, nsp_job_new((NspCallback*)nsp_app_feed_update_real, user_data));
}

static void 
nsp_app_feed_item_delete(void* user_data)
{
	NspApp *app = nsp_app_get();
	NspFeedItem *feed_item = (NspFeedItem*)user_data;
	gboolean foo; /* required by g_signal_emit_by_name */
	
	gtk_widget_grab_focus (GTK_WIDGET (app->window->feed_item_list));
	g_signal_emit_by_name (GTK_TREE_VIEW(app->window->feed_item_list), "move-cursor", GTK_MOVEMENT_DISPLAY_LINES, 1, &foo);
		
	nsp_feed_delete_item(app->current_feed, feed_item);
}

static void
nsp_app_feed_item_toggle_read(void* user_data)
{
	NspApp *app = nsp_app_get();
	NspFeedItem *feed_item = (NspFeedItem*)user_data;
	GtkTreeIter iter;
	
	if ( feed_item->status & NSP_FEED_ITEM_UNREAD ) {
		return;
	}
	
	feed_item->status |= NSP_FEED_ITEM_UNREAD;
	
	if( nsp_feed_item_list_search(GTK_TREE_MODEL(app->current_feed->items_store), feed_item, &iter) ) {
		nsp_feed_item_list_update_iter( iter, app->current_feed->items_store, feed_item);
		nsp_feed_item_save_status_to_db(feed_item);
		app->current_feed->unread_items ++;
		nsp_feed_list_update_entry(app->window->feed_list,  app->current_feed);
	} 
}


static void
nsp_app_load_feeds(NspApp *app)
{
	GList *feeds;
	assert(app->feeds == NULL);
	feeds = app->feeds = nsp_feed_load_feeds_from_db();
	
	while ( feeds != NULL ) {
		nsp_feed_load_items_from_db((NspFeed*) feeds->data);
		nsp_feed_update_model((NspFeed*) feeds->data);
		nsp_feed_list_add(app->window->feed_list, (NspFeed*) feeds->data);
		
		feeds = feeds->next;
	}
}


static void
nsp_app_window_show(NspApp *app)
{
	gtk_widget_show(app->window->window);
}

static void
nsp_app_feed_list_select (void* user_data)
{
	NspApp *app = nsp_app_get();
	NspFeed *feed = (NspFeed*) user_data;
	GtkWidget *feed_item_header;
	
	app->current_feed = feed;
	app->current_feed_item = NULL;
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(app->window->feed_item_list), nsp_feed_get_items_model(feed));
	
	feed_item_header = GTK_WIDGET(gtk_builder_get_object(app->window->builder, "feed_item_list_header"));
	gtk_label_set_text(GTK_LABEL(feed_item_header), feed->title);
}

static void
nsp_app_feed_add (void* user_data)
{
	NspApp *app = nsp_app_get();
	char *url = g_strdup((const char*) user_data);
	NspFeed *feed;
	
	if ((feed = nsp_feed_new_from_url(url))) {
		nsp_feed_update_items(feed);
		if (!nsp_feed_save_to_db(feed)) {
			GDK_THREADS_ENTER();
			nsp_feed_update_model(feed);
			nsp_feed_update_unread_count(feed);
			nsp_feed_list_add(app->window->feed_list, feed);
			GDK_THREADS_LEAVE();
			nsp_feed_update_icon(feed);
		}
	}
}

static void 
nsp_app_window_init(NspApp *app)
{
	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(app->window->feed_item_list));
	
	g_signal_connect(selection, "changed", G_CALLBACK(nsp_app_feed_item_list_sel), NULL);	
}


static NspApp *
nsp_app_new ()
{
	NspApp *app;
	app = malloc(sizeof(NspApp));
	
	assert(app != NULL);
	
	app->feeds = NULL;
	app->current_feed = NULL;
	app->current_feed_item = NULL;
	
	app->db = nsp_db_get();
	
	app->jobs = nsp_jobs_new();
	
	app->window = nsp_window_new();
	nsp_window_init(app->window, NULL);
	nsp_app_window_init(app);
	
	app->window->on_feed_add = nsp_app_feed_add;
	app->window->on_feed_update = nsp_app_feed_update;
	app->window->on_feed_item_delete = nsp_app_feed_item_delete;
	app->window->on_feed_item_toggle_read = nsp_app_feed_item_toggle_read;
	app->window->feed_list->on_select = nsp_app_feed_list_select;
	
	nsp_app_window_show(app);
	
	nsp_app_load_feeds(app);
	
	return app;
}

NspApp *
nsp_app_get()
{
	if ( _app == NULL ) {
		_app = nsp_app_new();
	}
	return _app;
}

void
nsp_app_free(NspApp *app)
{
	if (app == NULL) {
		return;
	}
	while (app->feeds != NULL) {
		nsp_feed_free((NspFeed*) app->feeds->data);
		app->feeds = app->feeds->next;
	}
	nsp_window_free(app->window);
	nsp_db_close(app->db);
}


