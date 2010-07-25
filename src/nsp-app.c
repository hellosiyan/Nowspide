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
	feeds = app->feeds = nsp_db_load_feeds_with_items(app->db);
	
	while ( feeds != NULL ) {
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
	NspFeedItemList *item_list = nsp_feed_item_list_new();
	
	nsp_feed_item_list_add_from_list(item_list, feed->items);
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(app->window->feed_item_list), item_list->list_model);
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


