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
 
#include "nsp-window.h"

#include "config.h"
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
static void nsp_window_cmd_item_toggle_status (GtkAction *action, gpointer user_data);
static void nsp_window_cmd_item_delete (GtkAction *action, gpointer user_data);
static gboolean nsp_window_cmd_popup_feed_item_menu (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static void nsp_window_cmd_main_menu_toggle (GtkToggleButton *button, gpointer *user_data);
static void nsp_window_cmd_main_menu_hidden (GtkWidget *widget, gpointer user_data);

const char *xml_main_menu = 
"<?xml version=\"1.0\"?><interface> <requires lib=\"gtk+\" version=\"2.16\"/>"
"<object class=\"GtkUIManager\" id=\"uiman\">"
	"<child>"
		"<object class=\"GtkActionGroup\" id=\"actiongroup\" />"
	"</child>"
	"<child>"
		"<object class=\"GtkActionGroup\" id=\"feed_item_actions\" />"
	"</child>"
	"<ui name=\"ui-manager\">"
		"<popup name=\"menubar1\" accelerators=\"true\">"
		  	"<menuitem action=\"FeedAdd\"/>"
	  		"<separator/>"
		  	"<menuitem action=\"HelpAbout\"/>"
	  		"<separator/>"
		  	"<menuitem action=\"FileClose\"/>"
		  	"<menuitem action=\"FileQuit\"/>"
		"</popup>"
		"<popup name=\"feed_item_menu\">"
		  	"<menuitem action=\"ItemToggleStatus\"/>"
		    "<separator/>"
		  	"<menuitem action=\"ItemDelete\"/>"
		"</popup>"
	"</ui>"
"</object></interface>";

const char *xml_feed_item_menu = 
"<?xml version=\"1.0\"?><interface> <requires lib=\"gtk+\" version=\"2.16\"/>"
	"<object class=\"GtkMenu\" id=\"feed_item_menu\" constructor=\"uiman\" />"
"</interface>";

static const GtkActionEntry action_entries_window[] = {
    { "FeedAdd", GTK_STOCK_ADD, "_Add Feed", NULL,
      "Add Feed", G_CALLBACK(nsp_window_cmd_add_feed) },
    { "FileClose", GTK_STOCK_CLOSE, "_Close", "<control>W",
      NULL, G_CALLBACK(gtk_main_quit) },
    { "FileQuit", GTK_STOCK_QUIT, "_Quit", "<control>Q",
      NULL, G_CALLBACK(gtk_main_quit) },
    { "HelpAbout", GTK_STOCK_ABOUT, "_About Nowspide", NULL,
      "About this application", G_CALLBACK(nsp_window_cmd_about) }
};

static const GtkActionEntry action_entries_feed_item[] = {
    { "ItemToggleStatus", GTK_STOCK_APPLY, "Mark as _Unread", NULL,
      "Toggle Read Status", G_CALLBACK(nsp_window_cmd_item_toggle_status) },
    { "ItemDelete", GTK_STOCK_DELETE, "Delete", "Delete",
      "Delete the item", G_CALLBACK(nsp_window_cmd_item_delete) }
};

NspWindow * 
nsp_window_new()
{
	NspWindow *win = malloc(sizeof(NspWindow));
	assert(win != NULL);
	
	win->builder = gtk_builder_new();
	win->feed_list = nsp_feed_list_new();
	win->feed_item_list = nsp_feed_item_list_get_view();
	win->on_feed_add = win->on_feed_update = win->on_feed_item_delete = win->on_feed_item_toggle_read = NULL;
	win->feed_item_menu = NULL;
	
	
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

	gtk_builder_add_from_string(win->builder, xml_main_menu, -1, NULL);
    
    GtkActionGroup *agroup = GTK_ACTION_GROUP(gtk_builder_get_object(win->builder, "actiongroup"));
    GtkActionGroup *feed_items_agroup = GTK_ACTION_GROUP(gtk_builder_get_object(win->builder, "feed_item_actions"));
    
    gtk_action_group_add_actions (agroup,
                                  action_entries_window,
                                  G_N_ELEMENTS (action_entries_window),
                                  NULL);
	
    gtk_action_group_add_actions (feed_items_agroup,
                                  action_entries_feed_item,
                                  G_N_ELEMENTS (action_entries_feed_item),
                                  NULL);
	
	
	gtk_builder_add_from_file(win->builder, NSP_UI_FILE, error);
	gtk_builder_add_from_string(win->builder, xml_feed_item_menu, -1, NULL);
	
	
	win->feed_item_menu = GTK_WIDGET(gtk_builder_get_object(win->builder, "feed_item_menu"));
	
    if (error != NULL)
    {
        g_warning ("%s\n", (*error)->message);
        return 1;
    }

	win->window = GTK_WIDGET (gtk_builder_get_object (win->builder, "main_window"));
    
	gtk_window_add_accel_group(GTK_WINDOW(win->window), gtk_ui_manager_get_accel_group(GTK_UI_MANAGER(gtk_builder_get_object(win->builder, "uiman"))));
	
	gtk_window_set_position(GTK_WINDOW(win->window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(win->window), 700, 400);
	g_signal_connect(win->window, "destroy", G_CALLBACK(nsp_window_destroy), NULL);
	
	gtk_container_add (GTK_CONTAINER (gtk_builder_get_object (win->builder, "scroll_win")), win->feed_list->list_view);
	gtk_container_add (GTK_CONTAINER (gtk_builder_get_object (win->builder, "feed_item_list_win")), win->feed_item_list);
	
	gtk_widget_set_size_request(GTK_WIDGET (gtk_builder_get_object (win->builder, "scroll_win")), 200, -1);
	
	gtk_menu_attach_to_widget(GTK_MENU(gtk_builder_get_object(win->builder, "menubar1")), GTK_WIDGET(gtk_builder_get_object(win->builder, "btn_main_menu")), NULL);
	
	/* Connect signals */
	g_signal_connect(gtk_builder_get_object(win->builder, "btn_update"), "clicked", G_CALLBACK(nsp_window_cmd_update_feed), win);
	g_signal_connect(gtk_builder_get_object(win->builder, "btn_main_menu"), "toggled", 
						G_CALLBACK(nsp_window_cmd_main_menu_toggle), 
						gtk_builder_get_object(win->builder, "menubar1"));
	g_signal_connect(gtk_builder_get_object(win->builder, "menubar1"), "hide", G_CALLBACK(nsp_window_cmd_main_menu_hidden), win);
	g_signal_connect(win->feed_item_list, "button-release-event", G_CALLBACK(nsp_window_cmd_popup_feed_item_menu), win);
	
	/* Show/hide specific widgets */
	gtk_widget_show(win->feed_list->list_view);
	gtk_widget_show(win->feed_item_list);
	
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
	
	if (gtk_dialog_run(dialog) == GTK_RESPONSE_OK && app->window->on_feed_add != NULL) {
		nsp_jobs_queue(app->jobs, nsp_job_new(app->window->on_feed_add, (void*)gtk_entry_get_text(GTK_ENTRY(input_area))));
	}
	
	gtk_widget_destroy(GTK_WIDGET(dialog));
}


static void
nsp_window_cmd_update_feed(GtkButton *button, gpointer user_data) 
{
	NspWindow *win = (NspWindow*) user_data;
	
	if ( win->on_feed_update != NULL) {
		win->on_feed_update(NULL);
	}
}


static void 
nsp_window_cmd_item_toggle_status (GtkAction *action, gpointer user_data)
{
	NspApp *app = nsp_app_get();
	
	if ( app->current_feed_item != NULL && app->window->on_feed_item_toggle_read != NULL) {
		app->window->on_feed_item_toggle_read(app->current_feed_item);
	}
}

static void 
nsp_window_cmd_item_delete (GtkAction *action, gpointer user_data)
{
	NspApp *app = nsp_app_get();
	
	if ( app->current_feed_item != NULL && app->window->on_feed_item_delete != NULL) {
		app->window->on_feed_item_delete(app->current_feed_item);
	}
}

static gboolean
nsp_window_cmd_popup_feed_item_menu (GtkWidget *widget, GdkEventButton *event, gpointer user_data)  
{
	NspApp *app = nsp_app_get();
	if ( event->button != 3 || app->current_feed_item == NULL || !gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget), event->x, event->y, NULL, NULL, NULL, NULL) )
		return FALSE;
	
    gtk_menu_popup(GTK_MENU(((NspWindow*)user_data)->feed_item_menu),
            NULL, NULL, NULL, NULL, event->button,
            gtk_get_current_event_time());
	
	return FALSE;
}

static void
nsp_window_main_menu_position (GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer user_data)
{
	GtkWidget *button = GTK_WIDGET(user_data);
	GdkWindow *window = gtk_widget_get_window(button);
	
	gdk_window_get_position(window, x, y);
	
	*x += 1;
	*y += button->allocation.height;
}

static void
nsp_window_cmd_main_menu_toggle (GtkToggleButton *button, gpointer *user_data)
{
	if( !gtk_toggle_button_get_active(button)) {
		return;
	}
	gtk_menu_popup(GTK_MENU(user_data), NULL, NULL, nsp_window_main_menu_position, button, 0, gtk_get_current_event_time());
	return;
}

static void
nsp_window_cmd_main_menu_hidden (GtkWidget *widget, gpointer user_data)
{
	NspWindow *win = (NspWindow*)user_data;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(win->builder, "btn_main_menu")), FALSE);
}

