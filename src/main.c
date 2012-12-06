/*
 * Copyright (C) 2011-2012 Andrea Zagli <azagli@libero.it>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <string.h>

#include <gtk/gtk.h>

#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>

#include "commons.h"
#include "openauditds.h"
#include "roles.h"
#include "resources.h"
#include "authorizations.h"

G_MODULE_EXPORT void on_mnu_file_close_activate (GtkMenuItem *menuitem,
                            gpointer user_data);

static AutozGuiCommons *commons;

static GtkWidget *w;
static GtkWidget *vbx_body;
static GtkWidget *vbx_body_child;

static void
main_set_vbx_body_child (GtkWidget *child)
{
	if (GTK_IS_WIDGET (vbx_body_child))
		{
			gtk_container_remove (GTK_CONTAINER (vbx_body), vbx_body_child);
			gtk_widget_destroy (vbx_body_child);
		}

	vbx_body_child = child;
	gtk_box_pack_start (GTK_BOX (vbx_body), vbx_body_child, TRUE, TRUE, 0);
}

static void
main_on_ds_opened (gpointer instance, const gchar *arg1, gpointer user_data)
{
	GtkWidget *vbx;
	GError *error;

	gchar *cnc;
	gchar *pos;

	if (arg1 == NULL)
		{
			return;
		}

	cnc = g_strstrip (g_strdup (arg1));
	if (g_strcmp0 (cnc, "") == 0)
		{
			return;
		}

	pos = g_strrstr (cnc, "{prefix}");
	if (pos != NULL)
		{
			cnc = g_strndup (cnc, pos - cnc);
			commons->prefix = g_strdup (pos + 8);
		}
	else
		{
			commons->prefix = g_strdup ("");
		}

	error = NULL;
	commons->gdacon = gda_connection_open_from_string (NULL, cnc, NULL,
	                                                   GDA_CONNECTION_OPTIONS_NONE,
	                                                   &error);
	if (!commons->gdacon || error != NULL)
		{
			GtkWidget *dialog;
			dialog = gtk_message_dialog_new (GTK_WINDOW (w),
			                                 GTK_DIALOG_DESTROY_WITH_PARENT,
			                                 GTK_MESSAGE_WARNING,
			                                 GTK_BUTTONS_OK,
			                                 "Unable to connect to the choosen autoz db.\n\n%s\n\n%s",
			                                 arg1, 
			                                 (error != NULL && error->message != NULL ? error->message : "No details."));
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			return;
		}

	commons->gdaparser = gda_connection_create_parser (commons->gdacon);
	if (!commons->gdaparser)
		{
			/* @cnc doe snot provide its own parser => use default one */
			commons->gdaparser = gda_sql_parser_new ();
		}

	gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (commons->gtkbuilder, "menuitem2")), TRUE);
	gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (commons->gtkbuilder, "menuitem3")), TRUE);

	g_free (cnc);
}

G_MODULE_EXPORT void
on_mnu_file_open_audit_ds_activate (GtkMenuItem *menuitem,
                            gpointer user_data)
{
	GtkWidget *vbx;

	if (vbx_body_child != NULL)
		{
			on_mnu_file_close_activate (NULL, user_data);
		}

	OpenAuditDS *m = open_audit_ds_new (commons);

	vbx = open_audit_ds_get_widget (m);

	g_signal_connect (G_OBJECT (m), "opened",
	                  G_CALLBACK (main_on_ds_opened), NULL);

	gtk_window_set_transient_for (GTK_WINDOW (vbx), GTK_WINDOW (w));
	gtk_widget_show_all (vbx);
}

G_MODULE_EXPORT void
on_mnu_file_close_activate (GtkMenuItem *menuitem,
                            gpointer user_data)
{
	gtk_widget_destroy (vbx_body_child);
	vbx_body_child = NULL;

	gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (commons->gtkbuilder, "menuitem2")), FALSE);
	gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (commons->gtkbuilder, "menuitem3")), FALSE);
}

G_MODULE_EXPORT void
on_mnu_view_roles_activate (GtkMenuItem *menuitem,
                            gpointer user_data)
{
	GtkWidget *vbx;

	Roles *m = roles_new (commons, FALSE);

	vbx = roles_get_widget (m);

	main_set_vbx_body_child (vbx);
}

G_MODULE_EXPORT void
on_mnu_view_resources_activate (GtkMenuItem *menuitem,
                            gpointer user_data)
{
	GtkWidget *vbx;

	Resources *m = resources_new (commons, FALSE);

	vbx = resources_get_widget (m);

	main_set_vbx_body_child (vbx);
}

G_MODULE_EXPORT void
on_mnu_view_authorizations_activate (GtkMenuItem *menuitem,
                            gpointer user_data)
{
	GtkWidget *vbx;

	Authorizations *m = authorizations_new (commons, FALSE);

	vbx = authorizations_get_widget (m);

	main_set_vbx_body_child (vbx);
}

G_MODULE_EXPORT void
on_mnu_help_about_activate (GtkMenuItem *menuitem,
                            gpointer user_data)
{
	GError *error;
	GtkWidget *diag;

	error = NULL;
	gtk_builder_add_objects_from_file (commons->gtkbuilder, commons->guifile,
	                                   g_strsplit_set ("dlg_about", "|", -1),
	                                   &error);
	if (error != NULL)
		{
			g_error ("Error: %s.", error->message);
		}

	diag = GTK_WIDGET (gtk_builder_get_object (commons->gtkbuilder, "dlg_about"));
	gtk_window_set_transient_for (GTK_WINDOW (diag), GTK_WINDOW (w));
	gtk_dialog_run (GTK_DIALOG (diag));
	gtk_widget_destroy (diag);
}

int
main (int argc, char *argv[])
{
	GError *error;

	gtk_init (&argc, &argv);

	gda_init ();

	/* inizializzazione commons */
	commons = g_malloc0 (sizeof (AutozGuiCommons));

#ifdef G_OS_WIN32

	gchar *moddir;
	gchar *p;

	moddir = g_win32_get_package_installation_directory_of_module (NULL);

	p = strrchr (moddir, G_DIR_SEPARATOR);
	if (p != NULL
	    && g_ascii_strcasecmp (p + 1, "src") == 0)
		{
			commons->guidir = g_strdup (GUIDIR);
		}
	else
		{
			commons->guidir = g_build_filename (moddir, "share", PACKAGE, "gui", NULL);
		}

#else

	commons->guidir = g_strdup (GUIDIR);

#endif

	commons->guifile = g_build_filename (commons->guidir, "autoz-gui.ui", NULL);

	commons->gtkbuilder = gtk_builder_new ();

	error = NULL;
	gtk_builder_add_objects_from_file (commons->gtkbuilder, commons->guifile,
	                                   g_strsplit_set ("w_main", "|", -1),
	                                   &error);
	if (error != NULL)
		{
			g_error ("Error: %s", error->message);
		}

	gtk_builder_connect_signals (commons->gtkbuilder, NULL);

	w = GTK_WIDGET (gtk_builder_get_object (commons->gtkbuilder, "w_main"));

	vbx_body = GTK_WIDGET (gtk_builder_get_object (commons->gtkbuilder, "vbx_body"));

	vbx_body_child = NULL;

	gtk_widget_show (w);

	gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (commons->gtkbuilder, "menuitem2")), FALSE);
	gtk_widget_set_sensitive (GTK_WIDGET (gtk_builder_get_object (commons->gtkbuilder, "menuitem3")), FALSE);

	if (argc > 0)
		{
			/* try to use the first argument of command line as a cnc_string */
			main_on_ds_opened (NULL, argv[1], NULL);
		}

	gtk_main ();

	return 0;
}
