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

static void resource_class_init (ResourceClass *klass);
static void resource_init (Resource *resource);

static void resource_load (Resource *resource);
static void resource_save (Resource *resource);

static void resource_set_property (GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec);
static void resource_get_property (GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

static void resource_on_btn_cancel_clicked (GtkButton *button,
                                    gpointer user_data);
static void resource_on_btn_save_clicked (GtkButton *button,
                                  gpointer user_data);

#define RESOURCE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_RESOURCE, ResourcePrivate))

typedef struct _ResourcePrivate ResourcePrivate;
struct _ResourcePrivate
	{
		Commons *commons;

		GtkWidget *w;

		gint id;
	};

G_DEFINE_TYPE (Resource, resource, G_TYPE_OBJECT)

static void
resource_class_init (ResourceClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (ResourcePrivate));

	object_class->set_property = resource_set_property;
	object_class->get_property = resource_get_property;

	/**
	 * Resource::updated:
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
resource_init (Resource *resource)
{
	ResourcePrivate *priv = RESOURCE_GET_PRIVATE (resource);
}

/**
 * resource_new:
 * @commons:
 * @id:
 *
 * Returns: the newly created #Resource object.
 */
Resource
*resource_new (Commons *commons, gint id)
{
	GError *error;

	Resource *a = RESOURCE (g_object_new (resource_get_type (), NULL));

	ResourcePrivate *priv = RESOURCE_GET_PRIVATE (a);

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
	                  "clicked", G_CALLBACK (resource_on_btn_cancel_clicked), (gpointer *)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button6"),
	                  "clicked", G_CALLBACK (resource_on_btn_save_clicked), (gpointer *)a);

	priv->id = id;
	if (priv->id == 0)
		{
			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label5")), "");
		}
	else
		{
			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label5")), g_strdup_printf ("%d", priv->id));
			resource_load (a);
		}

	return a;
}

/**
 * resource_get_widget:
 * @resource:
 *
 */
GtkWidget
*resource_get_widget (Resource *resource)
{
	ResourcePrivate *priv = RESOURCE_GET_PRIVATE (resource);

	return priv->w;
}

/* PRIVATE */
static void
resource_load (Resource *resource)
{
	ResourcePrivate *priv = RESOURCE_GET_PRIVATE (resource);

	GError *error;
	gchar *sql;
	GdaStatement *stmt;
	GdaDataModel *dm;

	sql = g_strdup_printf ("SELECT resource_id FROM %sresources WHERE id = %d",
	                       priv->commons->prefix,
	                       priv->id);
	stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
	g_free (sql);
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
resource_save (Resource *resource)
{
	const GdaDsnInfo *info;
	GError *error;
	gchar *sql;
	GdaStatement *stmt;
	GdaDataModel *dm;
	GtkWidget *dialog;

	ResourceClass *klass = RESOURCE_GET_CLASS (resource);

	ResourcePrivate *priv = RESOURCE_GET_PRIVATE (resource);

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
resource_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	Resource *resource = RESOURCE (object);
	ResourcePrivate *priv = RESOURCE_GET_PRIVATE (resource);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
resource_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	Resource *resource = RESOURCE (object);
	ResourcePrivate *priv = RESOURCE_GET_PRIVATE (resource);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

/* CALLBACK */
static void
resource_on_btn_cancel_clicked (GtkButton *button,
                        gpointer user_data)
{
	Resource *resource = (Resource *)user_data;

	ResourcePrivate *priv = RESOURCE_GET_PRIVATE (resource);

	gtk_widget_destroy (priv->w);
}

static void
resource_on_btn_save_clicked (GtkButton *button,
                      gpointer user_data)
{
	resource_save ((Resource *)user_data);
}
