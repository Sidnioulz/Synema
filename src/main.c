/* Copyright 2009 (c) ENSI de Bourges
 * 		88 boulevard Lahitolle, 18020 Bourges Cedex, France
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * \file main.c
 * \brief Source code of the application.
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains the source of the whole application. It should not be
 * modified unless you know what you do.
 */
#include <glib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <librsvg/rsvg.h>
#include <stdio.h>
#include <stdlib.h>

#include "application-data.h"
#include "constants.h"
#include "frame-common.h"
#include "frame-actions.h"
#include "frame-new-dialog.h"
#include "frame-open-panel.h"
#include "frame-saving.h"
#include "frame-table.h"
#include "log-player.h"
#include "notebook.h"



/** \fn int main (int argc, char *argv[])
 * \brief The main function.
 *
 * \warning Do not touch the code in this function, except for the customisable
 * part separated with comments.
 *
 * This is the main function. You should use it only to create and initialise
 * frames for testing purpose.
 * @param[in] argc the number of command line arguments
 * @param[in] argv an array containing the arguments
 * @return EXIT_SUCCESS if everything goes well, EXIT_FAILURE otherwise
 */
int main (int argc, char *argv[])
{
	synema_instance_t	*inst			= NULL;
	GtkWidget	 		*window 		= NULL;
	GtkWidget			*add_tab_button	= NULL;
	GtkWidget			*frame_box		= NULL;
	
	g_thread_init (NULL);
	gdk_threads_init();
	gdk_threads_enter ();
	gtk_init (&argc, &argv);
	rsvg_init ();
	dbus_g_thread_init ();
	g_set_application_name (APP_NAME);
	inst = synema_instance ();


	// The window used by frame developers
	window = (GtkWidget *) gtk_builder_get_object (inst->builder, "synema_window");
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (pre_quit_save_frames), NULL);


	// Connecting toolbar buttons to handlers
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "toolbutton_add"),
						"clicked", G_CALLBACK (frame_on_new), 
						(gpointer) inst->builder);
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "toolbutton_player"),
						"clicked", G_CALLBACK (toggle_panels), GINT_TO_POINTER (PANEL_PLAYER));
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "toolbutton_open_tab"),
						"clicked", G_CALLBACK (toggle_panels), GINT_TO_POINTER (PANEL_OPEN));
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "toolbutton_actions"),
						"clicked", G_CALLBACK (toggle_panels), GINT_TO_POINTER (PANEL_ACTIONS));
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "toolbutton_save_tab"),
						"clicked", G_CALLBACK (save_frames_dialog_show), NULL);
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "toolbutton_quit"),
						"clicked", G_CALLBACK (pre_quit_save_frames), NULL);


	// We put the widgets in their respective containers and connect signals to handlers
	add_tab_button = (GtkWidget *) gtk_builder_get_object (inst->builder, "toolbutton_add_tab");
	frame_box = (GtkWidget *) gtk_builder_get_object (inst->builder, "notebook");
	g_signal_connect (frame_box, "page_added", G_CALLBACK (notebook_on_page_added), NULL);
	g_signal_connect (frame_box, "page_removed", G_CALLBACK (notebook_on_page_removed), NULL);
	g_signal_connect (frame_box, "switch_page", G_CALLBACK (notebook_on_page_switch), NULL);
	g_signal_connect (add_tab_button, "clicked", G_CALLBACK (notebook_add_page), frame_box);
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "toolbutton_settings"), "clicked", G_CALLBACK (settings_dialog_show), NULL);
	g_signal_connect ((GtkWidget *) gtk_builder_get_object (inst->builder, "toolbutton_about"), "clicked", G_CALLBACK (about_dialog), NULL);


	// Load previously saved frames tables and frames
	if (inst->settings->autosave_frames)
		post_init_restore_frames ();

	// If nothing was loaded, put an empty frames table
	if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (frame_box)) == 0)
		_notebook_add_page (frame_box, NULL);


	// Make sure all widgets are shown
	gtk_window_maximize (GTK_WINDOW (window));
	gtk_widget_show_all (window);


	// Hide the side panel and setup the panels' widgets
	frame_action_setup_widgets ();
	frame_open_setup_widgets ();
	player_setup_widgets ();
	toggle_panels (GTK_TOOL_BUTTON ((GtkWidget *) gtk_builder_get_object (inst->builder, "toolbutton_player")), GINT_TO_POINTER (PANEL_NONE));

	gtk_main ();

	synema_instance_free ();
	rsvg_term ();
	gdk_threads_leave ();

	return EXIT_SUCCESS;
}
