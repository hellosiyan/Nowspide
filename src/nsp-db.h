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
 
#include <sqlite3.h>
#include "nsp-feed.h"

#ifndef __NSP_DB_H__
#define __NSP_DB_H__

typedef struct _NspDb NspDb;

struct _NspDb {
	sqlite3 *db;
};


NspDb * nsp_db_get();
void	nsp_db_close();

void	nsp_db_transaction_begin(NspDb *nsp_db);
void	nsp_db_transaction_end(NspDb *nsp_db);

GList * nsp_db_load_feeds(NspDb *db);

int nsp_db_add_feed(NspDb *db, NspFeed *feed, int include_items);


#endif /* __NSP_DB_H__ */
