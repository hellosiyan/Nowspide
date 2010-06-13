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
#include <gtk/gtk.h>

G_DEFINE_TYPE (NspNet, nsp_net, G_TYPE_OBJECT);

/*************************************************************/
/***** Private actions ***************************************/
/*************************************************************/

/*************************************************************/
/***** Private signal handlers *******************************/
/*************************************************************/

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
static void
nsp_net_dispose (GObject *gobject)
{
    G_OBJECT_CLASS (nsp_net_parent_class)->dispose (gobject);
}

static void
nsp_net_class_init (NspNetClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->dispose = nsp_net_dispose;
}

NspNet *
nsp_net_new ()
{
    return g_object_new (NSP_TYPE_NET, NULL);
}

static void
nsp_net_init (NspNet *net)
{
    net->location = '\0';
}

/*************************************************************/
/***** Actions ***********************************************/
/*************************************************************/

