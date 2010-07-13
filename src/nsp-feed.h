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


#ifndef __NSP_FEED_H_
#define __NSP_FEED_H_

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <gtk/gtk.h>

typedef enum {
	NSP_FEED_UNKNOWN,
	NSP_FEED_RSS_0_9_1,
	NSP_FEED_RSS_2_0
} NspFeedType;

typedef struct _NspFeedItem NspFeedItem;

struct _NspFeedItem
{
	char *title;
	char *link;
	char *description;
};

typedef struct _NspFeed NspFeed;

struct _NspFeed
{
	NspFeedType type;
	GList * items;
};

NspFeed * nsp_feed_new();
NspFeed * nsp_feed_new_from_xml(xmlDoc *xml, GError **error);

void	nsp_feed_free 		(NspFeed *feed);
int		nsp_feed_set_from_xml	(NspFeed *feed, xmlDoc *xml, GError **error);

#endif /* __NSP_FEED_H_ */
