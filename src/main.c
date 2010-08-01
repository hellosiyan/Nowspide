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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <gtk/gtk.h>
#include <curl/curl.h>
#include <sys/sysinfo.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "nsp-net.h"
#include "nsp-window.h"
#include "nsp-feed-parser.h"
#include "nsp-db.h"
#include "nsp-app.h"

void
handle_url(gpointer data, gpointer user_data ) {
	NspNetData *netdata = nsp_net_new();
	
	nsp_net_load_url((const char*) data, netdata);
	
	if ( netdata->error != NULL)
	{
		printf("ERROR: %s\n", netdata->error);
	}
	else
	{
		xmlDoc *xml_doc =  xmlReadMemory(netdata->content, netdata->size, NULL, NULL, 0);
		assert(xml_doc != NULL);
		GList *list = nsp_feed_item_parser_rss(xmlDocGetRootElement(xml_doc), NULL);
		
		// Test the generated GList
		while ( list != NULL ) {
			NspFeedItem * feed_item = (NspFeedItem *)list->data;
			if ( feed_item->pubdate != NULL) {
				printf("Date: %i-%i-%i at %i:%i:%i\n", feed_item->pubdate->tm_mday, feed_item->pubdate->tm_mon, feed_item->pubdate->tm_year + 1900, feed_item->pubdate->tm_hour, feed_item->pubdate->tm_min, feed_item->pubdate->tm_sec);
			}
			printf("Name: %s\nLink: %s\nDescription: %s\n#########\n", feed_item->title, feed_item->link, feed_item->description);
			list = list->next;
		}
	}
	nsp_net_free(netdata);
	netdata = NULL;
}

void
feed_list_example() {
	GThreadPool *fetch_pool;
	int i;
	printf("Hello, World!\nI'm Nowspide ...\n");
	
	char* sites[] = {
		"http://noscope.com/feed",		// RSS 2.0
		"http://feeds.feedburner.com/d0od",		// RSS 2.0
		"http://blogs.myspace.com/Modules/BlogV2/Pages/RssFeed.aspx?friendID=204244038",		// RSS 2.0s
		"http://www.gnomefiles.org/files/gnomefiles.rdf",		// RSS 1.0
		"http://www.linux-bg.org/linux-bg-news.rdf",		// RSS 0.9.1
		"http://www.feedzilla.com/rss/top-news/industry",		// RSS 2.0
		"http://git.gnome.org/cgit/eog/atom/?h=master",			// xmlns='http://www.w3.org/2005/Atom'
		"http://distrowatch.com/news/dwd.xml",		// RSS 2.0
		"http://fridge.ubuntu.com/node/feed",		// RSS 2.0
		"http://openfest.org/feed/",		// RSS 2.0
	};
	
	g_thread_init(NULL);
	
	fetch_pool = g_thread_pool_new ((GFunc)handle_url, NULL, get_nprocs()+1, FALSE, NULL );
	
	for (i = 0; i < 1; i += 1)
	{
		g_thread_pool_push(fetch_pool, (gpointer)sites[i], NULL);
	}
	
	g_thread_pool_free(fetch_pool, FALSE, TRUE);
}

int 
main (int argc, char *argv[])
{
	NspApp *app;
	gtk_init(&argc, &argv);
	g_thread_init(NULL);
	gdk_threads_init();
	
	app = nsp_app_get();
	
	gtk_main();
	
	return 0;
}



