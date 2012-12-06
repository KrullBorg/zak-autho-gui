/* 
 * Copyright (C) 2011-2012 Andrea Zagli <azagli@libero.it>
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

#ifndef __AUTOZ_GUI_RESOURCE_H__
#define __AUTOZ_GUI_RESOURCE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "commons.h"

G_BEGIN_DECLS


#define TYPE_AUTOZ_GUI_RESOURCE                 (autoz_gui_resource_get_type ())
#define AUTOZ_GUI_RESOURCE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_AUTOZ_GUI_RESOURCE, AutozGuiResource))
#define AUTOZ_GUI_RESOURCE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_AUTOZ_GUI_RESOURCE, AutozGuiResourceClass))
#define IS_AUTOZ_GUI_RESOURCE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_AUTOZ_GUI_RESOURCE))
#define IS_AUTOZ_GUI_RESOURCE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_AUTOZ_GUI_RESOURCE))
#define AUTOZ_GUI_RESOURCE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_AUTOZ_GUI_RESOURCE, AutozGuiResourceClass))


typedef struct _AutozGuiResource AutozGuiResource;
typedef struct _AutozGuiResourceClass AutozGuiResourceClass;

struct _AutozGuiResource
	{
		GObject parent;
	};

struct _AutozGuiResourceClass
	{
		GObjectClass parent_class;

		guint updated_signal_id;
	};

GType autoz_gui_resource_get_type (void) G_GNUC_CONST;

AutozGuiResource *autoz_gui_resource_new (AutozGuiCommons *commons, gint id);

GtkWidget *autoz_gui_resource_get_widget (AutozGuiResource *resource);


G_END_DECLS

#endif /* __AUTOZ_GUI_RESOURCE_H__ */
