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

#include "role.h"

static void role_class_init (RoleClass *klass);
static void role_init (Role *role);

static void role_load (Role *role);
static void role_save (Role *role);

static void role_set_property (GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec);
static void role_get_property (GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

static void role_on_btn_cancel_clicked (GtkButton *button,
                                    gpointer user_data);
static void role_on_btn_save_clicked (GtkButton *button,
                                  gpointer user_data);

#define ROLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_ROLE, RolePrivate))

typedef struct _RolePrivate RolePrivate;
struct _RolePrivate
	{
		AutozGuiCommons *commons;

		GtkWidget *w;

		gint id;
	};

G_DEFINE_TYPE (Role, role, G_TYPE_OBJECT)

static void
role_class_init (RoleClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (RolePrivate));

	object_class->set_property = role_set_property;
	object_class->get_property = role_get_property;

	/**
	 * Role::updated:
	 * @role:
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
role_init (Role *role)
{
	RolePrivate *priv = ROLE_GET_PRIVATE (role);
}

/**
 * role_new:
 * @commons:
 * @id:
 *
 * Returns: the newly created #Role object.
 */
Role
*role_new (AutozGuiCommons *commons, gint id)
{
	GError *error;

	Role *a = ROLE (g_object_new (role_get_type (), NULL));

	RolePrivate *priv = ROLE_GET_PRIVATE (a);

	priv->commons = commons;

	error = NULL;
	gtk_builder_add_objects_from_file (priv->commons->gtkbuilder, priv->commons->guifile,
	                                   g_strsplit ("w_role", "|", -1),
	                                   &error);
	if (error != NULL)
		{
			g_warning ("Error: %s.", error->message);
			return NULL;
		}

	priv->w = GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, "w_role"));

	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button3"),
	                  "clicked", G_CALLBACK (role_on_btn_cancel_clicked), (gpointer *)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button4"),
	                  "clicked", G_CALLBACK (role_on_btn_save_clicked), (gpointer *)a);

	priv->id = id;
	if (priv->id == 0)
		{
			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label2")), "");
		}
	else
		{
			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label2")), g_strdup_printf ("%d", priv->id));
			role_load (a);
		}

	return a;
}

/**
 * role_get_widget:
 * @role:
 *
 */
GtkWidget
*role_get_widget (Role *role)
{
	RolePrivate *priv = ROLE_GET_PRIVATE (role);

	return priv->w;
}

/* PRIVATE */
static void
role_load (Role *role)
{
	RolePrivate *priv = ROLE_GET_PRIVATE (role);

	GError *error;
	gchar *sql;
	GdaStatement *stmt;
	GdaDataModel *dm;

	sql = g_strdup_printf ("SELECT role_id FROM %sroles WHERE id = %d",
	                       priv->commons->prefix,
	                       priv->id);
	stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
	g_free (sql);
	error = NULL;
	dm = gda_connection_statement_execute_select (priv->commons->gdacon, stmt, NULL, &error);
	g_object_unref (stmt);
	if (dm != NULL && gda_data_model_get_n_rows (dm) == 1)
		{
			gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object (priv->commons->gtkbuilder, "entry1")),
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
	g_object_unref (dm);
}

static void
role_save (Role *role)
{
	const GdaDsnInfo *info;
	GError *error;
	gchar *sql;
	GdaStatement *stmt;
	GdaDataModel *dm;
	GtkWidget *dialog;

	RoleClass *klass = ROLE_GET_CLASS (role);

	RolePrivate *priv = ROLE_GET_PRIVATE (role);

	if (g_strcmp0 (gtk_entry_get_text (GTK_ENTRY (gtk_builder_get_object (priv->commons->gtkbuilder, "entry1"))), "") == 0)
		{
			dialog = gtk_message_dialog_new (GTK_WINDOW (priv->w),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_INFO,
			                                 GTK_BUTTONS_OK,
			                                 "Insert the name of the role.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			return;
		}

	if (priv->id == 0)
		{
			/* find the new id */
			guint new_id;

			new_id = 0;
			sql = g_strdup_printf ("SELECT COALESCE (MAX (id), 0) FROM %sroles", priv->commons->prefix);
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

			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label2")), g_strdup_printf ("%d", new_id));

			sql = g_strdup_printf ("INSERT INTO %sroles (id, role_id)"
			                       " VALUES (%d, '%s')",
			                       priv->commons->prefix,
			                       new_id,
			                       gtk_entry_get_text (GTK_ENTRY (gtk_builder_get_object (priv->commons->gtkbuilder, "entry1"))));
			stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
		}
	else
		{
			sql = g_strdup_printf ("UPDATE %sroles"
			                       " SET"
			                       " role_id = '%s'"
			                       " WHERE id = %d",
			                       priv->commons->prefix,
			                       gtk_entry_get_text (GTK_ENTRY (gtk_builder_get_object (priv->commons->gtkbuilder, "entry1"))),
			                       priv->id);
			stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
		}
	g_free (sql);

	error = NULL;
	if (gda_connection_statement_execute_non_select (priv->commons->gdacon, stmt, NULL, NULL, &error) == 1)
		{
			g_object_unref (stmt);

			g_signal_emit (role, klass->updated_signal_id, 0);

			if (priv->id == 0)
				{
					priv->id = strtol (gtk_label_get_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label2"))), NULL, 10);
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
					gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label2")), "");
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
role_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	Role *role = ROLE (object);
	RolePrivate *priv = ROLE_GET_PRIVATE (role);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
role_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	Role *role = ROLE (object);
	RolePrivate *priv = ROLE_GET_PRIVATE (role);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

/* CALLBACK */
static void
role_on_btn_cancel_clicked (GtkButton *button,
                        gpointer user_data)
{
	Role *role = (Role *)user_data;

	RolePrivate *priv = ROLE_GET_PRIVATE (role);

	gtk_widget_destroy (priv->w);
}

static void
role_on_btn_save_clicked (GtkButton *button,
                      gpointer user_data)
{
	role_save ((Role *)user_data);
}
