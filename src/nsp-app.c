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


static void
nsp_app_load_feeds(NspApp *app)
{
	GList *feeds;
	assert(app->feeds == NULL);
	feeds = app->feeds = nsp_db_load_feeds_with_items(app->db);
	
	while ( feeds != NULL ) {
		nsp_feed_list_view_add(app->window->feed_list, (NspFeed*) feeds->data);
		
		feeds = feeds->next;
	}
}

static void
nsp_app_window_show(NspApp *app)
{
	gtk_widget_show_all(app->window->window);
}

NspApp *
nsp_app_new ()
{
	NspApp *app;
	app = malloc(sizeof(NspApp));
	
	assert(app != NULL);
	
	app->feeds = NULL;
	
	app->db = nsp_db_get();
	
	app->window = nsp_window_new();
	nsp_window_init(app->window, NULL);
	nsp_app_window_show(app);
	
	nsp_app_load_feeds(app);
	
	return app;
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


