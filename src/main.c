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
#include <gtk/gtk.h>
#include <curl/curl.h>
#include <sys/sysinfo.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "nsp-net.h"
#include "nsp-window.h"

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
		printf("%s\n", netdata->content);
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
		"http://blogs.myspace.com/Modules/BlogV2/Pages/RssFeed.aspx?friendID=204244038",		// RSS 2.0
		"http://www.linux-bg.org/linux-bg-news.rdf",		// RSS 0.9.1
		"http://distrowatch.com/news/dwd.xml",		// RSS 2.0
		"http://fridge.ubuntu.com/node/feed",		// RSS 2.0
		"http://www.gnomefiles.org/files/gnomefiles.rdf",		// RSS 2.0
		"http://git.gnome.orgg/cgit/eog/atom/?h=master",			// xmlns='http://www.w3.org/2005/Atom'
		"http://openfest.org/feed/",		// RSS 2.0
		"http://noscope.com/feed",		// RSS 2.0
		"http://feeds.feedburner.com/d0od",		// RSS 2.0
		"http://crunchbanglinux.org/forums/feed/rss/topic/2982/"		// RSS 2.0
	};
	
	g_thread_init(NULL);
	
	fetch_pool = g_thread_pool_new ((GFunc)handle_url, NULL, get_nprocs()+1, FALSE, NULL );
	
	for (i = 0; i < 10; i += 1)
	{
		g_thread_pool_push(fetch_pool, (gpointer)sites[i], NULL);
	}
	
	g_thread_pool_free(fetch_pool, FALSE, TRUE);
	
	getchar();
}

int 
main (int argc, char *argv[])
{
	GError *error;
	gtk_init(&argc, &argv);
	
	NspWindow *win = nsp_window_new();
	
	if ( nsp_window_init(win, &error) ) {
		printf("%s\n", error->message);
		return 0;
	}
	
	gtk_widget_show_all(win->window);
	
	gtk_main();
	
	return 0;
}



