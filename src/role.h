/* 
 * Copyright (C) 2011 Andrea Zagli <azagli@libero.it>
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

#ifndef __ROLE_H__
#define __ROLE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "commons.h"

G_BEGIN_DECLS


#define TYPE_ROLE                 (role_get_type ())
#define ROLE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_ROLE, Role))
#define ROLE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_ROLE, RoleClass))
#define IS_ROLE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_ROLE))
#define IS_ROLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_ROLE))
#define ROLE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_ROLE, RoleClass))


typedef struct _Role Role;
typedef struct _RoleClass RoleClass;

struct _Role
	{
		GObject parent;
	};

struct _RoleClass
	{
		GObjectClass parent_class;

		guint updated_signal_id;
	};

GType role_get_type (void) G_GNUC_CONST;

Role *role_new (Commons *commons, gint id);

GtkWidget *role_get_widget (Role *role);


G_END_DECLS

#endif /* __ROLE_H__ */
