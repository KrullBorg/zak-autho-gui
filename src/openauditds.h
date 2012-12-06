/* 
 * Copyright (C) 2010-2011 Andrea Zagli <azagli@libero.it>
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

#ifndef __OPEN_AUDIT_DS_H__
#define __OPEN_AUDIT_DS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "commons.h"

G_BEGIN_DECLS


#define TYPE_OPEN_AUDIT_DS                 (open_audit_ds_get_type ())
#define OPEN_AUDIT_DS(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_OPEN_AUDIT_DS, OpenAuditDS))
#define OPEN_AUDIT_DS_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_OPEN_AUDIT_DS, OpenAuditDSClass))
#define IS_OPEN_AUDIT_DS(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_OPEN_AUDIT_DS))
#define IS_OPEN_AUDIT_DS_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_OPEN_AUDIT_DS))
#define OPEN_AUDIT_DS_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_OPEN_AUDIT_DS, OpenAuditDSClass))


typedef struct _OpenAuditDS OpenAuditDS;
typedef struct _OpenAuditDSClass OpenAuditDSClass;

struct _OpenAuditDS
	{
		GObject parent;
	};

struct _OpenAuditDSClass
	{
		GObjectClass parent_class;

		guint opened_signal_id;
	};

GType open_audit_ds_get_type (void) G_GNUC_CONST;

OpenAuditDS *open_audit_ds_new (AutozGuiCommons *commons);

GtkWidget *open_audit_ds_get_widget (OpenAuditDS *open_audit_ds);


G_END_DECLS

#endif /* __OPEN_AUDIT_DS_H__ */
