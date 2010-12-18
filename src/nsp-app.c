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

#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>

static NspApp *_app = NULL;

static void
nsp_app_feed_item_list_sel (GtkTreeSelection *selection, gpointer user_data)
{
	NspApp *app = nsp_app_get();
	GtkTreeIter iter, child_iter;
	GtkTreeModel *model;
	NspFeedItem *feed_item = NULL;
	
	if ( gtk_tree_selection_get_selected(selection, &model, &iter) ) {
		gtk_tree_model_get(model, &iter, ITEM_LIST_COL_ITEM_REF, &feed_item, -1);
	}
	
	if ( feed_item == NULL ) {
		return;
	}
	app->current_feed_item = feed_item;
	
	g_mutex_lock(app->current_feed->mutex);
	gtk_tree_model_sort_convert_iter_to_child_iter(GTK_TREE_MODEL_SORT(app->current_feed->items_sorter), &child_iter, &iter);
	
	if ( feed_item->status & NSP_FEED_ITEM_UNREAD ) {
		feed_item->status ^= NSP_FEED_ITEM_UNREAD;
		nsp_jobs_queue(app->jobs, nsp_job_new((NspCallback*)nsp_feed_item_save_status_to_db, (void*)feed_item));
		nsp_feed_item_list_update_iter(child_iter, app->current_feed->items_store, feed_item);
		
		app->current_feed->unread_items --;
		nsp_feed_list_update_entry(app->window->feed_list,  app->current_feed);
	}
	g_mutex_unlock(app->current_feed->mutex);
	
	nsp_webview_load_string (app->window->webview, feed_item->description);
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(app->window->builder, "feed_item_label")), feed_item->title);
	
	return;
}

static void 
nsp_app_feed_update_real (void* user_data)
{
	NspApp *app = nsp_app_get();
	NspFeed *feed = (NspFeed*)user_data;
	GtkTreeIter iter;
	
	nsp_feed_list_search(app->window->feed_list, feed, &iter);
	
	gtk_tree_store_set (GTK_TREE_STORE(app->window->feed_list->list_model), &iter,
					LIST_COL_ICON, app->window->feed_list->icon_load,
					-1);
	
	nsp_feed_update_items(feed);
	
	GDK_THREADS_ENTER();
	nsp_feed_update_model(feed);
	nsp_feed_list_update_entry(app->window->feed_list, feed);
	GDK_THREADS_LEAVE();
}

static void 
nsp_app_feed_update(void* user_data)
{
	NspApp *app = nsp_app_get();
	
	nsp_jobs_queue(app->jobs, nsp_job_new((NspCallback*)nsp_app_feed_update_real, user_data));
}

static void 
nsp_app_feeds_update(void* user_data)
{
	NspApp *app = nsp_app_get();
	GList *feeds = app->feeds;
	
	while ( feeds != NULL ) {
		nsp_jobs_queue(app->jobs, nsp_job_new((NspCallback*)nsp_app_feed_update_real, feeds->data));
		feeds = feeds->next;
	}
}

static void 
nsp_app_feed_item_delete(void* user_data)
{
	NspApp *app = nsp_app_get();
	NspFeedItem *feed_item = (NspFeedItem*)user_data;
	gboolean foo; /* required by g_signal_emit_by_name */
	
	gtk_widget_grab_focus (GTK_WIDGET (app->window->feed_item_list));
	g_signal_emit_by_name (GTK_TREE_VIEW(app->window->feed_item_list), "move-cursor", GTK_MOVEMENT_DISPLAY_LINES, 1, &foo);
		
	nsp_feed_delete_item(app->current_feed, feed_item);
}

static void
nsp_app_feed_item_toggle_read(void* user_data)
{
	NspApp *app = nsp_app_get();
	NspFeedItem *feed_item = (NspFeedItem*)user_data;
	GtkTreeIter iter;
	
	if ( feed_item->status & NSP_FEED_ITEM_UNREAD ) {
		return;
	}
	
	feed_item->status |= NSP_FEED_ITEM_UNREAD;
	
	if( nsp_feed_item_list_search(GTK_TREE_MODEL(app->current_feed->items_store), feed_item, &iter) ) {
		nsp_feed_item_list_update_iter( iter, app->current_feed->items_store, feed_item);
		nsp_feed_item_save_status_to_db(feed_item);
		app->current_feed->unread_items ++;
		nsp_feed_list_update_entry(app->window->feed_list,  app->current_feed);
	} 
}


static void
nsp_app_load_feeds(NspApp *app)
{
	GList *feeds;
	assert(app->feeds == NULL);
	feeds = app->feeds = nsp_feed_load_feeds_from_db();
	
	while ( feeds != NULL ) {
		nsp_feed_load_items_from_db((NspFeed*) feeds->data);
		nsp_feed_update_model((NspFeed*) feeds->data);
		nsp_feed_list_add(app->window->feed_list, (NspFeed*) feeds->data);
		
		feeds = feeds->next;
	}
}


static void
nsp_app_window_show(NspApp *app)
{
	gtk_widget_show(app->window->window);
}

static void
nsp_app_feed_list_select (void* user_data)
{
	NspApp *app = nsp_app_get();
	NspFeed *feed = (NspFeed*) user_data;
	GtkWidget *feed_item_header;
	
	app->current_feed = feed;
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(app->window->feed_item_list), nsp_feed_get_items_model(feed));
	
	feed_item_header = GTK_WIDGET(gtk_builder_get_object(app->window->builder, "feed_item_list_header"));
	gtk_label_set_text(GTK_LABEL(feed_item_header), feed->title);
}

static void
nsp_app_feed_add (void* user_data)
{
	NspApp *app = nsp_app_get();
	char *url = g_strdup((const char*) user_data);
	NspFeed *feed;
	GtkTreeIter iter;
	char *new_feed_title = "New Subscription";
	
	/* Create the new feed, populate it and add it to the list */
	feed = nsp_feed_new();
	feed->url = url;
	feed->title = malloc(sizeof(char)*(strlen(new_feed_title) + 1));
	memcpy(feed->title, new_feed_title, sizeof(char)*(strlen(new_feed_title)+1));
	
	GDK_THREADS_ENTER();
	iter = nsp_feed_list_add(app->window->feed_list, feed);
	gtk_tree_store_set (GTK_TREE_STORE(app->window->feed_list->list_model), &iter,
					LIST_COL_ICON, app->window->feed_list->icon_load,
					-1);
	GDK_THREADS_LEAVE();
	
	/* Fetch and save the items */
	if ( nsp_feed_update_items(feed) || nsp_feed_save_to_db(feed) ) {
		nsp_feed_list_remove(app->window->feed_list, feed);
		nsp_feed_free(feed);
		return;
	}
	
	app->feeds = g_list_append(app->feeds, feed);
	GDK_THREADS_ENTER();
	nsp_feed_update_model(feed);
	nsp_feed_update_unread_count(feed);
	nsp_feed_list_update_entry(app->window->feed_list, feed);
	GDK_THREADS_LEAVE();
	
	if ( !nsp_feed_update_icon(feed) ) {
		GDK_THREADS_ENTER();
		nsp_feed_list_update_entry(app->window->feed_list, feed);
		GDK_THREADS_LEAVE();
	}
}

static void 
nsp_app_window_init(NspApp *app)
{
	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(app->window->feed_item_list));
	
	g_signal_connect(selection, "changed", G_CALLBACK(nsp_app_feed_item_list_sel), NULL);	
}


static NspApp *
nsp_app_new ()
{
	NspApp *app;
	app = malloc(sizeof(NspApp));
	
	assert(app != NULL);
	
	app->feeds = NULL;
	app->current_feed = NULL;
	app->current_feed_item = NULL;
	
	app->db = nsp_db_get();
	
	app->jobs = nsp_jobs_new();
	
	app->window = nsp_window_new();
	nsp_window_init(app->window, NULL);
	nsp_app_window_init(app);
	
	app->window->on_feeds_add = nsp_app_feed_add;
	app->window->on_feeds_update = nsp_app_feeds_update;
	app->window->on_feed_item_delete = nsp_app_feed_item_delete;
	app->window->on_feed_item_toggle_read = nsp_app_feed_item_toggle_read;
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

void 
nsp_app_cmd_feed_read_all (GtkAction *action, gpointer user_data)
{
	NspApp *app = nsp_app_get();
	NspFeed *feed = (NspFeed*)user_data;
	
	if ( feed == NULL ) {
		feed = app->current_feed;
	}
	
	g_mutex_lock(feed->mutex);
	
	nsp_feed_read_all(feed);
	feed->unread_items = 0;
	nsp_feed_load_items_from_db(feed);
	
	g_mutex_unlock(feed->mutex);
	
	nsp_feed_update_model(feed);
	nsp_feed_list_update_entry(app->window->feed_list, feed);
}

void 
nsp_app_cmd_feed_update (GtkAction *action, gpointer user_data)
{
	NspApp *app = nsp_app_get();
	nsp_app_feed_update(app->current_feed);
}

void 
nsp_app_cmd_feed_delete (GtkAction *action, gpointer user_data)
{
	NspApp *app = nsp_app_get();
	NspFeed *feed = (NspFeed*)user_data;
    GtkWidget *dialog_confirm = NULL;
    gchar *markup, *prompt, *warning;
	
	if ( feed == NULL ) {
		feed = app->current_feed;
	}
	
    warning = "If you delete a feed, all items will be permanently lost.";

    /* I18N: The '%s' is replaced with the name of the file to be deleted. */
    prompt = g_strdup_printf ("Are you sure you want to delete\n\"%s\"?",
                              feed->title);
    markup = g_strdup_printf ("<span weight=\"bold\" size=\"larger\">%s</span>\n\n%s",
                              prompt, warning);


    dialog_confirm = gtk_message_dialog_new(GTK_WINDOW(app->window->window),
                                 GTK_DIALOG_MODAL,
                                 GTK_MESSAGE_WARNING,
                                 GTK_BUTTONS_NONE,
                                 NULL);

    gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog_confirm),
                                   markup);

    gtk_dialog_add_buttons (GTK_DIALOG (dialog_confirm),
                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                            GTK_STOCK_DELETE, GTK_RESPONSE_YES,
                            NULL);
	
	
	if ( gtk_dialog_run(GTK_DIALOG(dialog_confirm)) == GTK_RESPONSE_YES ) {
		app->feeds = g_list_remove(app->feeds, feed);
		nsp_feed_delete(feed);
		nsp_feed_update_model(feed);
		nsp_feed_list_remove(app->window->feed_list, feed);
	}
	
    g_free(prompt);
    g_free(markup);
    gtk_widget_destroy( dialog_confirm );
}

