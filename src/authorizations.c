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

#include <string.h>

#include <sql-parser/gda-sql-parser.h>

#include "authorizations.h"
#include "authorization.h"

static void autoz_gui_authorizations_class_init (AutozGuiAuthorizationsClass *klass);
static void autoz_gui_authorizations_init (AutozGuiAuthorizations *authorizations);

static void autoz_gui_authorizations_load (AutozGuiAuthorizations *authorizations);
static void autoz_gui_authorizations_edit (AutozGuiAuthorizations *authorizations);

static void autoz_gui_authorizations_on_authorization_updated (gpointer instance, gpointer user_data);

static void autoz_gui_authorizations_set_property (GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec);
static void autoz_gui_authorizations_get_property (GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

static void autoz_gui_authorizations_on_btn_new_clicked (GtkButton *button,
                      gpointer user_data);
static void autoz_gui_authorizations_on_btn_edit_clicked (GtkButton *button,
                      gpointer user_data);
static void autoz_gui_authorizations_on_btn_delete_clicked (GtkButton *button,
                        gpointer user_data);
static void autoz_gui_authorizations_on_trv_autoz_gui_authorizations_row_activated (GtkTreeView *tree_view,
                                             GtkTreePath *tree_path,
                                             GtkTreeViewColumn *column,
                                             gpointer user_data);

#define AUTOZ_GUI_AUTHORIZATIONS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_AUTOZ_GUI_AUTHORIZATIONS, AutozGuiAuthorizationsPrivate))

enum
{
	COL_ID,
	COL_ROLE,
	COL_RESOURCE,
	COL_TYPE
};

typedef struct _AutozGuiAuthorizationsPrivate AutozGuiAuthorizationsPrivate;
struct _AutozGuiAuthorizationsPrivate
	{
		AutozGuiCommons *commons;

		GtkWidget *widget;

		GtkTreeSelection *selection;
		GtkListStore *lstore_authorizations;
	};

G_DEFINE_TYPE (AutozGuiAuthorizations, autoz_gui_authorizations, G_TYPE_OBJECT)

static void
autoz_gui_authorizations_class_init (AutozGuiAuthorizationsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AutozGuiAuthorizationsPrivate));

	object_class->set_property = autoz_gui_authorizations_set_property;
	object_class->get_property = autoz_gui_authorizations_get_property;
}

static void
autoz_gui_authorizations_init (AutozGuiAuthorizations *authorizations)
{
	AutozGuiAuthorizationsPrivate *priv = AUTOZ_GUI_AUTHORIZATIONS_GET_PRIVATE (authorizations);
}

/**
 * autoz_gui_authorizations_new:
 * @commons:
 * @selection:
 *
 * Returns: the newly created #AutozGuiAuthorizations object.
 */
AutozGuiAuthorizations
*autoz_gui_authorizations_new (AutozGuiCommons *commons, gboolean selection)
{
	GError *error;

	AutozGuiAuthorizations *a = AUTOZ_GUI_AUTHORIZATIONS (g_object_new (autoz_gui_authorizations_get_type (), NULL));

	AutozGuiAuthorizationsPrivate *priv = AUTOZ_GUI_AUTHORIZATIONS_GET_PRIVATE (a);

	priv->commons = commons;

	error = NULL;
	gtk_builder_add_objects_from_file (priv->commons->gtkbuilder, priv->commons->guifile,
	                                   g_strsplit (g_strconcat ("lstore_authorizations",
	                                                            (selection ? "|w_authorizations" : "|vbox8"),
	                                                            NULL),
	                                               "|", -1),
	                                   &error);
	if (error != NULL)
		{
			g_warning ("Error: %s.", error->message);
			return NULL;
		}

	priv->widget = GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, (selection ? "w_authorizations" : "vbox8")));
	priv->selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (gtk_builder_get_object (priv->commons->gtkbuilder, "treeview3")));
	priv->lstore_authorizations = GTK_LIST_STORE (gtk_builder_get_object (priv->commons->gtkbuilder, "lstore_authorizations"));

	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button17"),
	                  "clicked", G_CALLBACK (autoz_gui_authorizations_on_btn_new_clicked), (gpointer)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button18"),
	                  "clicked", G_CALLBACK (autoz_gui_authorizations_on_btn_edit_clicked), (gpointer)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button19"),
	                  "clicked", G_CALLBACK (autoz_gui_authorizations_on_btn_delete_clicked), (gpointer)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "treeview3"),
	                  "row-activated", G_CALLBACK (autoz_gui_authorizations_on_trv_autoz_gui_authorizations_row_activated), (gpointer)a);

	if (!selection)
		{
			/*gtk_widget_hide (GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, "button10")));
			gtk_widget_hide (GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, "button13")));*/
		}

	autoz_gui_authorizations_load (a);

	return a;
}

/**
 * autoz_gui_authorizations_get_widget:
 * @authorizations:
 *
 */
GtkWidget
*autoz_gui_authorizations_get_widget (AutozGuiAuthorizations *authorizations)
{
	AutozGuiAuthorizationsPrivate *priv;

	g_return_val_if_fail (IS_AUTOZ_GUI_AUTHORIZATIONS (authorizations), NULL);

	priv = AUTOZ_GUI_AUTHORIZATIONS_GET_PRIVATE (authorizations);

	return priv->widget;
}

/* PRIVATE */
static void
autoz_gui_authorizations_load (AutozGuiAuthorizations *authorizations)
{
	GtkTreeIter iter;

	gchar *sql;

	GdaStatement *stmt;
	GError *error;
	GdaDataModel *dm;

	gint rows;
	gint row;

	AutozGuiAuthorizationsPrivate *priv = AUTOZ_GUI_AUTHORIZATIONS_GET_PRIVATE (authorizations);

	gtk_list_store_clear (priv->lstore_authorizations);

	sql = g_strdup_printf ("SELECT ru.id,"
	                       " ro.role_id,"
	                       " re.resource_id,"
	                       " ru.type"
	                       " FROM %srules AS ru"
	                       " INNER JOIN %sroles AS ro ON ru.id_roles = ro.id"
	                       " LEFT JOIN %sresources AS re ON ru.id_resources = re.id"
	                       " ORDER BY ro.role_id, re.resource_id, ru.type",
	                       priv->commons->prefix,
	                       priv->commons->prefix,
	                       priv->commons->prefix);
	stmt = gda_sql_parser_parse_string (priv->commons->gdaparser, sql, NULL, NULL);
	g_free (sql);
	dm = gda_connection_statement_execute_select (priv->commons->gdacon, stmt, NULL, &error);
	g_object_unref (stmt);
	if (dm != NULL)
		{
			rows = gda_data_model_get_n_rows (dm);
			for (row = 0; row < rows; row++)
				{
					gtk_list_store_append (priv->lstore_authorizations, &iter);
					gtk_list_store_set (priv->lstore_authorizations, &iter,
					                    COL_ID, g_value_get_int (gda_data_model_get_value_at (dm, 0, row, NULL)),
					                    COL_ROLE, gda_value_stringify (gda_data_model_get_value_at (dm, 1, row, NULL)),
					                    COL_RESOURCE, gda_value_stringify (gda_data_model_get_value_at (dm, 2, row, NULL)),
					                    COL_TYPE, g_value_get_int (gda_data_model_get_value_at (dm, 3, row, NULL)) == 1 ? "gtk-yes" : "gtk-no",
					                    -1);
				}

			g_object_unref (dm);
		}
	else
		{
			/* TODO */
		}
}

static void
autoz_gui_authorizations_edit (AutozGuiAuthorizations *authorizations)
{
	GtkTreeIter iter;
	guint id;

	AutozGuiAuthorizationsPrivate *priv = AUTOZ_GUI_AUTHORIZATIONS_GET_PRIVATE (authorizations);

	if (gtk_tree_selection_get_selected (priv->selection, NULL, &iter))
		{
			GtkWidget *w;

			gtk_tree_model_get (GTK_TREE_MODEL (priv->lstore_authorizations), &iter,
			                    COL_ID, &id,
			                    -1);

			AutozGuiAuthorization *c = autoz_gui_authorization_new (priv->commons, id);

			g_signal_connect (G_OBJECT (c), "updated",
			                  G_CALLBACK (autoz_gui_authorizations_on_authorization_updated), (gpointer)authorizations);

			w = autoz_gui_authorization_get_widget (c);
			gtk_window_set_transient_for (GTK_WINDOW (w), GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")));
			gtk_widget_show_all (w);
		}
	else
		{
			GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
			                                            GTK_DIALOG_DESTROY_WITH_PARENT,
			                                            GTK_MESSAGE_WARNING,
			                                            GTK_BUTTONS_OK,
			                                            "Select an authorization.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
}

static void
autoz_gui_authorizations_on_authorization_updated (gpointer instance, gpointer user_data)
{
	autoz_gui_authorizations_load ((AutozGuiAuthorizations *)user_data);
}

static void
autoz_gui_authorizations_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AutozGuiAuthorizations *authorizations = AUTOZ_GUI_AUTHORIZATIONS (object);
	AutozGuiAuthorizationsPrivate *priv = AUTOZ_GUI_AUTHORIZATIONS_GET_PRIVATE (authorizations);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
autoz_gui_authorizations_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AutozGuiAuthorizations *authorizations = AUTOZ_GUI_AUTHORIZATIONS (object);
	AutozGuiAuthorizationsPrivate *priv = AUTOZ_GUI_AUTHORIZATIONS_GET_PRIVATE (authorizations);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

/* CALLBACK */
static void
autoz_gui_authorizations_on_btn_new_clicked (GtkButton *button,
                      gpointer user_data)
{
	GtkWidget *w;

	AutozGuiAuthorizations *authorizations = (AutozGuiAuthorizations *)user_data;
	AutozGuiAuthorizationsPrivate *priv = AUTOZ_GUI_AUTHORIZATIONS_GET_PRIVATE (authorizations);

	AutozGuiAuthorization *c = autoz_gui_authorization_new (priv->commons, 0);

	g_signal_connect (G_OBJECT (c), "updated",
	                  G_CALLBACK (autoz_gui_authorizations_on_authorization_updated), (gpointer)authorizations);

	w = autoz_gui_authorization_get_widget (c);
	gtk_window_set_transient_for (GTK_WINDOW (w), GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")));
	gtk_widget_show_all (w);
}

static void
autoz_gui_authorizations_on_btn_edit_clicked (GtkButton *button,
                      gpointer user_data)
{
	autoz_gui_authorizations_edit ((AutozGuiAuthorizations *)user_data);
}

static void
autoz_gui_authorizations_on_btn_delete_clicked (GtkButton *button,
                        gpointer user_data)
{
	GtkWidget *dialog;
	gboolean risp;

	GtkTreeIter iter;
	guint id;

	AutozGuiAuthorizations *authorizations = (AutozGuiAuthorizations *)user_data;
	AutozGuiAuthorizationsPrivate *priv = AUTOZ_GUI_AUTHORIZATIONS_GET_PRIVATE (authorizations);

	if (gtk_tree_selection_get_selected (priv->selection, NULL, &iter))
		{
			dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_QUESTION,
			                                 GTK_BUTTONS_YES_NO,
			                                 "Are you sure to want to delete the selected authorization?");
			risp = gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			if (risp == GTK_RESPONSE_YES)
				{
					GError *error;
					GdaStatement *stmt;

					gtk_tree_model_get (GTK_TREE_MODEL (priv->lstore_authorizations), &iter,
					                    COL_ID, &id,
					                    -1);

					error = NULL;
					stmt = gda_sql_parser_parse_string (priv->commons->gdaparser,
					                                    g_strdup_printf ("DELETE FROM %srules WHERE id = %d", priv->commons->prefix, id),
					                                    NULL, &error);

					if (stmt == NULL || error != NULL)
						{
							dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
							                                 GTK_DIALOG_DESTROY_WITH_PARENT,
							                                 GTK_MESSAGE_WARNING,
							                                 GTK_BUTTONS_OK,
							                                 "You must select an authorization.%s",
							                                 error != NULL && error->message != NULL ? g_strdup_printf ("\n\n%s", error->message) : "");
							gtk_dialog_run (GTK_DIALOG (dialog));
							gtk_widget_destroy (dialog);
							return;
						}

					error = NULL;
					if (gda_connection_statement_execute_non_select (priv->commons->gdacon, stmt, NULL, NULL, &error) <= 0)
						{
							dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
							                                 GTK_DIALOG_DESTROY_WITH_PARENT,
							                                 GTK_MESSAGE_WARNING,
							                                 GTK_BUTTONS_OK,
							                                 "Error on saving.\n\n%s",
							                                 (error != NULL && error->message != NULL ? error->message : "No details."));
							gtk_dialog_run (GTK_DIALOG (dialog));
							gtk_widget_destroy (dialog);
						}

					autoz_gui_authorizations_load (authorizations);
				}
		}
	else
		{
			dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_WARNING,
			                                 GTK_BUTTONS_OK,
			                                 "Select an authorization.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
}

static void
autoz_gui_authorizations_on_trv_autoz_gui_authorizations_row_activated (GtkTreeView *tree_view,
                                             GtkTreePath *tree_path,
                                             GtkTreeViewColumn *column,
                                             gpointer user_data)
{
	AutozGuiAuthorizationsPrivate *priv = AUTOZ_GUI_AUTHORIZATIONS_GET_PRIVATE ((AutozGuiAuthorizations *)user_data);

	autoz_gui_authorizations_edit ((AutozGuiAuthorizations *)user_data);
}
