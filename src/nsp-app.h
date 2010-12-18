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
 
#ifndef __NSP_APP_H__
#define __NSP_APP_H__ 1

#include <stdlib.h>
#include <assert.h>
#include <gtk/gtk.h>

#include "nsp-db.h"
#include "nsp-jobs.h"
#include "nsp-feed.h"
#include "nsp-feed-item.h"
#include "nsp-window.h"

typedef struct _NspApp NspApp;

struct _NspApp {
	GList *feeds;
	NspWindow *window;
	NspDb *db;
	NspJobs *jobs;
	
	NspFeed *current_feed;
	NspFeedItem *current_feed_item;
};

NspApp *nsp_app_get();

void nsp_app_cmd_feed_update (GtkAction *action, gpointer user_data);
void nsp_app_cmd_feed_read_all (GtkAction *action, gpointer user_data);
void nsp_app_cmd_feed_delete (GtkAction *action, gpointer user_data);

#endif /* __NSP_APP_H__ */
