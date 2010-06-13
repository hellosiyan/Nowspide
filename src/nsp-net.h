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

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _NspNet NspNet;
typedef struct _NspNetClass NspNetClass;

#define NSP_TYPE_NET             (nsp_net_get_type ())
#define NSP_NET(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), NSP_TYPE_NET, NspNet))
#define NSP_NET_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  NSP_TYPE_NET, NspNetClass))
#define NSP_IS_NET(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NSP_TYPE_NET))
#define NSP_IS_NET_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  NSP_TYPE_NET))
#define NSP_NET_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  NSP_TYPE_NET, NspNetClass))

struct _NspNet {
    gchar* location;
};

struct _NspNetClass {
    GObjectClass parent_class;
};

GType     nsp_net_get_type (void) G_GNUC_CONST;

NspNet  *nsp_net_new      ();

G_END_DECLS

#endif /* __NSP_NET_H_ */
