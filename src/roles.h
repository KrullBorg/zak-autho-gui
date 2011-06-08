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

#ifndef __ROLES_H__
#define __ROLES_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "commons.h"

G_BEGIN_DECLS


#define TYPE_ROLES                 (roles_get_type ())
#define ROLES(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_ROLES, Roles))
#define ROLES_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_ROLES, RolesClass))
#define IS_ROLES(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_ROLES))
#define IS_ROLES_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_ROLES))
#define ROLES_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_ROLES, RolesClass))


typedef struct _Roles Roles;
typedef struct _RolesClass RolesClass;

struct _Roles
	{
		GObject parent;
	};

struct _RolesClass
	{
		GObjectClass parent_class;
	};

GType roles_get_type (void) G_GNUC_CONST;

Roles *roles_new (Commons *commons, gboolean selection);

GtkWidget *roles_get_widget (Roles *roles);


G_END_DECLS

#endif /* __ROLES_H__ */
