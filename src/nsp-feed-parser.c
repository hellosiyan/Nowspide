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

#define _GNU_SOURCE 1

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <gtk/gtk.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include "nsp-feed-parser.h"
#include "nsp-feed.h"

#define ATOM_0_3_URI	"http://purl.org/atom/ns#"
#define ATOM_1_0_URI	"http://www.w3.org/2005/Atom"

static NspFeedType
nsp_feed_parse_type(xmlNode *node)
{
	if (node->name && node->type == XML_ELEMENT_NODE) {
		if (strcmp((const char *)node->name, "rss")==0) {
			const char * version = (const char *)xmlGetProp(node, (const xmlChar *)"version");
			if (!version) {
				xmlFree((void *)version);
				g_warning("no RSS version");
			}
			if (strcmp(version, "0.91")==0)
				return NSP_FEED_RSS_0_9;
			else if (strcmp(version, "0.92")==0)
				return NSP_FEED_RSS_0_9;
			else if (strcmp(version, "0.94")==0)
				return NSP_FEED_RSS_0_9;
			else if (strcmp(version, "2.0")==0 || strcmp(version, "2")==0)
				return NSP_FEED_RSS_2_0;
			else {
				xmlFree((void *)version);
				g_warning("invalid RSS version");
			}
			xmlFree((void *)version);
		} else if (strcmp((const char *)node->name, "RDF")==0) {
			return NSP_FEED_RSS_1_0;
		} else if (strcmp((const char *)node->name, "feed")==0) {
			if (node->ns && node->ns->href) {
				if (strcmp((const char *)node->ns->href, ATOM_0_3_URI)==0) {
					return NSP_FEED_ATOM_0_3;
				} else if (strcmp((const char *)node->ns->href, ATOM_1_0_URI)==0) {
					return NSP_FEED_ATOM_1_0;
				} else {
					const char * version = (const char *)xmlGetProp(node, (const xmlChar *)"version");
					if (!version) {
						xmlFree((void *)version);
						g_warning("invalid Atom version");
					} else if (strcmp(version, "0.3")==0) {
						xmlFree((void *)version);
						return NSP_FEED_ATOM_0_3;
					} else {
						xmlFree((void *)version);
						g_warning("invalid Atom version");
						return NSP_FEED_UNKNOWN;
					}
				}
			} else {
				g_warning("no Atom version");
			}
		}
	} else {
		g_warning("erorrrs");
	}
	
	return NSP_FEED_UNKNOWN;
}


static int
nsp_feed_parse_rss(xmlNode *node, NspFeed *feed)
{
	xmlNode *tmp = node->children;
	
	while ( tmp != NULL ) {
		if ( !xmlStrcasecmp(tmp->name, (xmlChar *)"channel") ) {
			tmp = tmp->children;
			
			while ( tmp != NULL ) {
				if ( !xmlStrcasecmp(tmp->name, (xmlChar *) "title") ) {
					feed->title = (char *) xmlNodeGetContent(tmp);
				} else if ( !xmlStrcasecmp(tmp->name, (xmlChar *) "description") ) {
					feed->description = (char *) xmlNodeGetContent(tmp);
				}
				
				tmp = tmp->next;
			}
			
			break; // parse only one channel per feed
		}
		tmp = tmp->next;
	}
	return 0;
}


GList *
nsp_feed_item_parser_rss (xmlDoc *xml, GError **error) {
	xmlNode *root = NULL;
	xmlNode *tpm = NULL;
	xmlNode *item = NULL;
	xmlNode *prop = NULL;
	
	GList *items = NULL;
	
	assert(xml != NULL);
	root = xmlDocGetRootElement(xml);
	
	tpm = root->children;
	while( tpm != NULL ) {
		if ( !xmlStrcasecmp(tpm->name, (xmlChar *)"channel") ) {
			item = tpm->children;
			while ( item != NULL) {
				if ( strcmp((char *)item->name, "item") ) {
					item = item->next;
					continue;
				}
				NspFeedItem *feed_item = nsp_feed_item_new();
				
				prop = item->children;
				while ( prop != NULL ) {
					if ( !xmlStrcasecmp(prop->name, (xmlChar *)"title") ) {
						feed_item->title = (char*) xmlNodeGetContent(prop);
					} else if( !xmlStrcasecmp(prop->name, (xmlChar *)"link") ) {
						feed_item->link = (char*) xmlNodeGetContent(prop);
					} else if( !xmlStrcasecmp(prop->name, (xmlChar *)"description") ) {
						feed_item->description = (char*) xmlNodeGetContent(prop);
					} else if( !xmlStrcasecmp(prop->name, (xmlChar *)"pubdate") ) {
						if ( feed_item->pubdate == NULL ) {
							feed_item->pubdate = malloc(sizeof(struct tm));
							assert(feed_item->pubdate != NULL);
							feed_item->pubdate->tm_zone = NULL;
						}
						strptime((const char*) xmlNodeGetContent(prop), "%a, %d %b %Y %H:%M:%S %z", feed_item->pubdate);
					}
					prop = prop->next;
				}
			
				items = g_list_prepend(items, (gpointer)feed_item);
				
				item = item->next;
			}
			break; // parse only one channel per feed
		}
		tpm = tpm->next;
	}
	
	return items;
}


int
nsp_feed_parse (xmlDoc *xml, NspFeed *feed)
{
	xmlNode *node = NULL;
	
	assert(xml != NULL);
	node = xmlDocGetRootElement(xml);

	feed->type = nsp_feed_parse_type(node);
	
	switch( feed->type ) {
		case NSP_FEED_RSS_2_0:
		case NSP_FEED_RSS_1_0:
		case NSP_FEED_RSS_0_9:
			nsp_feed_parse_rss(node, feed);
			break;
		default:
			break;
	}

	return 0;
}


