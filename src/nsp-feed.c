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

#include "nsp-feed.h"
#include "nsp-feed-parser.h"
#include "nsp-net.h"
#include <assert.h>
#include <stdlib.h>

NspFeedItem *
nsp_feed_item_new()
{
	NspFeedItem * item = malloc(sizeof(NspFeedItem));
	assert(item != NULL);
	
	item->title = item->link = item->description = NULL;
	item->pubdate = NULL;
	
	return item;
}

void
nsp_feed_item_free(NspFeedItem * item)
{
	if ( item == NULL) {
		return;
	}
	
	free(item->title);
	free(item->link);
	free(item->description);
	free(item->pubdate);
}

NspFeed * 
nsp_feed_new()
{
	NspFeed * feed = malloc(sizeof(NspFeed));
	assert(feed != NULL);
	
	feed->type = NSP_FEED_UNKNOWN;
	feed->items = NULL;
	feed->title = feed->url = feed->description = NULL;
	return feed;
}

NspFeed * 
nsp_feed_new_from_url(const char *url, GError **error)
{
	NspNetData *data;
	xmlDoc *xml_doc;
	xmlNode *root;
	
	assert(error == NULL || *error == NULL);
	
	NspFeed * feed = nsp_feed_new();
	data = nsp_net_new();
	
	if ( nsp_net_load_url(url, data) ) {
		g_warning("ERROR: %s\n", data->error);
		nsp_feed_free(feed);
		return NULL;
	}
	
	xml_doc =  xmlReadMemory(data->content, data->size, NULL, NULL, 0);
	if ( xml_doc == NULL || !(root = xmlDocGetRootElement(xml_doc)) ) {
		g_warning("Error parsing xml!\n");
		nsp_feed_free(feed);
		return NULL;
	}
	
	feed->url = (char *) url;
	nsp_feed_parse(xml_doc, feed);
	feed->items = nsp_feed_item_parser_rss(root, NULL);
	
	return feed;
}

void
nsp_feed_free (NspFeed *feed)
{
	if ( feed == NULL)
		return;
	
	free(feed->title);
	free(feed->url);
	free(feed->description);
}



