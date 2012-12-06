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

#include <string.h>

#include <sql-parser/gda-sql-parser.h>

#include "resources.h"
#include "resource.h"

static void resources_class_init (ResourcesClass *klass);
static void resources_init (Resources *resources);

static void resources_load (Resources *resources);
static void resources_edit (Resources *resources);

static void resources_on_resource_updated (gpointer instance, gpointer user_data);

static void resources_set_property (GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec);
static void resources_get_property (GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

static void resources_on_btn_new_clicked (GtkButton *button,
                      gpointer user_data);
static void resources_on_btn_edit_clicked (GtkButton *button,
                      gpointer user_data);
static void resources_on_btn_delete_clicked (GtkButton *button,
                        gpointer user_data);
static void resources_on_trv_resources_row_activated (GtkTreeView *tree_view,
                                             GtkTreePath *tree_path,
                                             GtkTreeViewColumn *column,
                                             gpointer user_data);

#define RESOURCES_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_RESOURCES, ResourcesPrivate))

enum
{
	COL_ID,
	COL_NAME
};

typedef struct _ResourcesPrivate ResourcesPrivate;
struct _ResourcesPrivate
	{
		AutozGuiCommons *commons;

		GtkWidget *widget;

		GtkTreeSelection *selection;
		GtkListStore *lstore_resources;
	};

G_DEFINE_TYPE (Resources, resources, G_TYPE_OBJECT)

static void
resources_class_init (ResourcesClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (ResourcesPrivate));

	object_class->set_property = resources_set_property;
	object_class->get_property = resources_get_property;
}

static void
resources_init (Resources *resources)
{
	ResourcesPrivate *priv = RESOURCES_GET_PRIVATE (resources);
}

/**
 * resources_new:
 * @commons:
 * @selection:
 *
 * Returns: the newly created #Resources object.
 */
Resources
*resources_new (AutozGuiCommons *commons, gboolean selection)
{
	GError *error;

	Resources *a = RESOURCES (g_object_new (resources_get_type (), NULL));

	ResourcesPrivate *priv = RESOURCES_GET_PRIVATE (a);

	priv->commons = commons;

	error = NULL;
	gtk_builder_add_objects_from_file (priv->commons->gtkbuilder, priv->commons->guifile,
	                                   g_strsplit (g_strconcat ("lstore_resources",
	                                                            (selection ? "|w_resources" : "|vbox7"),
	                                                            NULL),
	                                               "|", -1),
	                                   &error);
	if (error != NULL)
		{
			g_warning ("Error: %s.", error->message);
			return NULL;
		}

	priv->widget = GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, (selection ? "w_resources" : "vbox7")));
	priv->selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (gtk_builder_get_object (priv->commons->gtkbuilder, "treeview1")));
	priv->lstore_resources = GTK_LIST_STORE (gtk_builder_get_object (priv->commons->gtkbuilder, "lstore_resources"));

	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button11"),
	                  "clicked", G_CALLBACK (resources_on_btn_new_clicked), (gpointer)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button12"),
	                  "clicked", G_CALLBACK (resources_on_btn_edit_clicked), (gpointer)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button14"),
	                  "clicked", G_CALLBACK (resources_on_btn_delete_clicked), (gpointer)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "treeview1"),
	                  "row-activated", G_CALLBACK (resources_on_trv_resources_row_activated), (gpointer)a);

	if (!selection)
		{
			gtk_widget_hide (GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, "button15")));
			gtk_widget_hide (GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, "button16")));
		}

	resources_load (a);

	return a;
}

/**
 * resources_get_widget:
 * @resources:
 *
 */
GtkWidget
*resources_get_widget (Resources *resources)
{
	ResourcesPrivate *priv;

	g_return_val_if_fail (IS_RESOURCES (resources), NULL);

	priv = RESOURCES_GET_PRIVATE (resources);

	return priv->widget;
}

/* PRIVATE */
static void
resources_load (Resources *resources)
{
	GtkTreeIter iter;

	gchar *sql;

	GdaStatement *stmt;
	GError *error;
	GdaDataModel *dm;

	gint rows;
	gint row;

	ResourcesPrivate *priv = RESOURCES_GET_PRIVATE (resources);

	gtk_list_store_clear (priv->lstore_resources);

	sql = g_strdup_printf ("SELECT id, resource_id"
	                       " FROM %sresources"
	                       " ORDER BY resource_id",
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
					gtk_list_store_append (priv->lstore_resources, &iter);
					gtk_list_store_set (priv->lstore_resources, &iter,
					                    COL_ID, g_value_get_int (gda_data_model_get_value_at (dm, 0, row, NULL)),
					                    COL_NAME, gda_value_stringify (gda_data_model_get_value_at (dm, 1, row, NULL)),
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
resources_edit (Resources *resources)
{
	GtkTreeIter iter;
	guint id;

	ResourcesPrivate *priv = RESOURCES_GET_PRIVATE (resources);

	if (gtk_tree_selection_get_selected (priv->selection, NULL, &iter))
		{
			GtkWidget *w;

			gtk_tree_model_get (GTK_TREE_MODEL (priv->lstore_resources), &iter,
			                    COL_ID, &id,
			                    -1);

			Resource *c = resource_new (priv->commons, id);

			g_signal_connect (G_OBJECT (c), "updated",
			                  G_CALLBACK (resources_on_resource_updated), (gpointer)resources);

			w = resource_get_widget (c);
			gtk_window_set_transient_for (GTK_WINDOW (w), GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")));
			gtk_widget_show_all (w);
		}
	else
		{
			GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
			                                            GTK_DIALOG_DESTROY_WITH_PARENT,
			                                            GTK_MESSAGE_WARNING,
			                                            GTK_BUTTONS_OK,
			                                            "Select a role.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
}

static void
resources_on_resource_updated (gpointer instance, gpointer user_data)
{
	resources_load ((Resources *)user_data);
}

static void
resources_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	Resources *resources = RESOURCES (object);
	ResourcesPrivate *priv = RESOURCES_GET_PRIVATE (resources);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
resources_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	Resources *resources = RESOURCES (object);
	ResourcesPrivate *priv = RESOURCES_GET_PRIVATE (resources);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

/* CALLBACK */
static void
resources_on_btn_new_clicked (GtkButton *button,
                      gpointer user_data)
{
	GtkWidget *w;

	Resources *resources = (Resources *)user_data;
	ResourcesPrivate *priv = RESOURCES_GET_PRIVATE (resources);

	Resource *c = resource_new (priv->commons, 0);

	g_signal_connect (G_OBJECT (c), "updated",
	                  G_CALLBACK (resources_on_resource_updated), (gpointer)resources);

	w = resource_get_widget (c);
	gtk_window_set_transient_for (GTK_WINDOW (w), GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")));
	gtk_widget_show_all (w);
}

static void
resources_on_btn_edit_clicked (GtkButton *button,
                      gpointer user_data)
{
	resources_edit ((Resources *)user_data);
}

static void
resources_on_btn_delete_clicked (GtkButton *button,
                        gpointer user_data)
{
	GtkWidget *dialog;
	gboolean risp;

	GtkTreeIter iter;
	guint id;

	Resources *resources = (Resources *)user_data;
	ResourcesPrivate *priv = RESOURCES_GET_PRIVATE (resources);

	if (gtk_tree_selection_get_selected (priv->selection, NULL, &iter))
		{
			dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_QUESTION,
			                                 GTK_BUTTONS_YES_NO,
			                                 "Are you sure to want to delete the selected resource?");
			risp = gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			if (risp == GTK_RESPONSE_YES)
				{
					GError *error;
					GdaStatement *stmt;

					gtk_tree_model_get (GTK_TREE_MODEL (priv->lstore_resources), &iter,
					                    COL_ID, &id,
					                    -1);

					error = NULL;
					stmt = gda_sql_parser_parse_string (priv->commons->gdaparser,
					                                    g_strdup_printf ("DELETE FROM %sresources WHERE id = %d", priv->commons->prefix, id),
					                                    NULL, &error);

					if (stmt == NULL || error != NULL)
						{
							dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
							                                 GTK_DIALOG_DESTROY_WITH_PARENT,
							                                 GTK_MESSAGE_WARNING,
							                                 GTK_BUTTONS_OK,
							                                 "You must select a resource.%s",
							                                 error != NULL && error->message != NULL ? g_strdup_printf ("\n\n", error->message) : "");
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

					resources_load (resources);
				}
		}
	else
		{
			dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_WARNING,
			                                 GTK_BUTTONS_OK,
			                                 "Select a resource.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
}

static void
resources_on_trv_resources_row_activated (GtkTreeView *tree_view,
                                             GtkTreePath *tree_path,
                                             GtkTreeViewColumn *column,
                                             gpointer user_data)
{
	ResourcesPrivate *priv = RESOURCES_GET_PRIVATE ((Resources *)user_data);

	resources_edit ((Resources *)user_data);
}
