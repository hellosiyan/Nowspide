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
 
#include "nsp-webview.h"

#include <assert.h>
#include <stdlib.h>

#define CSS_PATH "file://"PACKAGE_DATA_DIR"/nowspide/style.css"


static void nsp_webview_cmd_load_status (WebKitWebView *web_view, GParamSpec* pspec, gpointer user_data);
static void nsp_webview_cmd_go_back (GtkButton *button, gpointer user_data);
static gboolean nsp_webview_cmd_navigate (WebKitWebView *web_view, WebKitWebFrame *frame, WebKitNetworkRequest *request, WebKitWebNavigationAction *navigation_action, WebKitWebPolicyDecision *policy_decision, gpointer user_data);
static gboolean nsp_webview_cmd_new_window (WebKitWebView *web_view, WebKitWebFrame *frame, WebKitNetworkRequest *request, WebKitWebNavigationAction *navigation_action, WebKitWebPolicyDecision *policy_decision, gpointer user_data);

NspWebview * 
nsp_webview_new()
{
	NspWebview *webview = malloc(sizeof(NspWebview));
	assert(webview != NULL);
	
	webview->status = NSP_WEBVIEW_OFFLINE;
	
	webview->webkit_webview = (WebKitWebView*) webkit_web_view_new();
	webview->btn_view_switch = webview->btn_back = webview->spinner = NULL;
	
	return webview;
}

int 
nsp_webview_init(NspWebview *webview)
{
	WebKitWebSettings *settings = webkit_web_view_get_settings(webview->webkit_webview);
	
	g_object_set(G_OBJECT(settings), "default-font-size", 10, "enable-html5-database", FALSE, "enable-html5-local-storage", FALSE, "enable-java-applet", FALSE, "enable-offline-web-application-cache", FALSE, "enable-plugins", FALSE, "resizable-text-areas", FALSE, "enable-scripts", FALSE, "user-stylesheet-uri", CSS_PATH, "enable-private-browsing", TRUE, NULL);
	
	g_signal_connect(webview->webkit_webview, "notify::load-status", G_CALLBACK(nsp_webview_cmd_load_status), webview);
	g_signal_connect(webview->webkit_webview, "navigation-policy-decision-requested", G_CALLBACK(nsp_webview_cmd_navigate), webview);
	g_signal_connect(webview->webkit_webview, "new-window-policy-decision-requested", G_CALLBACK(nsp_webview_cmd_new_window), webview);
	
	
	if ( webview->btn_back != NULL ) {
		g_signal_connect(webview->btn_back, "clicked", G_CALLBACK(nsp_webview_cmd_go_back), webview);
	}
	
	return 0;
}

void 
nsp_webview_load_string(NspWebview *webview, const char *string)
{
	GtkWidget *btn_image = gtk_bin_get_child(GTK_BIN(webview->btn_view_switch));
	
	webkit_web_view_load_string (webview->webkit_webview, string, "text/html", "UTF-8", "");
	webkit_web_back_forward_list_clear(webkit_web_view_get_back_forward_list(webview->webkit_webview));
	
	gtk_image_set_from_stock(GTK_IMAGE(btn_image), "gtk-disconnect", GTK_ICON_SIZE_BUTTON);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(webview->btn_view_switch), false);
	
	
	webview->status = NSP_WEBVIEW_OFFLINE;
}

void
nsp_webview_load_url(NspWebview *webview, const char *url)
{
	GtkWidget *btn_image = gtk_bin_get_child(GTK_BIN(webview->btn_view_switch));
	
	webkit_web_view_load_uri(webview->webkit_webview, url);	
	
	if ( webview->status == NSP_WEBVIEW_OFFLINE ) {
		webview->status = NSP_WEBVIEW_ONLINE;
		
		gtk_image_set_from_stock(GTK_IMAGE(btn_image), "gtk-connect", GTK_ICON_SIZE_BUTTON);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(webview->btn_view_switch), true);
		
		webkit_web_back_forward_list_clear(webkit_web_view_get_back_forward_list(webview->webkit_webview));
	}
}

static void
nsp_webview_cmd_load_status (WebKitWebView *web_view, GParamSpec* pspec, gpointer user_data) 
{
	NspWebview *webview = (NspWebview*)user_data;
	assert(webview != NULL);
	
	WebKitLoadStatus load_status = webkit_web_view_get_load_status(web_view);
	switch(load_status) {
		case WEBKIT_LOAD_FINISHED:
		case WEBKIT_LOAD_FAILED:
			gtk_widget_hide(webview->spinner);
			if ( webview->status == NSP_WEBVIEW_ONLINE && webview->btn_back != NULL ) {
				gtk_widget_set_sensitive(webview->btn_back, webkit_web_view_can_go_back(web_view));
				gtk_widget_show(webview->btn_back);
			}
			break;
		default:
			gtk_widget_show(webview->spinner);
			gtk_widget_hide(webview->btn_back);
	}
}

static void
nsp_webview_cmd_go_back (GtkButton *button, gpointer user_data) 
{
	NspWebview *webview = (NspWebview*)user_data;
	assert(webview != NULL);
	
	webkit_web_view_go_back (webview->webkit_webview);
}

static gboolean 
nsp_webview_cmd_navigate (WebKitWebView *web_view, WebKitWebFrame *frame, WebKitNetworkRequest *request, WebKitWebNavigationAction *navigation_action, WebKitWebPolicyDecision *policy_decision, gpointer user_data)
{
	NspWebview *webview = (NspWebview*)user_data;
	GtkWidget *btn_image;
	assert(webview != NULL);
	
	webkit_web_policy_decision_use (policy_decision);
	
	if ( webview->status == NSP_WEBVIEW_ONLINE ) {
		return TRUE;
	}
	
	/* Setup environment when navigating from offline mode */
	btn_image = gtk_bin_get_child(GTK_BIN(webview->btn_view_switch));
	gtk_image_set_from_stock(GTK_IMAGE(btn_image), "gtk-connect", GTK_ICON_SIZE_BUTTON);
	
	webview->status = NSP_WEBVIEW_ONLINE;
	
	webkit_web_back_forward_list_clear(webkit_web_view_get_back_forward_list(webview->webkit_webview));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(webview->btn_view_switch), TRUE);
	
	return TRUE;
}

static gboolean 
nsp_webview_cmd_new_window (WebKitWebView *web_view, WebKitWebFrame *frame, WebKitNetworkRequest *request, WebKitWebNavigationAction *navigation_action, WebKitWebPolicyDecision *policy_decision, gpointer user_data)
{
	NspWebview *webview = (NspWebview*)user_data;
	assert(webview != NULL);
	
	/* Ignore the new window request and load the uri into the current view */
	webkit_web_policy_decision_ignore (policy_decision);
	nsp_webview_load_url(webview, webkit_network_request_get_uri(request));
	return true;
}
