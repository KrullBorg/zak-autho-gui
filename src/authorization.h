/* 
 * Copyright (C) 2012 Andrea Zagli <azagli@libero.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef __AUTHORIZATION_H__
#define __AUTHORIZATION_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "commons.h"

G_BEGIN_DECLS


#define TYPE_AUTHORIZATION                 (authorization_get_type ())
#define AUTHORIZATION(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_AUTHORIZATION, Authorization))
#define AUTHORIZATION_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_AUTHORIZATION, AuthorizationClass))
#define IS_AUTHORIZATION(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_AUTHORIZATION))
#define IS_AUTHORIZATION_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_AUTHORIZATION))
#define AUTHORIZATION_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_AUTHORIZATION, AuthorizationClass))


typedef struct _Authorization Authorization;
typedef struct _AuthorizationClass AuthorizationClass;

struct _Authorization
	{
		GObject parent;
	};

struct _AuthorizationClass
	{
		GObjectClass parent_class;

		guint updated_signal_id;
	};

GType authorization_get_type (void) G_GNUC_CONST;

Authorization *authorization_new (Commons *commons, gint id);

GtkWidget *authorization_get_widget (Authorization *authorization);


G_END_DECLS

#endif /* __AUTHORIZATION_H__ */
