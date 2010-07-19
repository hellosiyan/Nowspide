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
 
#include "config.h"
#include "nsp-window.h"
#include "nsp-feed-list-view.h"
#include <assert.h>
#include <stdlib.h>
 
#define NSP_UI_FILE PACKAGE_DATA_DIR"/nowspide/nowspide-ui.xml"

enum
{
	COL_NAME=0,
	COL_ITEM,
	NUM_COLS
};

static void nsp_window_cmd_about (GtkAction *action, GtkWindow *window);
static const GtkActionEntry action_entries_window[] = {
    { "File",  NULL, "_File" },
    { "Help",  NULL, "_Help" },

    { "FileClose", GTK_STOCK_CLOSE, "Close", "<control>w",
      "Close Window",NULL },
     
    { "HelpAbout", GTK_STOCK_FILE, "About", "<control>O",
      "Show About dialog", G_CALLBACK(nsp_window_cmd_about) }
};

NspWindow * 
nsp_window_new()
{
	NspWindow *win = malloc(sizeof(NspWindow));
	assert(win != NULL);
	
	win->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	win->feed_list = nsp_feed_list_view_new();
	
	
	return win;
}

void
nsp_window_free (NspWindow *win)
{
	if ( win == NULL) {
		return;
	}
	gtk_object_destroy(GTK_OBJECT(win->window));
	free(win);
}

static void
nsp_window_destroy (GtkObject *window, NspWindow *win)
{
	nsp_window_free(win);
	gtk_main_quit();
}

int 
nsp_window_init(NspWindow *win, GError **error)
{
	GtkWidget *main_container;
	GtkWidget *main_menu;
	
	GtkWidget *column_holder;
	
	GtkWidget *c1_holder;
	GtkWidget *fl_title;
	
	GtkWidget *c2_holder;
	GtkWidget *f_title;
	
	GtkWidget *scroll_win;
	
	assert(error == NULL || *error == NULL );
	
    GtkUIManager *ui_mng = gtk_ui_manager_new();
    GtkActionGroup *actions_window;
    
	gtk_ui_manager_add_ui_from_file(ui_mng, NSP_UI_FILE, error);
	
	if ( error != NULL && *error != NULL ) {
		return 1;
	}
	
    actions_window = gtk_action_group_new("MenuActionsWindow");

    gtk_action_group_add_actions (actions_window,
                                  action_entries_window,
                                  G_N_ELEMENTS (action_entries_window),
                                  NULL);

    gtk_ui_manager_insert_action_group (ui_mng,
                                        actions_window, 0);
	
	/* Set up window properties */
	gtk_window_set_position(GTK_WINDOW(win->window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(win->window), 500, 300);
	g_signal_connect(win->window, "destroy", G_CALLBACK(nsp_window_destroy), NULL);
	
	/* Set up main container */
	main_container = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(win->window), main_container);
	
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
	
	scroll_win = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll_win), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll_win), GTK_SHADOW_IN);
	
	gtk_widget_set_size_request(scroll_win, 200, -1);
	gtk_container_add (GTK_CONTAINER (scroll_win), win->feed_list->list_view);
	
	gtk_box_pack_start(GTK_BOX(c1_holder), scroll_win, TRUE, TRUE, 0);
	
	
	/* RIGHT COLUMN */
	/* Holder */
	c2_holder = gtk_vbox_new(FALSE, 0);
	gtk_paned_add2(GTK_PANED(column_holder), c2_holder);
	
	
	/* header */
	f_title = gtk_label_new("My Feed");
	gtk_box_pack_start(GTK_BOX(c2_holder), f_title, FALSE, FALSE, 0);
	
	return 0;
}

static void
nsp_window_cmd_about (GtkAction *action, GtkWindow *window)
{
    static const char *authors[] = {
        "Programming",
        "\tSiyan Panayotov <xsisqox@gmail.com>",
        NULL
    };

    char *license =
          ("Nowspide is free software: you can redistribute it and/or modify "
           "it under the terms of the GNU General Public License as published by "
           "the Free Software Foundation, either version 3 of the License, or "
           "(at your option) any later version.\n\n"
           "Viewnior is distributed in the hope that it will be useful, "
           "but WITHOUT ANY WARRANTY; without even the implied warranty of "
           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
           "GNU General Public License for more details.\n\n"
           "You should have received a copy of the GNU General Public License "
           "along with Nowspide.  If not, see <http://www.gnu.org/licenses/>.\n");

    gtk_show_about_dialog (GTK_WINDOW (window),
                   "program-name", "Nowspide",
                   "version", VERSION,
                   "copyright", "Copyright \xc2\xa9 2010 Siyan Panayotov <xsisqox@gmail.com>",
                   "comments","Handy and modern desktop feed reader",
                   "authors", authors,
                   "logo-icon-name", GTK_STOCK_INFO,
                   "wrap-license", TRUE,
                   "license", license,
                   NULL);
}
