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

#include <gtk/gtk.h>

#include "nsp-app.h"


int 
main (int argc, char *argv[])
{
	NspApp *app;
	gtk_init(&argc, &argv);
	g_thread_init(NULL);
	gdk_threads_init();
	
	app = nsp_app_get();
	
	gtk_main();
	
	g_thread_pool_free(app->jobs->pool, FALSE, TRUE);
	
	return 0;
}



