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

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <gtk/gtk.h>
#include <assert.h>
#include <string.h>

#include "nsp-feed-parser.h"
#include "nsp-feed.h"


int 
nsp_feed_parser_rss_2_0 (xmlDoc *xml, GError **error) {
	xmlNode *root = NULL;
	xmlNode *tpm = NULL;
	xmlNode *item = NULL;
	xmlNode *prop = NULL;
	
	assert(xml != NULL);
	root = xmlDocGetRootElement(xml);
	
	tpm = root->children;
	while( tpm != NULL ) {
		if ( !xmlStrcasecmp(tpm->name, (xmlChar *)"channel")) {
			item = tpm->children;
			while ( item != NULL) {
				if ( strcmp((char *)item->name, "item") ) {
					item = item->next;
					continue;
				}
				NspFeedItem feed_item;
				
				prop = item->children;
				while ( prop != NULL ) {
					if ( !xmlStrcasecmp(prop->name, (xmlChar *)"title") ) {
						feed_item.title = (char*) xmlNodeGetContent(prop);
					} else if( !xmlStrcasecmp(prop->name, (xmlChar *)"link") ) {
						feed_item.link = (char*) xmlNodeGetContent(prop);
					} else if( !xmlStrcasecmp(prop->name, (xmlChar *)"description") ) {
						feed_item.description = (char*) xmlNodeGetContent(prop);
					}
					prop = prop->next;
				}
			
				printf("Name: %s\nLink: %s\nDescription: %s\n#########\n", feed_item.title, feed_item.link, feed_item.description);
				feed_item.title = feed_item.link = feed_item.description = NULL;
				
				item = item->next;
			}
			break; // parse only one channel per feed
		}
		tpm = tpm->next;
	}
	
	return 0;
}




