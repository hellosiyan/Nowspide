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
#include "nsp-window.h"
#include "nsp-feed.h"

#include <stdlib.h>
#include <assert.h>

static NspApp *_app = NULL;

static void
nsp_app_load_feeds(NspApp *app)
{
	GList *feeds;
	assert(app->feeds == NULL);
	feeds = app->feeds = nsp_feed_load_feeds_from_db();
	
	while ( feeds != NULL ) {
		nsp_feed_load_items_from_db((NspFeed*) feeds->data);
		nsp_feed_list_add(app->window->feed_list, (NspFeed*) feeds->data);
		
		feeds = feeds->next;
	}
}

static void
nsp_app_window_show(NspApp *app)
{
	gtk_widget_show_all(app->window->window);
}

static void
nsp_app_feed_list_select (void* user_data)
{
	NspApp *app = nsp_app_get();
	NspFeed *feed = (NspFeed*) user_data;
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(app->window->feed_item_list), nsp_feed_get_items_model(feed));
}

static void
nsp_app_feed_add (void* user_data)
{
	NspApp *app = nsp_app_get();
	char *url = g_strdup((const char*) user_data);
	NspFeed *feed;
	
	if ((feed = nsp_feed_new_from_url(url))) {
		nsp_feed_update_items(feed);
		nsp_feed_save_to_db(feed);
		nsp_feed_update_model(feed);
		nsp_feed_list_add(app->window->feed_list, feed);
	}
}

static int
nsp_app_update_feed(GtkButton *button, gpointer user_data) 
{
	NspApp *app = nsp_app_get();
	GtkTreeIter iter;
	GtkTreeModel *model;
	NspFeed *feed = NULL;
	
	if ( gtk_tree_selection_get_selected(gtk_tree_view_get_selection(GTK_TREE_VIEW(app->window->feed_list->list_view)), &model, &iter) ) {
		gtk_tree_model_get(model, &iter, LIST_COL_FEED_REF, &feed, -1);
	}
	
	nsp_feed_update_items(feed);
	
	return 0;
}

static NspApp *
nsp_app_new ()
{
	NspApp *app;
	app = malloc(sizeof(NspApp));
	
	assert(app != NULL);
	
	app->feeds = NULL;
	
	app->db = nsp_db_get();
	
	app->window = nsp_window_new();
	nsp_window_init(app->window, NULL);
	
	app->window->on_feed_add = nsp_app_feed_add;
	app->window->feed_list->on_select = nsp_app_feed_list_select;
	g_signal_connect(G_OBJECT(gtk_builder_get_object(app->window->builder, "btn_update")), "clicked", G_CALLBACK(nsp_app_update_feed), NULL);
	
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


