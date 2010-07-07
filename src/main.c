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

static const GtkActionEntry action_entries_window[] = {
    { "File",  NULL, "_File" },
    { "Edit",  NULL, "_Edit" },
    { "Help",  NULL, "_Help" },

    { "FileAdd", GTK_STOCK_ADD, "Open _Image...", "<control>O",
      "Open an Image", NULL },
    { "FileUpdateAll", GTK_STOCK_REFRESH, "_Update all Feeds", "<control>R",
      "Open an Image",NULL },
    { "FileClose", GTK_STOCK_CLOSE, "Close", "<control>w",
      "Close Window",NULL },
      
      
    { "FileDelete", GTK_STOCK_DELETE, "Open _Image...", "<control>O",
      "Open an Image", NULL },
    { "EditPreferences", GTK_STOCK_PREFERENCES, "Preferences", NULL,
      "Open an Image",NULL },
    { "HelpAbout", GTK_STOCK_FILE, "About", "<control>O",
      "Open an Image",NULL }
};

void 
ns_build_window () {
	GtkWidget *win;
	GtkWidget *main_container;
	GtkWidget *main_menu;
	
	GtkWidget *column_holder;
	
	GtkWidget *c1_holder;
	GtkWidget *fl_title;
	
	GtkWidget *c2_holder;
	GtkWidget *f_title;
	
    GtkUIManager *ui_mng = gtk_ui_manager_new();
    GtkActionGroup *actions_window;
    
	gtk_ui_manager_add_ui_from_file(ui_mng, "ui-definition.xml", NULL);
	
    actions_window = gtk_action_group_new("MenuActionsWindow");

    gtk_action_group_add_actions (actions_window,
                                  action_entries_window,
                                  G_N_ELEMENTS (action_entries_window),
                                  NULL);

    gtk_ui_manager_insert_action_group (ui_mng,
                                        actions_window, 0);
	
	/* Set up window properties */
	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(win), 500, 300);
	g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	
	/* Set up main container */
	main_container = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(win), main_container);
	
	/* Set Up Main Menu */
	main_menu = gtk_ui_manager_get_widget(ui_mng, "/MainMenu");
	gtk_box_pack_start(GTK_BOX(main_container), main_menu, FALSE, TRUE, 0);
	
	/* Column Layout */
	column_holder = gtk_hpaned_new();
	gtk_box_pack_start(GTK_BOX(main_container), column_holder, TRUE, TRUE, 0);
	
	/* LEFT COLUMN */
	/* Holder */
	c1_holder = gtk_vbox_new(FALSE, 0);
	gtk_paned_add1(GTK_PANED(column_holder), c1_holder);
	
	
	/* header */
	fl_title = gtk_label_new("Feed List");
	gtk_box_pack_start(GTK_BOX(c1_holder), fl_title, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(fl_title), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(fl_title), 0, 52);
	
	
	/* RIGHT COLUMN */
	/* Holder */
	c2_holder = gtk_vbox_new(FALSE, 0);
	gtk_paned_add2(GTK_PANED(column_holder), c2_holder);
	
	
	/* header */
	f_title = gtk_label_new("My Feed");
	gtk_box_pack_start(GTK_BOX(c2_holder), f_title, FALSE, FALSE, 0);
	
	
	
	gtk_widget_show_all(win);
	
}

int 
main (int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	
	ns_build_window();
	
	gtk_main();
	
	return 0;
}



