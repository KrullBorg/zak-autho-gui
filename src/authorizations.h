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

#ifndef __AUTHORIZATIONS_H__
#define __AUTHORIZATIONS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "commons.h"

G_BEGIN_DECLS


#define TYPE_AUTHORIZATIONS                 (authorizations_get_type ())
#define AUTHORIZATIONS(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_AUTHORIZATIONS, Authorizations))
#define AUTHORIZATIONS_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_AUTHORIZATIONS, AuthorizationsClass))
#define IS_AUTHORIZATIONS(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_AUTHORIZATIONS))
#define IS_AUTHORIZATIONS_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_AUTHORIZATIONS))
#define AUTHORIZATIONS_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_AUTHORIZATIONS, AuthorizationsClass))


typedef struct _Authorizations Authorizations;
typedef struct _AuthorizationsClass AuthorizationsClass;

struct _Authorizations
	{
		GObject parent;
	};

struct _AuthorizationsClass
	{
		GObjectClass parent_class;
	};

GType authorizations_get_type (void) G_GNUC_CONST;

Authorizations *authorizations_new (AutozGuiCommons *commons, gboolean selection);

GtkWidget *authorizations_get_widget (Authorizations *authorizations);


G_END_DECLS

#endif /* __AUTHORIZATIONS_H__ */
