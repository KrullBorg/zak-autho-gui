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

#ifndef __AUTOZ_GUI_ROLE_H__
#define __AUTOZ_GUI_ROLE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "commons.h"

G_BEGIN_DECLS


#define TYPE_AUTOZ_GUI_ROLE                 (autoz_gui_role_get_type ())
#define AUTOZ_GUI_ROLE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_AUTOZ_GUI_ROLE, AutozGuiRole))
#define AUTOZ_GUI_ROLE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_AUTOZ_GUI_ROLE, AutozGuiRoleClass))
#define IS_AUTOZ_GUI_ROLE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_AUTOZ_GUI_ROLE))
#define IS_AUTOZ_GUI_ROLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_AUTOZ_GUI_ROLE))
#define AUTOZ_GUI_ROLE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_AUTOZ_GUI_ROLE, AutozGuiRoleClass))


typedef struct _AutozGuiRole AutozGuiRole;
typedef struct _AutozGuiRoleClass AutozGuiRoleClass;

struct _AutozGuiRole
	{
		GObject parent;
	};

struct _AutozGuiRoleClass
	{
		GObjectClass parent_class;

		guint updated_signal_id;
	};

GType autoz_gui_role_get_type (void) G_GNUC_CONST;

AutozGuiRole *autoz_gui_role_new (AutozGuiCommons *commons, gint id);

GtkWidget *autoz_gui_role_get_widget (AutozGuiRole *role);


G_END_DECLS

#endif /* __AUTOZ_GUI_ROLE_H__ */
