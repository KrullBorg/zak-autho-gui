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

#include <libgda-ui/libgda-ui.h>
#include <sql-parser/gda-sql-parser.h>

#include "authorization.h"
#include "roles.h"
#include "resources.h"

static void authorization_class_init (AuthorizationClass *klass);
static void authorization_init (Authorization *authorization);

static void authorization_load (Authorization *authorization);
static void authorization_save (Authorization *authorization);

static void authorization_fill_role (Authorization *authorization);
static void authorization_fill_resource (Authorization *authorization);

static void authorization_on_role_selected (gpointer instance, guint id, gpointer user_data);
static void authorization_on_resource_selected (gpointer instance, guint id, gpointer user_data);

static void authorization_set_property (GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec);
static void authorization_get_property (GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

static void authorization_on_btn_role_clicked (GtkButton *button,
                                    gpointer user_data);
static void authorization_on_btn_resource_clicked (GtkButton *button,
                                    gpointer user_data);

static void authorization_on_btn_cancel_clicked (GtkButton *button,
                                    gpointer user_data);
static void authorization_on_btn_save_clicked (GtkButton *button,
                                  gpointer user_data);

#define AUTHORIZATION_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_AUTHORIZATION, AuthorizationPrivate))

typedef struct _AuthorizationPrivate AuthorizationPrivate;
struct _AuthorizationPrivate
	{
		AutozGuiCommons *commons;

		GtkWidget *w;

		gint id;
	};

G_DEFINE_TYPE (Authorization, authorization, G_TYPE_OBJECT)

static void
authorization_class_init (AuthorizationClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AuthorizationPrivate));

	object_class->set_property = authorization_set_property;
	object_class->get_property = authorization_get_property;

	/**
	 * Authorization::updated:
	 * @authorization:
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
authorization_init (Authorization *authorization)
{
	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);
}

/**
 * authorization_new:
 * @commons:
 * @id:
 *
 * Returns: the newly created #Authorization object.
 */
Authorization
*authorization_new (AutozGuiCommons *commons, gint id)
{
	GError *error;

	Authorization *a = AUTHORIZATION (g_object_new (authorization_get_type (), NULL));

	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (a);

	priv->commons = commons;

	error = NULL;
	gtk_builder_add_objects_from_file (priv->commons->gtkbuilder, priv->commons->guifile,
	                                   g_strsplit ("w_authorization", "|", -1),
	                                   &error);
	if (error != NULL)
		{
			g_warning ("Error: %s.", error->message);
			return NULL;
		}

	priv->w = GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, "w_authorization"));

	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button23"),
	                  "clicked", G_CALLBACK (authorization_on_btn_role_clicked), (gpointer *)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button22"),
	                  "clicked", G_CALLBACK (authorization_on_btn_resource_clicked), (gpointer *)a);

	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button20"),
	                  "clicked", G_CALLBACK (authorization_on_btn_cancel_clicked), (gpointer *)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button21"),
	                  "clicked", G_CALLBACK (authorization_on_btn_save_clicked), (gpointer *)a);

	priv->id = id;
	if (priv->id == 0)
		{
			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label12")), "");
		}
	else
		{
			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label12")),
			                    g_strdup_printf ("%d", priv->id));
			authorization_load (a);
		}

	return a;
}

/**
 * authorization_get_widget:
 * @authorization:
 *
 */
GtkWidget
*authorization_get_widget (Authorization *authorization)
{
	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);

	return priv->w;
}

/* PRIVATE */
static void
authorization_load (Authorization *authorization)
{
	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);

	GError *error;
	gchar *sql;
	GdaStatement *stmt;
	GdaDataModel *dm;

	sql = g_strdup_printf ("SELECT id_roles, id_resources, type"
	                       " FROM %srules"
	                       " WHERE id = %d",
	                       priv->commons->prefix,
	                       priv->id);
	stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
	g_free (sql);
	error = NULL;
	dm = gda_connection_statement_execute_select (priv->commons->gdacon, stmt, NULL, &error);
	g_object_unref (stmt);
	if (dm != NULL && gda_data_model_get_n_rows (dm) == 1)
		{
			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label15")),
			                    gda_value_stringify (gda_data_model_get_value_at (dm, 0, 0, NULL)));
			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label14")),
			                    gda_value_stringify (gda_data_model_get_value_at (dm, 1, 0, NULL)));
			if (g_value_get_int (gda_data_model_get_value_at (dm, 2, 0, NULL)) == 1)
				{
					gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (priv->commons->gtkbuilder, "radiobutton1")), TRUE);
				}
			else
				{
					gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (priv->commons->gtkbuilder, "radiobutton2")), TRUE);
				}

			authorization_fill_role (authorization);
			authorization_fill_resource (authorization);
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
authorization_save (Authorization *authorization)
{
	const GdaDsnInfo *info;
	GError *error;
	gchar *sql;
	GdaStatement *stmt;
	GdaDataModel *dm;
	GtkWidget *dialog;

	guint type;

	AuthorizationClass *klass = AUTHORIZATION_GET_CLASS (authorization);

	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (priv->commons->gtkbuilder, "radiobutton1"))))
		{
			type = 1;
		}
	else
		{
			type = 2;
		}

	if (strtol (gtk_label_get_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label15"))), NULL, 10) == 0)
		{
			dialog = gtk_message_dialog_new (GTK_WINDOW (priv->w),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_INFO,
			                                 GTK_BUTTONS_OK,
			                                 "Select the role.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			return;
		}

	if (priv->id == 0)
		{
			/* find the new id */
			guint new_id;

			new_id = 0;
			sql = g_strdup_printf ("SELECT COALESCE (MAX (id), 0) FROM %srules", priv->commons->prefix);
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

			gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label12")), g_strdup_printf ("%d", new_id));

			sql = g_strdup_printf ("INSERT INTO %srules"
			                       " (id, id_roles, id_resources, type)"
			                       " VALUES (%d, %d, %d, %d)",
			                       priv->commons->prefix,
			                       new_id,
			                       strtol (gtk_label_get_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label15"))), NULL, 10),
			                       strtol (gtk_label_get_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label14"))), NULL, 10),
			                       type);
			stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
		}
	else
		{
			sql = g_strdup_printf ("UPDATE %srules"
			                       " SET"
			                       " id_roles = %d,"
			                       " id_resources = %d,"
			                       " type = %d,"
			                       " WHERE id = %d",
			                       priv->commons->prefix,
			                       strtol (gtk_label_get_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label15"))), NULL, 10),
			                       strtol (gtk_label_get_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label14"))), NULL, 10),
			                       type,
			                       priv->id);
			stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
		}
	g_free (sql);

	error = NULL;
	if (gda_connection_statement_execute_non_select (priv->commons->gdacon, stmt, NULL, NULL, &error) == 1)
		{
			g_object_unref (stmt);

			g_signal_emit (authorization, klass->updated_signal_id, 0);

			if (priv->id == 0)
				{
					priv->id = strtol (gtk_label_get_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label12"))), NULL, 10);
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
					gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label12")), "");
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
authorization_fill_role (Authorization *authorization)
{
	GError *error;
	gchar *sql;
	GdaStatement *stmt;
	GdaDataModel *dm;

	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);

	sql = g_strdup_printf ("SELECT role_id"
	                       " FROM %sroles"
	                       " WHERE id = %d",
	                       priv->commons->prefix,
	                       strtol (gtk_label_get_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label15"))), NULL, 10));
	stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
	g_free (sql);
	dm = gda_connection_statement_execute_select (priv->commons->gdacon, stmt, NULL, &error);
	g_object_unref (stmt);
	if (dm != NULL && gda_data_model_get_n_rows (dm) == 1)
		{
			gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object (priv->commons->gtkbuilder, "entry5")),
			                    gda_value_stringify (gda_data_model_get_value_at (dm, 0, 0, NULL)));
		}
	else
		{
			gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object (priv->commons->gtkbuilder, "entry5")),
			                    "");
		}
	g_object_unref (dm);
}

static void
authorization_fill_resource (Authorization *authorization)
{
	GError *error;
	gchar *sql;
	GdaStatement *stmt;
	GdaDataModel *dm;

	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);

	sql = g_strdup_printf ("SELECT resource_id"
	                       " FROM %sresources"
	                       " WHERE id = %d",
	                       priv->commons->prefix,
	                       strtol (gtk_label_get_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label14"))), NULL, 10));
	stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
	g_free (sql);
	dm = gda_connection_statement_execute_select (priv->commons->gdacon, stmt, NULL, &error);
	g_object_unref (stmt);
	if (dm != NULL && gda_data_model_get_n_rows (dm) == 1)
		{
			gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object (priv->commons->gtkbuilder, "entry4")),
			                    gda_value_stringify (gda_data_model_get_value_at (dm, 0, 0, NULL)));
		}
	else
		{
			gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object (priv->commons->gtkbuilder, "entry4")),
			                    "");
		}
	g_object_unref (dm);
}

static void
authorization_on_role_selected (gpointer instance, guint id, gpointer user_data)
{
	Authorization *authorization = AUTHORIZATION (instance);
	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);

	gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label15")),
	                    g_strdup_printf ("%d", id));
}

static void
authorization_on_resource_selected (gpointer instance, guint id, gpointer user_data)
{
	Authorization *authorization = AUTHORIZATION (instance);
	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);

	gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (priv->commons->gtkbuilder, "label14")),
	                    g_strdup_printf ("%d", id));
}

static void
authorization_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	Authorization *authorization = AUTHORIZATION (object);
	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
authorization_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	Authorization *authorization = AUTHORIZATION (object);
	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

/* CALLBACK */
static void
authorization_on_btn_role_clicked (GtkButton *button,
                        gpointer user_data)
{
	GtkWidget *w;

	Authorization *authorization = (Authorization *)user_data;
	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);

	Roles *c = roles_new (priv->commons, TRUE);

	g_signal_connect (G_OBJECT (c), "selezionato",
	                  G_CALLBACK (authorization_on_role_selected), user_data);

	w = roles_get_widget (c);
	gtk_window_set_transient_for (GTK_WINDOW (w), GTK_WINDOW (priv->w));
	gtk_widget_show (w);
}

static void
authorization_on_btn_resource_clicked (GtkButton *button,
                        gpointer user_data)
{
	GtkWidget *w;

	Authorization *authorization = (Authorization *)user_data;
	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);

	Resources *c = resources_new (priv->commons, TRUE);

	g_signal_connect (G_OBJECT (c), "selezionato",
	                  G_CALLBACK (authorization_on_resource_selected), user_data);

	w = resources_get_widget (c);
	gtk_window_set_transient_for (GTK_WINDOW (w), GTK_WINDOW (priv->w));
	gtk_widget_show (w);
}

static void
authorization_on_btn_cancel_clicked (GtkButton *button,
                        gpointer user_data)
{
	Authorization *authorization = (Authorization *)user_data;

	AuthorizationPrivate *priv = AUTHORIZATION_GET_PRIVATE (authorization);

	gtk_widget_destroy (priv->w);
}

static void
authorization_on_btn_save_clicked (GtkButton *button,
                      gpointer user_data)
{
	authorization_save ((Authorization *)user_data);
}
