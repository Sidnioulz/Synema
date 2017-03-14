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
 * \file frame-utilities.c
 * \brief Source code for the Synema frames' utility functions
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains the source code of the frame-utilities.h functions.
 */
#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>

#include "application-data.h"
#include "frame-common.h"
#include "frame-private.h"
#include "frame-utilities.h"
#include "frame-new-dialog.h"
#include "frame-private.h"
#include "gedit-spinner.h"
#include "machine.h"
#include "time-period.h"


GtkWidget *frame_get_info_bar (frame_t *frame)
{
	g_return_val_if_fail (frame != NULL, NULL);

	GtkWidget	*info_bar		= NULL;
	GList		*children		= NULL;
	GList		*iter			= NULL;

	children = gtk_container_get_children (GTK_CONTAINER (gtk_builder_get_object (frame->builder, "root_box")));
	if (children) {
		iter = children;
		do {
			if (GTK_IS_INFO_BAR (iter->data)) {
				info_bar = iter->data;
				iter = NULL;
			}
		} while ((iter = g_list_next (iter)) != NULL);
	}
	g_list_free (children);

	return info_bar;
}



void frame_display_error_message (frame_t *frame, frame_error_t type, const gchar *message, ...)
{
	GList		*children		= NULL;
	gchar		*buffer			= NULL;
	gchar		*final			= NULL;
	GtkWidget	*info_bar		= frame_get_info_bar (frame);
	va_list		args;

	va_start (args, message);
	buffer = g_strdup_vprintf (message, args);
	final = g_strdup_printf ("<b>Error</b>\n\n%s", buffer);
	va_end (args);


	frame->last_err = type;

	gtk_widget_set_size_request (info_bar, frame_get_width (frame), frame_get_height (frame));
	gtk_widget_hide ((GtkWidget *) gtk_builder_get_object (frame->builder, "drawing_area"));


	children = gtk_container_get_children (GTK_CONTAINER (gtk_info_bar_get_content_area (GTK_INFO_BAR (info_bar))));
	if (children) {
		gtk_label_set_markup (GTK_LABEL (children->data), final);
		g_list_free (children);
	}

	gtk_widget_set_tooltip_markup (info_bar, buffer);
	gtk_widget_show (info_bar);
	g_free (buffer);
	g_free (final);
}



/** \fn void frame_func_button_clicked (GtkButton *, gpointer)
 * \brief Makes a frame's menu pop up.
 *
 * This function shows the functions menu for a given frame, when it's menu
 * button is clicked.
 * @param[in] button the button that was clicked
 * @param[in] user_data a pointer the frame whose menu will be shown
 */
static void frame_func_button_clicked (GtkButton *button, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	GtkWidget *menu = (GtkWidget *) gtk_builder_get_object (f->builder, "func_menu");
	
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time ());
}



static void frame_detach_menu (GtkWidget *button, GtkMenu *menu)
{}



double frame_get_height (frame_t *f)
{
	switch (f->size) {
		case NORMAL:
			return FRAME_HEIGHT_NORMAL;
			break;
		case LARGE:
			return FRAME_HEIGHT_LARGE;
			break;
		case FULL:
		case LAST_SIZE_RATIO_T:
			return 1.0f;
	}
	return 1.0f;
}



double frame_get_width (frame_t *f)
{
	switch (f->size) {
		case NORMAL:
			return FRAME_WIDTH_NORMAL;
			break;
		case LARGE:
			return FRAME_WIDTH_LARGE;
			break;
		case FULL:
		case LAST_SIZE_RATIO_T:
			return 1.0f;
	}
	return 1.0f;
}



gboolean frame_get_position (frame_t *frame, guint *position)
{
	g_return_val_if_fail (frame != NULL, FALSE);
	g_return_val_if_fail (frame->parent != NULL, FALSE);
	g_return_val_if_fail (position != NULL, FALSE);

	gint	stack		= 0;

	if ((*position = g_list_index (frame->parent->frames_list, frame)) == -1) {
		if ((stack = stack_search (frame->parent->hidden_stack, frame)) == -1)
			return FALSE;
		else
			*position = g_list_length (frame->parent->frames_list) + stack;
	}

	return TRUE;
}



void frame_request_redraw (frame_t *f)
{
	GtkWidget *area = (GtkWidget *) gtk_builder_get_object (f->builder, "drawing_area");
	
	if (!f->hidden)
		gtk_widget_queue_draw (area);
}



void frame_request_content_refresh (frame_t *f)
{
	if (!f->hidden) {
		f->computing_requested = TRUE;
		g_cond_signal (f->cond);
	}
}



/*void frame_function_menu_used (GtkWidget *menu, gpointer data)*/
/*{*/
/*	frame_t *f = (frame_t *) data;*/

/*	f->menu_used_once = TRUE;*/

/*	g_signal_handlers_disconnect_by_func (menu, frame_function_menu_used, data);*/
/*}*/



static void _frame_unset_error (GtkInfoBar *info_bar, gint response_id, gpointer user_data)
{
	frame_t	*frame	= user_data;

	frame->last_err = ERROR_NONE;
	gtk_widget_hide (GTK_WIDGET (info_bar));
/*	gtk_widget_set_size_request ((GtkWidget *) gtk_builder_get_object (frame->builder, "drawing_area"),*/
/*									frame_get_width (f),*/
/*									frame_get_height (f));*/
	gtk_widget_show ((GtkWidget *) gtk_builder_get_object (frame->builder, "drawing_area"));
}



void frame_unset_error (frame_t *frame)
{
	g_return_if_fail (frame != NULL);

	_frame_unset_error (GTK_INFO_BAR (frame_get_info_bar (frame)), GTK_RESPONSE_OK, frame);
}



int frame_initialise (frame_t *f,
							size_ratio_t size,
							machine_t *display_machine,
							time_period_t *pref_period)
{
	synema_instance_t	*inst			= synema_instance ();
	gchar				*framepath		= NULL;
	gchar				*label_text		= NULL;
	GError 				*err 			= NULL;
	GList				*func_list		= NULL;
	GList				*machines_list	= NULL;
	GList		 		*time_list		= NULL;
	GList		 		*time_list_iter	= NULL;
	GList				*func_list_iter	= NULL;
	GtkCellRenderer 	*renderer 		= NULL;
	GtkListStore 		*store 			= NULL;
	GtkTreeIter			iter;
	GtkWidget 			*area 			= NULL;	
	GtkWidget			*content_area	= NULL;
	GtkWidget 			*childmenu		= NULL;	
	GtkWidget			*func_btn		= NULL;	
	GtkWidget			*func_menu		= NULL;
	GtkWidget			*info_bar		= NULL;
	GtkWidget 			*label 			= NULL;
	GtkWidget 			*machine 		= NULL;
	GtkWidget 			*period			= NULL;
	GtkWidget			*root_box		= NULL;
	GtkWidget 			*size_btn 		= NULL;	
	GtkWidget			*topbox			= NULL;


	// Initialise the interface and gtkbuilder
	f->builder = gtk_builder_new ();
	framepath = g_strdup_printf ("%s/frame.ui", inst->data_dir);
	gtk_builder_add_from_file (f->builder, framepath, &err);
	g_free (framepath);
	if (err) {
		g_warning ("frame_initialise: %s", err->message);
		g_clear_error (&err);
		return -1;
	}


	// Initialise the struct members
	f->size = size;
	f->display_machine = display_machine;
	f->id = 0; // Will be actually set when the frame is added to a list

	
	// Set the name of the frame
	label = (GtkWidget *) gtk_builder_get_object (f->builder, "frame_label");
	label_text = g_strdup_printf ("<b>%s</b>", f->get_display_name(f));
	gtk_label_set_markup (GTK_LABEL (label), label_text);
	g_free (label_text);
	
	// Fill the display_period combobox
	period = (GtkWidget *) gtk_builder_get_object (f->builder, "display_period");
	
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (period), renderer, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (period),
									renderer,
									"text",
									0,
									NULL);
	
	store = GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX(period)));
	time_list = f->build_time_periods ();
	
	// We give a default to the display_period value before going in the loop
	f->display_period.type = 0;
	f->display_period.value = 0;
	
	if (time_list != NULL) {
		time_list_iter = time_list;
		do {
				time_period_t *p = time_list_iter->data;
				char *name = time_period_get_display_name (p, TRUE);
				
				gtk_list_store_append (store, &iter);
				gtk_list_store_set (store, &iter, 0, name, 1, p->type, 2, p->value, -1);
				
				if (time_period_cmp (p, pref_period) == 0) {
					time_period_copy (p, &(f->display_period));
					gtk_combo_box_set_active_iter (GTK_COMBO_BOX(period), &iter);	
				}
						
				time_period_free (p);
				g_free (name);
			} while ((time_list_iter = g_list_next (time_list_iter)) != NULL);
	}
	
	if (gtk_combo_box_get_active (GTK_COMBO_BOX(period)) == -1)
		gtk_combo_box_set_active (GTK_COMBO_BOX(period), 0);
	
	g_list_free (time_list);
	
	g_signal_connect (period, "changed",
					G_CALLBACK (frame_time_period_changed),
					(gpointer) f);


	// Fill the display_machine combobox
	machine = (GtkWidget *) gtk_builder_get_object (f->builder, "display_machine");

	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (machine), renderer, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (machine),
									renderer,
									"text",
									0,
									NULL);
	
	machines_list = machine_get_list_for_plugin (f->type, TRUE);
	frame_populate_machine_combo (machine, machines_list, f->display_machine);
	g_list_free (machines_list);

	g_signal_connect (machine, "changed",
					G_CALLBACK (frame_machine_changed),
					(gpointer) f);


	// Connect the show menu button to it's menu
	func_btn = (GtkWidget *) gtk_builder_get_object (f->builder, "func_button");
	func_menu = (GtkWidget *) gtk_builder_get_object (f->builder, "func_menu");

	func_list = f->build_func_list ();
	
	if (func_list != NULL) {
		func_list_iter = func_list;
		gtk_menu_attach_to_widget (GTK_MENU (func_menu), func_btn, frame_detach_menu);

		g_signal_connect (func_btn, "clicked",
						G_CALLBACK (frame_func_button_clicked),
						(gpointer) f);
		
		do {
			func_list_t *fn = func_list_iter->data;
			GtkWidget *item = NULL;
			
			switch (fn->type) {
				case FUNCTION:
					item = gtk_menu_item_new_with_label (fn->name);

					g_signal_connect (item, "activate",
									G_CALLBACK (fn->data.func),
									(gpointer) f);

					gtk_menu_shell_append (GTK_MENU_SHELL (func_menu), item);
					gtk_widget_show (item);
					break;
				case SEPARATOR:
					item = gtk_separator_menu_item_new ();
					gtk_menu_shell_append (GTK_MENU_SHELL (func_menu), item);
					gtk_widget_show (item);
					break;
				case SUBMENU:
					item = gtk_menu_item_new_with_label (fn->name);
					gtk_menu_shell_append (GTK_MENU_SHELL (func_menu), item);

					childmenu = func_list_t_setup_submenu (fn, f);
					gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), childmenu);

					gtk_widget_show (item);
					break;
			}
			func_list_t_free (fn);
		} while ((func_list_iter = g_list_next (func_list_iter)) != NULL);

		g_list_free (func_list);
		
	} else {
		gtk_container_remove (GTK_CONTAINER (gtk_widget_get_parent (func_btn)), func_btn);
	}


	// Set the size of the drawing area
	area = (GtkWidget *) gtk_builder_get_object (f->builder, "drawing_area");
	gtk_widget_set_size_request (area, frame_get_width (f),
										frame_get_height (f));


	// Initialise the size button and connect to it's handler
	frame_update_size_button (f);
	size_btn = (GtkWidget *) gtk_builder_get_object (f->builder, "size_toggle");
	g_signal_connect (size_btn, "clicked",
					G_CALLBACK (frame_size_changed_event),
					(gpointer) f);


	// Allow mouse events on the drawing area and link to handlers
	// If more events are needed, students should ask which they want
	gtk_widget_add_events (area, GDK_BUTTON_PRESS_MASK);
	gtk_widget_add_events (area, GDK_BUTTON_RELEASE_MASK);
	g_signal_connect (area, "button_press_event",
					G_CALLBACK (frame_button_event),
					(gpointer) f);
	g_signal_connect (area, "button_release_event",
					G_CALLBACK (frame_button_event),
					(gpointer) f);


	// Connect to the expose event
	g_signal_connect (area, "expose_event",
					G_CALLBACK (frame_expose_handler),
					(gpointer) f);


	// Connect the exit button to the exit function
	g_signal_connect (gtk_builder_get_object (f->builder, "close_button"),
					"clicked",
					G_CALLBACK (frame_on_exit),
					(gpointer) f);


	// The signal box used to send signals between threads in the frame code
	f->signal_box = frame_signals_new ();
/*	g_object_ref_sink (f->signal_box);*/


	// Spinner indicating that computing is going on in the frame
	f->spinner_stop_src = 0;
	f->spinner_start_src = 0;
	f->computing_spinner = gedit_spinner_new ();
	gedit_spinner_set_size (GEDIT_SPINNER (f->computing_spinner), GTK_ICON_SIZE_BUTTON);
	topbox = (GtkWidget *) gtk_builder_get_object (f->builder, "top_box");
	gtk_box_pack_start (GTK_BOX (topbox), f->computing_spinner, FALSE, FALSE, 0);
	gtk_box_reorder_child (GTK_BOX (topbox), f->computing_spinner, 0);
	gtk_widget_show (f->computing_spinner);


	// Launch the computing thread and initialise its related data
	f->mutex = g_mutex_new ();
	f->cond = g_cond_new ();
	f->compute = TRUE;
	f->computing_thread = g_thread_create (frame_computing_thread,
											(gpointer) f,
											TRUE,
											&err);
	if (err) {
		g_error ("frame_initialise::g_thread_create: %s", err->message);
		g_clear_error (&err);
		return -1;
	}

	// Set a default average refresh time for the auto refresh frequency
	f->avg_refresh_time = 0;
	f->avg_refresh_timeout = 0;


	// Initialises the frame's parent to NULL and make it hidden by default
	f->parent = NULL;
	f->hidden = TRUE;
	f->hidden_item = NULL;
	f->left_slot = -1;
	f->top_slot = -1;


	// Setup the frame's info bar for errors
	f->last_err = ERROR_NONE;
	info_bar = gtk_info_bar_new ();
	gtk_widget_set_no_show_all (info_bar, TRUE);

	label = gtk_label_new ("");
	gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
	gtk_label_set_line_wrap_mode (GTK_LABEL (label), PANGO_WRAP_WORD_CHAR);
	gtk_widget_set_size_request (label, frame_get_width (f) - 100, -1);
	gtk_misc_set_alignment (GTK_MISC (label), 0.1, 0.0);
	gtk_widget_show (label);

	content_area = gtk_info_bar_get_content_area (GTK_INFO_BAR (info_bar));
	gtk_container_add (GTK_CONTAINER (content_area), label);

	gtk_info_bar_add_button (GTK_INFO_BAR (info_bar), GTK_STOCK_CLOSE, GTK_RESPONSE_OK);
	gtk_info_bar_set_message_type (GTK_INFO_BAR (info_bar), GTK_MESSAGE_ERROR);

	g_signal_connect (info_bar, "response", G_CALLBACK (_frame_unset_error), f);

	root_box = (GtkWidget *) gtk_builder_get_object (f->builder, "root_box");
	gtk_box_pack_start_defaults (GTK_BOX (root_box), info_bar);
	gtk_box_reorder_child (GTK_BOX (root_box), info_bar, 1);


	// Initialise the private data of the frame developers	
	return f->init_private (f);
}



GtkWidget *frame_get_root_widget (frame_t *f)
{
	return (GtkWidget *) gtk_builder_get_object (f->builder, "frame_widget");
}



char *frame_get_absolute_reports_path (frame_t *f, const char *relpath, const char *machine)
{
	g_return_val_if_fail (f != NULL, NULL);

	synema_instance_t		*inst		= synema_instance ();

	if (relpath)
		return g_strdup_printf ("%s/%s/"REPORTS_DIR"/%s/%s",
								inst->machines_dir,
								(machine) ? machine : machine_get_folder_name (f->display_machine),
								f->type,
								relpath);
	else
		return g_strdup_printf ("%s/%s/"REPORTS_DIR"/%s",
								inst->machines_dir,
								(machine) ? machine : machine_get_folder_name (f->display_machine),
								f->type);
}



char *frame_get_absolute_data_path (frame_t *f, const char *relpath)
{
	g_return_val_if_fail (f != NULL, NULL);

	synema_instance_t		*inst		= synema_instance ();

	if (relpath)
		return g_strdup_printf ("%s/"PLUGINS_DIR"/%s/%s", inst->data_dir, f->type, relpath);
	else
		return g_strdup_printf ("%s/"PLUGINS_DIR"/%s", inst->data_dir, f->type);
}



char *frame_get_absolute_tmp_path (frame_t *f, const char *relpath)
{
	g_return_val_if_fail (f != NULL, NULL);

	synema_instance_t		*inst		= synema_instance ();

	if (relpath)
		return g_strdup_printf ("%s/frame_%u/%s", inst->tmp_dir, f->id, relpath);
	else
		return g_strdup_printf ("%s/frame_%u", inst->tmp_dir, f->id);
}
