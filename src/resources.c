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

static void autoz_gui_resources_class_init (AutozGuiResourcesClass *klass);
static void autoz_gui_resources_init (AutozGuiResources *resources);

static void autoz_gui_resources_load (AutozGuiResources *resources);
static void autoz_gui_resources_edit (AutozGuiResources *resources);

static void autoz_gui_resources_selected (AutozGuiResources *resources);
static void autoz_gui_resources_on_resource_updated (gpointer instance, gpointer user_data);

static void autoz_gui_resources_set_property (GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec);
static void autoz_gui_resources_get_property (GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

static void autoz_gui_resources_on_btn_new_clicked (GtkButton *button,
                      gpointer user_data);
static void autoz_gui_resources_on_btn_edit_clicked (GtkButton *button,
                      gpointer user_data);
static void autoz_gui_resources_on_btn_delete_clicked (GtkButton *button,
                        gpointer user_data);
static void autoz_gui_resources_on_trv_autoz_gui_resources_row_activated (GtkTreeView *tree_view,
                                             GtkTreePath *tree_path,
                                             GtkTreeViewColumn *column,
                                             gpointer user_data);

static void autoz_gui_resources_on_btn_cancel_clicked (GtkButton *button,
                      gpointer user_data);
static void autoz_gui_resources_on_btn_ok_clicked (GtkButton *button,
                      gpointer user_data);

#define AUTOZ_GUI_RESOURCES_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_AUTOZ_GUI_RESOURCES, AutozGuiResourcesPrivate))

enum
{
	COL_ID,
	COL_NAME
};

typedef struct _AutozGuiResourcesPrivate AutozGuiResourcesPrivate;
struct _AutozGuiResourcesPrivate
	{
		AutozGuiCommons *commons;

		GtkWidget *widget;

		GtkTreeSelection *sel_selection;
		GtkListStore *lstore_resources;

		gboolean selection;
	};

G_DEFINE_TYPE (AutozGuiResources, autoz_gui_resources, G_TYPE_OBJECT)

static void
autoz_gui_resources_class_init (AutozGuiResourcesClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (AutozGuiResourcesPrivate));

	object_class->set_property = autoz_gui_resources_set_property;
	object_class->get_property = autoz_gui_resources_get_property;

	/**
	 * Resources::selected:
	 * @resources:
	 *
	 */
	klass->selected_signal_id = g_signal_new ("selected",
	                                               G_TYPE_FROM_CLASS (object_class),
	                                               G_SIGNAL_RUN_LAST,
	                                               0,
	                                               NULL,
	                                               NULL,
	                                               g_cclosure_marshal_VOID__UINT,
	                                               G_TYPE_NONE,
	                                               1, G_TYPE_UINT);
}

static void
autoz_gui_resources_init (AutozGuiResources *resources)
{
	AutozGuiResourcesPrivate *priv = AUTOZ_GUI_RESOURCES_GET_PRIVATE (resources);
}

/**
 * autoz_gui_resources_new:
 * @commons:
 * @selection:
 *
 * Returns: the newly created #AutozGuiResources object.
 */
AutozGuiResources
*autoz_gui_resources_new (AutozGuiCommons *commons, gboolean selection)
{
	GError *error;

	AutozGuiResources *a = AUTOZ_GUI_RESOURCES (g_object_new (autoz_gui_resources_get_type (), NULL));

	AutozGuiResourcesPrivate *priv = AUTOZ_GUI_RESOURCES_GET_PRIVATE (a);

	priv->commons = commons;
	priv->selection = selection;

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
	priv->sel_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (gtk_builder_get_object (priv->commons->gtkbuilder, "treeview1")));
	priv->lstore_resources = GTK_LIST_STORE (gtk_builder_get_object (priv->commons->gtkbuilder, "lstore_resources"));

	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button11"),
	                  "clicked", G_CALLBACK (autoz_gui_resources_on_btn_new_clicked), (gpointer)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button12"),
	                  "clicked", G_CALLBACK (autoz_gui_resources_on_btn_edit_clicked), (gpointer)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button14"),
	                  "clicked", G_CALLBACK (autoz_gui_resources_on_btn_delete_clicked), (gpointer)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "treeview1"),
	                  "row-activated", G_CALLBACK (autoz_gui_resources_on_trv_autoz_gui_resources_row_activated), (gpointer)a);

	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button15"),
	                  "clicked", G_CALLBACK (autoz_gui_resources_on_btn_cancel_clicked), (gpointer)a);
	g_signal_connect (gtk_builder_get_object (priv->commons->gtkbuilder, "button16"),
	                  "clicked", G_CALLBACK (autoz_gui_resources_on_btn_ok_clicked), (gpointer)a);

	if (!selection)
		{
			gtk_widget_hide (GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, "button15")));
			gtk_widget_hide (GTK_WIDGET (gtk_builder_get_object (priv->commons->gtkbuilder, "button16")));
		}

	autoz_gui_resources_load (a);

	return a;
}

/**
 * autoz_gui_resources_get_widget:
 * @resources:
 *
 */
GtkWidget
*autoz_gui_resources_get_widget (AutozGuiResources *resources)
{
	AutozGuiResourcesPrivate *priv;

	g_return_val_if_fail (IS_AUTOZ_GUI_RESOURCES (resources), NULL);

	priv = AUTOZ_GUI_RESOURCES_GET_PRIVATE (resources);

	return priv->widget;
}

/* PRIVATE */
static void
autoz_gui_resources_load (AutozGuiResources *resources)
{
	GtkTreeIter iter;

	gchar *sql;

	GdaStatement *stmt;
	GError *error;
	GdaDataModel *dm;

	gint rows;
	gint row;

	AutozGuiResourcesPrivate *priv = AUTOZ_GUI_RESOURCES_GET_PRIVATE (resources);

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
autoz_gui_resources_edit (AutozGuiResources *resources)
{
	GtkTreeIter iter;
	guint id;

	AutozGuiResourcesPrivate *priv = AUTOZ_GUI_RESOURCES_GET_PRIVATE (resources);

	if (gtk_tree_selection_get_selected (priv->sel_selection, NULL, &iter))
		{
			GtkWidget *w;

			gtk_tree_model_get (GTK_TREE_MODEL (priv->lstore_resources), &iter,
			                    COL_ID, &id,
			                    -1);

			AutozGuiResource *c = autoz_gui_resource_new (priv->commons, id);

			g_signal_connect (G_OBJECT (c), "updated",
			                  G_CALLBACK (autoz_gui_resources_on_resource_updated), (gpointer)resources);

			w = autoz_gui_resource_get_widget (c);
			gtk_window_set_transient_for (GTK_WINDOW (w), priv->selection ? GTK_WINDOW (priv->widget) :GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")));
			gtk_widget_show_all (w);
		}
	else
		{
			GtkWidget *dialog = gtk_message_dialog_new (priv->selection ? GTK_WINDOW (priv->widget) :GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
			                                            GTK_DIALOG_DESTROY_WITH_PARENT,
			                                            GTK_MESSAGE_WARNING,
			                                            GTK_BUTTONS_OK,
			                                            "Select a role.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
}

static void
autoz_gui_resources_on_resource_updated (gpointer instance, gpointer user_data)
{
	autoz_gui_resources_load ((AutozGuiResources *)user_data);
}

static void
autoz_gui_resources_selected (AutozGuiResources *resources)
{
	GtkTreeIter iter;
	guint id;

	AutozGuiResourcesClass *klass = AUTOZ_GUI_RESOURCES_GET_CLASS (resources);
	AutozGuiResourcesPrivate *priv = AUTOZ_GUI_RESOURCES_GET_PRIVATE (resources);

	if (gtk_tree_selection_get_selected (priv->sel_selection, NULL, &iter))
		{
			gtk_tree_model_get (GTK_TREE_MODEL (priv->lstore_resources), &iter,
			                    COL_ID, &id,
			                    -1);

			g_signal_emit (G_OBJECT (resources), klass->selected_signal_id, 0, id);

			gtk_widget_destroy (priv->widget);
			g_object_unref (G_OBJECT (resources));
		}
	else
		{
			GtkWidget *dialog = gtk_message_dialog_new (priv->selection ? GTK_WINDOW (priv->widget) : GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
			                                            GTK_DIALOG_DESTROY_WITH_PARENT,
			                                            GTK_MESSAGE_WARNING,
			                                            GTK_BUTTONS_OK,
			                                            "Select a resource.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
}

static void
autoz_gui_resources_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	AutozGuiResources *resources = AUTOZ_GUI_RESOURCES (object);
	AutozGuiResourcesPrivate *priv = AUTOZ_GUI_RESOURCES_GET_PRIVATE (resources);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
autoz_gui_resources_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AutozGuiResources *resources = AUTOZ_GUI_RESOURCES (object);
	AutozGuiResourcesPrivate *priv = AUTOZ_GUI_RESOURCES_GET_PRIVATE (resources);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

/* CALLBACK */
static void
autoz_gui_resources_on_btn_new_clicked (GtkButton *button,
                      gpointer user_data)
{
	GtkWidget *w;

	AutozGuiResources *resources = (AutozGuiResources *)user_data;
	AutozGuiResourcesPrivate *priv = AUTOZ_GUI_RESOURCES_GET_PRIVATE (resources);

	AutozGuiResource *c = autoz_gui_resource_new (priv->commons, 0);

	g_signal_connect (G_OBJECT (c), "updated",
	                  G_CALLBACK (autoz_gui_resources_on_resource_updated), (gpointer)resources);

	w = autoz_gui_resource_get_widget (c);
	gtk_window_set_transient_for (GTK_WINDOW (w), priv->selection ? GTK_WINDOW (priv->widget) :GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")));
	gtk_widget_show_all (w);
}

static void
autoz_gui_resources_on_btn_edit_clicked (GtkButton *button,
                      gpointer user_data)
{
	autoz_gui_resources_edit ((AutozGuiResources *)user_data);
}

static void
autoz_gui_resources_on_btn_delete_clicked (GtkButton *button,
                        gpointer user_data)
{
	GtkWidget *dialog;
	gboolean risp;

	GtkTreeIter iter;
	guint id;

	AutozGuiResources *resources = (AutozGuiResources *)user_data;
	AutozGuiResourcesPrivate *priv = AUTOZ_GUI_RESOURCES_GET_PRIVATE (resources);

	if (gtk_tree_selection_get_selected (priv->sel_selection, NULL, &iter))
		{
			dialog = gtk_message_dialog_new (priv->selection ? GTK_WINDOW (priv->widget) :GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
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
							dialog = gtk_message_dialog_new (priv->selection ? GTK_WINDOW (priv->widget) :GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
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
							dialog = gtk_message_dialog_new (priv->selection ? GTK_WINDOW (priv->widget) :GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
							                                 GTK_DIALOG_DESTROY_WITH_PARENT,
							                                 GTK_MESSAGE_WARNING,
							                                 GTK_BUTTONS_OK,
							                                 "Error on saving.\n\n%s",
							                                 (error != NULL && error->message != NULL ? error->message : "No details."));
							gtk_dialog_run (GTK_DIALOG (dialog));
							gtk_widget_destroy (dialog);
						}

					autoz_gui_resources_load (resources);
				}
		}
	else
		{
			dialog = gtk_message_dialog_new (priv->selection ? GTK_WINDOW (priv->widget) :GTK_WINDOW (gtk_builder_get_object (priv->commons->gtkbuilder, "w_main")),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_WARNING,
			                                 GTK_BUTTONS_OK,
			                                 "Select a resource.");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		}
}

static void
autoz_gui_resources_on_trv_autoz_gui_resources_row_activated (GtkTreeView *tree_view,
                                             GtkTreePath *tree_path,
                                             GtkTreeViewColumn *column,
                                             gpointer user_data)
{
	AutozGuiResourcesPrivate *priv = AUTOZ_GUI_RESOURCES_GET_PRIVATE ((AutozGuiResources *)user_data);

	if (priv->selection)
		{
			autoz_gui_resources_selected ((AutozGuiResources *)user_data);
		}
	else
		{
			autoz_gui_resources_edit ((AutozGuiResources *)user_data);
		}
}

static void
autoz_gui_resources_on_btn_cancel_clicked (GtkButton *button,
                      gpointer user_data)
{
	AutozGuiResourcesPrivate *priv = AUTOZ_GUI_RESOURCES_GET_PRIVATE ((AutozGuiResources *)user_data);

	gtk_widget_destroy (priv->widget);
	g_object_unref (G_OBJECT (user_data));
}

static void
autoz_gui_resources_on_btn_ok_clicked (GtkButton *button,
                      gpointer user_data)
{
	autoz_gui_resources_selected ((AutozGuiResources *)user_data);
}
