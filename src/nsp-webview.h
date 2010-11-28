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


#ifndef __NSP_WEBVIEW_H_
#define __NSP_WEBVIEW_H_ 1

#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>

#include "nsp-typedefs.h"
#include "nsp-feed-list.h"
#include "nsp-feed-item-list.h"

typedef struct _NspWebview NspWebview;

typedef enum {
	NSP_WEBVIEW_OFFLINE,
	NSP_WEBVIEW_ONLINE
} NspWebviewStatus;

struct _NspWebview
{
	WebKitWebView* webkit_webview;
	
	NspWebviewStatus status;
	
	GtkWidget *btn_view_switch;
	GtkWidget *btn_back;
	GtkWidget *spinner;
};

NspWebview * nsp_webview_new();

int 	nsp_webview_init (NspWebview *webview);

void nsp_webview_load_string(NspWebview *webview, const char *string);
void nsp_webview_load_url(NspWebview *webview, const char *url);


#endif /* __NSP_WEBVIEW_H_ */
