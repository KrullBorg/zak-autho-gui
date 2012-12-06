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

#include <libgda-ui/libgda-ui.h>
#include <sql-parser/gda-sql-parser.h>

#include "resource.h"

static void autoz_gui_resource_class_init (AutozGuiResourceClass *klass);
static void autoz_gui_resource_init (AutozGuiResource *resource);

static void autoz_gui_resource_load (AutozGuiResource *resource);
static void autoz_gui_resource_save (AutozGuiResource *resource);

static void autoz_gui_resource_set_property (GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec);
static void autoz_gui_resource_get_property (GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

static void autoz_gui_resource_on_btn_cancel_clicked (GtkButton *button,
                                    gpointer user_data);
static void autoz_gui_resource_on_btn_save_clicked (GtkButton *button,
                                  gpointer user_data);

#define AUTOZ_GUI_RESOURCE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_AUTOZ_GUI_RESOURCE, AutozGuiResourcePrivate))

typedef struct _AutozGuiResourcePrivate AutozGuiResourcePrivate;
struct _AutozGuiResourcePrivate
	{
		AutozGuiCommons *commons;

		GtkWidget *w;

		gint id;
	};

G_DEFINE_TYPE (AutozGuiResource, autoz_gui_resource, G_TYPE_OBJECT)

static void
autoz_gui_resource_class_init (AutozGuiResourceClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AutozGuiResourcePrivate));

	object_class->set_property = autoz_gui_resource_set_property;
	object_class->get_property = autoz_gui_resource_get_property;

	/**
	 * AutozGuiResource::updated:
	 * @resource:
	 *
	 */
	klass->updated_signal_id = g_signal_new ("updated",
	                                               G_TYPE_FROM_CLASS (object_class),
	                                               G_SIGNAL_RUN_LAST,
	                                               0,
	                                               NULL,
	                                               NULL,
	                                               g_cclosure_marshal_VOID__VOID,
	                                               G_TYPE_NONE,
	                                               0);
}

static void
autoz_gui_resource_init (AutozGuiResource *resource)
{
	AutozGuiResourcePrivate *priv = AUTOZ_GUI_RESOURCE_GET_PRIVATE (resource);
}

/**
 * autoz_gui_resource_new:
 * @commons:
 * @id:
 *
 * Returns: the newly created #AutozGuiResource object.
 */
AutozGuiResource
*autoz_gui_resource_new (AutozGuiCommons *commons, gint id)
{
	GError *error;

	AutozGuiResource *a = AUTOZ_GUI_RESOURCE (g_object_new (autoz_gui_resource_get_type (), NULL));

	AutozGuiResourcePrivate *priv = AUTOZ_GUI_RESOURCE_GET_PRIVATE (a);

	priv->commons = commons;

	error = NULL;
	gtk_builder_add_objects_from_file (priv->commons->gtkbuilder, priv->commons->guifile,
	                                   g_strsplit ("w_resource", "|", -1),
	                                   &error);
	if (error != NULL)
		{
			g_warning ("Error: %s.", error->message);
			return NULL;
		}

	priv->w = GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, "w_resource"));

	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button5"),
	                  "clicked", G_CALLBACK (autoz_gui_resource_on_btn_cancel_clicked), (gpointer *)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button6"),
	                  "clicked", G_CALLBACK (autoz_gui_resource_on_btn_save_clicked), (gpointer *)a);

	priv->id = id;
	if (priv->id == 0)
		{
			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label5")), "");
		}
	else
		{
			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label5")), g_strdup_printf ("%d", priv->id));
			autoz_gui_resource_load (a);
		}

	return a;
}

/**
 * autoz_gui_resource_get_widget:
 * @resource:
 *
 */
GtkWidget
*autoz_gui_resource_get_widget (AutozGuiResource *resource)
{
	AutozGuiResourcePrivate *priv = AUTOZ_GUI_RESOURCE_GET_PRIVATE (resource);

	return priv->w;
}

/* PRIVATE */
static void
autoz_gui_resource_load (AutozGuiResource *resource)
{
	AutozGuiResourcePrivate *priv = AUTOZ_GUI_RESOURCE_GET_PRIVATE (resource);

	GError *error;
	gchar *sql;
	GdaStatement *stmt;
	GdaDataModel *dm;

	sql = g_strdup_printf ("SELECT resource_id FROM %sresources WHERE id = %d",
	                       priv->commons->prefix,
	                       priv->id);
	stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
	g_free (sql);
	error = NULL;
	dm = gda_connection_statement_execute_select (priv->commons->gdacon, stmt, NULL, &error);
	g_object_unref (stmt);
	if (dm != NULL && gda_data_model_get_n_rows (dm) == 1)
		{
			gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object (priv->commons->gtkbuilder, "entry2")),
			                    gda_value_stringify (gda_data_model_get_value_at (dm, 0, 0, NULL)));
		}
	else
		{
			GtkWidget *dialog;
			dialog = gtk_message_dialog_new (GTK_WINDOW (priv->w),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_WARNING,
			                                 GTK_BUTTONS_OK,
			                                 "Record %d doesn't exist.", priv->id);
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
}

static void
autoz_gui_resource_save (AutozGuiResource *resource)
{
	const GdaDsnInfo *info;
	GError *error;
	gchar *sql;
	GdaStatement *stmt;
	GdaDataModel *dm;
	GtkWidget *dialog;

	AutozGuiResourceClass *klass = AUTOZ_GUI_RESOURCE_GET_CLASS (resource);

	AutozGuiResourcePrivate *priv = AUTOZ_GUI_RESOURCE_GET_PRIVATE (resource);

	if (g_strcmp0 (gtk_entry_get_text (GTK_ENTRY (gtk_builder_get_object (priv->commons->gtkbuilder, "entry2"))), "") == 0)
		{
			dialog = gtk_message_dialog_new (GTK_WINDOW (priv->w),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_INFO,
			                                 GTK_BUTTONS_OK,
			                                 "Insert the name of the resource.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			return;
		}

	if (priv->id == 0)
		{
			/* find the new id */
			guint new_id;

			new_id = 0;
			sql = g_strdup_printf ("SELECT COALESCE (MAX (id), 0) FROM %sresources", priv->commons->prefix);
			stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
			g_free (sql);
			dm = gda_connection_statement_execute_select (priv->commons->gdacon, stmt, NULL, &error);
			g_object_unref (stmt);
			if (dm != NULL && gda_data_model_get_n_rows (dm) == 1)
				{
					new_id = g_value_get_int (gda_data_model_get_value_at (dm, 0, 0, NULL));
				}
			new_id++;
			g_object_unref (dm);

			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label5")), g_strdup_printf ("%d", new_id));

			sql = g_strdup_printf ("INSERT INTO %sresources (id, resource_id)"
			                       " VALUES (%d, '%s')",
			                       priv->commons->prefix,
			                       new_id,
			                       gtk_entry_get_text (GTK_ENTRY (gtk_builder_get_object (priv->commons->gtkbuilder, "entry2"))));
			stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
		}
	else
		{
			sql = g_strdup_printf ("UPDATE %sresources"
			                       " SET"
			                       " resource_id = '%s'"
			                       " WHERE id = %d",
			                       priv->commons->prefix,
			                       gtk_entry_get_text (GTK_ENTRY (gtk_builder_get_object (priv->commons->gtkbuilder, "entry2"))),
			                       priv->id);
			stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
		}
	g_free (sql);

	error = NULL;
	if (gda_connection_statement_execute_non_select (priv->commons->gdacon, stmt, NULL, NULL, &error) == 1)
		{
			g_object_unref (stmt);

			g_signal_emit (resource, klass->updated_signal_id, 0);

			if (priv->id == 0)
				{
					priv->id = strtol (gtk_label_get_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label5"))), NULL, 10);
				}

			dialog = gtk_message_dialog_new (GTK_WINDOW (priv->w),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_INFO,
			                                 GTK_BUTTONS_OK,
			                                 "Saved with success.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
	else
		{
			if (priv->id == 0)
				{
					gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label5")), "");
				}

			dialog = gtk_message_dialog_new (GTK_WINDOW (priv->w),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_WARNING,
			                                 GTK_BUTTONS_OK,
			                                 "Error on saving.\n\n%s",
			                                 (error != NULL && error->message != NULL ? error->message : "No details."));
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
}

static void
autoz_gui_resource_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AutozGuiResource *resource = AUTOZ_GUI_RESOURCE (object);
	AutozGuiResourcePrivate *priv = AUTOZ_GUI_RESOURCE_GET_PRIVATE (resource);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
autoz_gui_resource_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AutozGuiResource *resource = AUTOZ_GUI_RESOURCE (object);
	AutozGuiResourcePrivate *priv = AUTOZ_GUI_RESOURCE_GET_PRIVATE (resource);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

/* CALLBACK */
static void
autoz_gui_resource_on_btn_cancel_clicked (GtkButton *button,
                        gpointer user_data)
{
	AutozGuiResource *resource = (AutozGuiResource *)user_data;

	AutozGuiResourcePrivate *priv = AUTOZ_GUI_RESOURCE_GET_PRIVATE (resource);

	gtk_widget_destroy (priv->w);
}

static void
autoz_gui_resource_on_btn_save_clicked (GtkButton *button,
                      gpointer user_data)
{
	autoz_gui_resource_save ((AutozGuiResource *)user_data);
}
