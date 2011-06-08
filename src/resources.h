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

#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "commons.h"

G_BEGIN_DECLS


#define TYPE_RESOURCES                 (resources_get_type ())
#define RESOURCES(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_RESOURCES, Resources))
#define RESOURCES_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_RESOURCES, ResourcesClass))
#define IS_RESOURCES(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_RESOURCES))
#define IS_RESOURCES_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_RESOURCES))
#define RESOURCES_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_RESOURCES, ResourcesClass))


typedef struct _Resources Resources;
typedef struct _ResourcesClass ResourcesClass;

struct _Resources
	{
		GObject parent;
	};

struct _ResourcesClass
	{
		GObjectClass parent_class;
	};

GType resources_get_type (void) G_GNUC_CONST;

Resources *resources_new (Commons *commons, gboolean selection);

GtkWidget *resources_get_widget (Resources *resources);


G_END_DECLS

#endif /* __RESOURCES_H__ */
