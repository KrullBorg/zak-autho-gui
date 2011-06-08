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

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "commons.h"

G_BEGIN_DECLS


#define TYPE_RESOURCE                 (resource_get_type ())
#define RESOURCE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_RESOURCE, Resource))
#define RESOURCE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_RESOURCE, ResourceClass))
#define IS_RESOURCE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_RESOURCE))
#define IS_RESOURCE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_RESOURCE))
#define RESOURCE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_RESOURCE, ResourceClass))


typedef struct _Resource Resource;
typedef struct _ResourceClass ResourceClass;

struct _Resource
	{
		GObject parent;
	};

struct _ResourceClass
	{
		GObjectClass parent_class;

		guint updated_signal_id;
	};

GType resource_get_type (void) G_GNUC_CONST;

Resource *resource_new (Commons *commons, gint id);

GtkWidget *resource_get_widget (Resource *resource);


G_END_DECLS

#endif /* __RESOURCE_H__ */
