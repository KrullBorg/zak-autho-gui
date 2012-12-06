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

#include <libgda-ui/libgda-ui.h>

#include "openauditds.h"

static void autoz_gui_open_audit_ds_class_init (AutozGuiOpenAuditDSClass *klass);
static void autoz_gui_open_audit_ds_init (AutozGuiOpenAuditDS *open_audit_ds);

static void autoz_gui_open_audit_ds_set_property (GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec);
static void autoz_gui_open_audit_ds_get_property (GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

static void autoz_gui_open_audit_ds_on_wlogin_changed (GdauiLogin *gdauilogin,
                                             gboolean arg1,
                                             gpointer user_data);

static void autoz_gui_open_audit_ds_on_btn_cancel_clicked (GtkButton *button,
                                    gpointer user_data);
static void autoz_gui_open_audit_ds_on_btn_open_clicked (GtkButton *button,
                                  gpointer user_data);

#define AUTOZ_GUI_OPEN_AUDIT_DS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_AUTOZ_GUI_OPEN_AUDIT_DS, AutozGuiOpenAuditDSPrivate))

typedef struct _AutozGuiOpenAuditDSPrivate AutozGuiOpenAuditDSPrivate;
struct _AutozGuiOpenAuditDSPrivate
	{
		AutozGuiCommons *commons;

		GtkWidget *w;
		GtkWidget *wlogin;
	};

G_DEFINE_TYPE (AutozGuiOpenAuditDS, autoz_gui_open_audit_ds, G_TYPE_OBJECT)

static void
autoz_gui_open_audit_ds_class_init (AutozGuiOpenAuditDSClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AutozGuiOpenAuditDSPrivate));

	object_class->set_property = autoz_gui_open_audit_ds_set_property;
	object_class->get_property = autoz_gui_open_audit_ds_get_property;

	/**
	 * AutozGuiOpenAuditDS::opened:
	 * @open_audit_ds:
	 *
	 */
	klass->opened_signal_id = g_signal_new ("opened",
	                                        G_TYPE_FROM_CLASS (object_class),
	                                        G_SIGNAL_RUN_LAST,
	                                        0,
	                                        NULL,
	                                        NULL,
	                                        g_cclosure_marshal_VOID__STRING,
	                                        G_TYPE_NONE,
	                                        1, G_TYPE_STRING);
}

static void
autoz_gui_open_audit_ds_init (AutozGuiOpenAuditDS *open_audit_ds)
{
	AutozGuiOpenAuditDSPrivate *priv = AUTOZ_GUI_OPEN_AUDIT_DS_GET_PRIVATE (open_audit_ds);
}

/**
 * autoz_gui_open_audit_ds_new:
 * @commons:
 * @id:
 *
 * Returns: the newly created #AutozGuiOpenAuditDS object.
 */
AutozGuiOpenAuditDS
*autoz_gui_open_audit_ds_new (AutozGuiCommons *commons)
{
	GError *error;

	GdauiLoginMode mode;

	AutozGuiOpenAuditDS *a = AUTOZ_GUI_OPEN_AUDIT_DS (g_object_new (autoz_gui_open_audit_ds_get_type (), NULL));

	AutozGuiOpenAuditDSPrivate *priv = AUTOZ_GUI_OPEN_AUDIT_DS_GET_PRIVATE (a);

	priv->commons = commons;

	error = NULL;
	gtk_builder_add_objects_from_file (priv->commons->gtkbuilder, priv->commons->guifile,
	                                   g_strsplit ("w_open_autoz_datasource", "|", -1),
	                                   &error);
	if (error != NULL)
		{
			g_warning ("Error: %s.", error->message);
			return NULL;
		}

	priv->w = GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, "w_open_autoz_datasource"));

	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button1"),
	                  "clicked", G_CALLBACK (autoz_gui_open_audit_ds_on_btn_cancel_clicked), (gpointer *)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button2"),
	                  "clicked", G_CALLBACK (autoz_gui_open_audit_ds_on_btn_open_clicked), (gpointer *)a);

	/* creating login widget */
	priv->wlogin = gdaui_login_new (NULL);

	g_object_get (G_OBJECT (priv->wlogin), "mode", &mode, NULL);
	mode |= GDA_UI_LOGIN_HIDE_DSN_SELECTION_MODE;
	gdaui_login_set_mode (GDAUI_LOGIN (priv->wlogin), mode);

	g_signal_connect (G_OBJECT (priv->wlogin), "changed",
	                  G_CALLBACK (autoz_gui_open_audit_ds_on_wlogin_changed), (gpointer *)a);

	gtk_container_add (GTK_CONTAINER (gtk_builder_get_object (priv->commons->gtkbuilder, "frame1")),
	                    priv->wlogin);

	gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, "button2")), FALSE);

	return a;
}

/**
 * autoz_gui_open_audit_ds_get_widget:
 * @open_audit_ds:
 *
 */
GtkWidget
*autoz_gui_open_audit_ds_get_widget (AutozGuiOpenAuditDS *open_audit_ds)
{
	AutozGuiOpenAuditDSPrivate *priv = AUTOZ_GUI_OPEN_AUDIT_DS_GET_PRIVATE (open_audit_ds);

	return priv->w;
}

/* PRIVATE */
static void
autoz_gui_open_audit_ds_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AutozGuiOpenAuditDS *open_audit_ds = AUTOZ_GUI_OPEN_AUDIT_DS (object);
	AutozGuiOpenAuditDSPrivate *priv = AUTOZ_GUI_OPEN_AUDIT_DS_GET_PRIVATE (open_audit_ds);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
autoz_gui_open_audit_ds_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AutozGuiOpenAuditDS *open_audit_ds = AUTOZ_GUI_OPEN_AUDIT_DS (object);
	AutozGuiOpenAuditDSPrivate *priv = AUTOZ_GUI_OPEN_AUDIT_DS_GET_PRIVATE (open_audit_ds);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

/* CALLBACK */
static void
autoz_gui_open_audit_ds_on_wlogin_changed (GdauiLogin *gdauilogin,
                                 gboolean arg1,
                                 gpointer user_data)
{
	AutozGuiOpenAuditDS *open_audit_ds = (AutozGuiOpenAuditDS *)user_data;

	AutozGuiOpenAuditDSPrivate *priv = AUTOZ_GUI_OPEN_AUDIT_DS_GET_PRIVATE (open_audit_ds);

	gboolean is_valid;

	g_object_get (G_OBJECT (priv->wlogin), "valid", &is_valid, NULL);
	gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, "button2")),
	                          is_valid);
}

static void
autoz_gui_open_audit_ds_on_btn_cancel_clicked (GtkButton *button,
                                     gpointer user_data)
{
	AutozGuiOpenAuditDS *open_audit_ds = (AutozGuiOpenAuditDS *)user_data;
	AutozGuiOpenAuditDSPrivate *priv = AUTOZ_GUI_OPEN_AUDIT_DS_GET_PRIVATE (open_audit_ds);

	gtk_widget_destroy (priv->w);
}

static void
autoz_gui_open_audit_ds_on_btn_open_clicked (GtkButton *button,
                                   gpointer user_data)
{
	AutozGuiOpenAuditDS *open_audit_ds = (AutozGuiOpenAuditDS *)user_data;
	AutozGuiOpenAuditDSPrivate *priv = AUTOZ_GUI_OPEN_AUDIT_DS_GET_PRIVATE (open_audit_ds);
	AutozGuiOpenAuditDSClass *klass = AUTOZ_GUI_OPEN_AUDIT_DS_GET_CLASS (open_audit_ds);

	gchar *cncstring;

	const GdaDsnInfo *info;
	info = gdaui_login_get_connection_information (GDAUI_LOGIN (priv->wlogin));

	cncstring = g_strconcat (info->provider, "://",
	                         (info->auth_string != NULL ? g_strdup_printf ("%s;", info->auth_string) : ""),
	                         info->cnc_string,
	                         NULL);

	g_signal_emit (open_audit_ds, klass->opened_signal_id, 0, cncstring);

	gtk_widget_destroy (priv->w);
}
