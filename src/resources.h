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

#ifndef __AUTOZ_GUI_RESOURCES_H__
#define __AUTOZ_GUI_RESOURCES_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "commons.h"

G_BEGIN_DECLS


#define TYPE_AUTOZ_GUI_RESOURCES                 (autoz_gui_resources_get_type ())
#define AUTOZ_GUI_RESOURCES(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_AUTOZ_GUI_RESOURCES, AutozGuiResources))
#define AUTOZ_GUI_RESOURCES_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_AUTOZ_GUI_RESOURCES, AutozGuiResourcesClass))
#define IS_AUTOZ_GUI_RESOURCES(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_AUTOZ_GUI_RESOURCES))
#define IS_AUTOZ_GUI_RESOURCES_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_AUTOZ_GUI_RESOURCES))
#define AUTOZ_GUI_RESOURCES_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_AUTOZ_GUI_RESOURCES, AutozGuiResourcesClass))


typedef struct _AutozGuiResources AutozGuiResources;
typedef struct _AutozGuiResourcesClass AutozGuiResourcesClass;

struct _AutozGuiResources
	{
		GObject parent;
	};

struct _AutozGuiResourcesClass
	{
		GObjectClass parent_class;

		guint selected_signal_id;
	};

GType autoz_gui_resources_get_type (void) G_GNUC_CONST;

AutozGuiResources *autoz_gui_resources_new (AutozGuiCommons *commons, gboolean selection);

GtkWidget *autoz_gui_resources_get_widget (AutozGuiResources *resources);


G_END_DECLS

#endif /* __AUTOZ_GUI_RESOURCES_H__ */
