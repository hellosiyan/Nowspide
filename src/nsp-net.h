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


#ifndef __NSP_NET_H_
#define __NSP_NET_H_

#include <string.h>

typedef struct _NspNetData NspNetData;

struct _NspNetData
{
	char *content;
	size_t size;
	char *error;
};

NspNetData * nsp_net_new();

int 	nsp_net_load_url	(const char *url, NspNetData *netdata);
void	nsp_net_free 		(NspNetData *netdata);

#endif /* __NSP_NET_H_ */
