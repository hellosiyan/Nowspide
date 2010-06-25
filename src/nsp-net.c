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

#include "nsp-net.h"
#include <assert.h>
#include <stdlib.h>
#include <curl/curl.h>

size_t nsp_net_curl_write_func (void *ptr, size_t size, size_t nmemb, void *data);

NspNetData *
nsp_net_new() {
	NspNetData *netdata = malloc(sizeof(NspNetData));
	
	assert(netdata != NULL);
	
	netdata->content = netdata->error = NULL;
	netdata->size = 0;
	
	return netdata;
}

int
nsp_net_load_url(const char *url, NspNetData *netdata) {
	CURL *c;
	CURLcode res;
	
	assert(netdata != NULL);
	
	free(netdata->content);
	netdata->content = NULL;
	netdata->size = 0;
	
	c = curl_easy_init();
	curl_easy_setopt(c, CURLOPT_URL, url);
	curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, nsp_net_curl_write_func);
	curl_easy_setopt(c, CURLOPT_WRITEDATA, (void *) netdata);
	
	res = curl_easy_perform(c);
	if ( res != 0)
	{
		netdata->error = (char *)curl_easy_strerror(res);
		curl_easy_cleanup(c);
		return 1;
	}
	
	curl_easy_cleanup(c);
	return 0;
}

void
nsp_net_free (NspNetData *netdata) {
	if ( netdata == NULL)
		return
		
	free (netdata->content);
	netdata->size = 0;
	free (netdata);
}

size_t
nsp_net_curl_write_func (void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t chunk_size = size*nmemb;
	NspNetData *string = (NspNetData *) data;
	
	string->content = realloc(string->content, string->size + chunk_size + 1);
	if ( string->content )
	{
		memcpy(&(string->content[string->size]), ptr, chunk_size);
		string->size += chunk_size;
		string->content[string->size] = 0;
	}
	
	return chunk_size;
}

