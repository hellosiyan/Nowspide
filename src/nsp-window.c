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
#include "nsp-feed-list.h"
#include "nsp-jobs.h"
#include "nsp-app.h"
#include <assert.h>
#include <stdlib.h>
 
#define NSP_UI_FILE PACKAGE_DATA_DIR"/nowspide/nowspide.ui"

enum
{
	COL_NAME=0,
	COL_ITEM,
	NUM_COLS
};

static void nsp_window_cmd_about (GtkAction *action, GtkWindow *window);
static void nsp_window_cmd_add_feed(GtkButton *button, gpointer user_data);
static void nsp_window_cmd_update_feed (GtkButton *button, gpointer user_data);

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
	
	win->builder = gtk_builder_new();
	win->feed_list = nsp_feed_list_new();
	win->feed_item_list = nsp_feed_item_list_get_view();
	win->on_feed_add = win->on_feed_update = NULL;
	
	
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
	gtk_builder_add_from_file(win->builder, NSP_UI_FILE, error);
	
    if (error != NULL)
    {
        g_warning ("%s\n", (*error)->message);
        return 1;
    }

	win->window = GTK_WIDGET (gtk_builder_get_object (win->builder, "main_window"));
	
	gtk_window_set_position(GTK_WINDOW(win->window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(win->window), 700, 400);
	g_signal_connect(win->window, "destroy", G_CALLBACK(nsp_window_destroy), NULL);
	
	gtk_container_add (GTK_CONTAINER (gtk_builder_get_object (win->builder, "scroll_win")), win->feed_list->list_view);
	gtk_container_add (GTK_CONTAINER (gtk_builder_get_object (win->builder, "feed_item_list_win")), win->feed_item_list);
	
	gtk_widget_set_size_request(GTK_WIDGET (gtk_builder_get_object (win->builder, "scroll_win")), 200, -1);
	
	/* Connect signals */
	g_signal_connect(gtk_builder_get_object(win->builder, "btn_feed_add"), "clicked", G_CALLBACK(nsp_window_cmd_add_feed), win);
	g_signal_connect(gtk_builder_get_object(win->builder, "btn_update"), "clicked", G_CALLBACK(nsp_window_cmd_update_feed), win);
	
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

static void 
nsp_window_cmd_add_feed(GtkButton *button, gpointer user_data)
{
	NspApp *app = nsp_app_get();
	NspWindow *win = (NspWindow*)user_data;
	GtkDialog *dialog = (GtkDialog*)gtk_dialog_new();
	GtkWidget *dialog_content = gtk_dialog_get_content_area(dialog);
	GtkWidget *input_area = gtk_entry_new();
	GtkWidget *alignment = NULL;
	GtkWidget *label = gtk_label_new(NULL);
		
	gtk_label_set_markup (GTK_LABEL (label), "<b>Feed Source</b>\n<small>Enter the URL of the feed.</small>");
	
	alignment = gtk_alignment_new(0, 0, 0, 0);
	gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 5, 5, 5, 5);
	gtk_container_add (GTK_CONTAINER (alignment), label);
	gtk_box_pack_start (GTK_BOX (dialog_content), alignment, FALSE, FALSE, 0);
	
	alignment = gtk_alignment_new(0, 0, 0, 0);
	gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 5, 5, 5);
	gtk_widget_set_size_request(input_area, 250, -1);
	gtk_container_add (GTK_CONTAINER (alignment), input_area);
	gtk_box_pack_start (GTK_BOX (dialog_content), alignment, FALSE, FALSE, 0);
		
	gtk_widget_show_all(GTK_WIDGET(dialog_content));
	gtk_dialog_add_buttons(dialog, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_ADD, GTK_RESPONSE_OK, NULL);
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	
	if (gtk_dialog_run(dialog) == GTK_RESPONSE_OK && win->on_feed_add != NULL) {
		nsp_jobs_queue(app->jobs, nsp_job_new(win->on_feed_add, (void*)gtk_entry_get_text(GTK_ENTRY(input_area))));
	}
	
	gtk_widget_destroy(GTK_WIDGET(dialog));
}


static void
nsp_window_cmd_update_feed(GtkButton *button, gpointer user_data) 
{
	NspWindow *win = (NspWindow*) user_data;
	GtkTreeIter iter;
	GtkTreeModel *model;
	NspFeed *feed = NULL;
	
	if ( gtk_tree_selection_get_selected(gtk_tree_view_get_selection(GTK_TREE_VIEW(win->feed_list->list_view)), &model, &iter) ) {
		gtk_tree_model_get(model, &iter, LIST_COL_FEED_REF, &feed, -1);
	}
	
	if ( feed != NULL && win->on_feed_update != NULL) {
		win->on_feed_update(feed);
	}
}

