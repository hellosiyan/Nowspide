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
#include <time.h>

#include "nsp-feed.h"

#ifndef __NSP_PARSERS_H__
#define __NSP_PARSERS_H__

GList * nsp_feed_item_parser_rss (xmlNode *xml, GError **error);
int nsp_feed_parse (xmlDoc *xml, NspFeed *feed);


#endif /* __NSP_PARSERS_H__ */



