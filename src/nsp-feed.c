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
#include <assert.h>
#include <stdlib.h>

NspFeed * 
nsp_feed_new()
{
	NspFeed * feed = malloc(sizeof(NspFeed));
	assert(feed != NULL);
	
	feed->title = feed->link = feed->description = NULL;
	feed->type = NSP_FEED_UNKNOWN;
	
	return feed;
}

NspFeed * 
nsp_feed_new_from_xml(xmlDoc *xml, GError **error)
{
	NspFeed * feed = nsp_feed_new();
	assert(feed != NULL);
	
	assert(error == NULL || *error == NULL);
	
	if ( nsp_feed_load_xml(feed, xml, error) ) {
		nsp_feed_free(feed);
		return NULL;
	}
	return feed;
}

void
nsp_feed_free (NspFeed *feed)
{
	if ( feed == NULL)
		return;
		
	free(feed->title);
	free(feed->link);
	free(feed->description);
}

int
nsp_feed_load_xml	(NspFeed *feed, xmlDoc *xml, GError **error)
{
	// do xml parsing here
	return 0;
}
