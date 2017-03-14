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
 * \file frame-private.c
 * \brief Source code for the Synema frames' private functions
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This source code file contains private utility functions for Synema
 * frames.
 */
#include <errno.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>

#include "application-data.h"
#include "frame-common.h"
#include "frame-private.h"
#include "frame-signals.h"
#include "frame-table.h"
#include "frame-utilities.h"
#include "gedit-spinner.h"
#include "machine.h"
#include "plugins.h"
#include "table-utilities.h"
#include "time-period.h"



void frame_set_id (frame_t *f, GList *list)
{
	synema_instance_t *inst = synema_instance ();
	f->id = unique_frame_id_generator (list);
	gchar *path = g_strdup_printf ("%s/frame_%u", inst->tmp_dir, f->id);

	if (g_mkdir_with_parents (path, 0777) != 0) {
		g_warning ("frame_set_id: Could not create a tmp directory, the plugin may encounter problems (%s)",
				g_strerror (errno));
	}
	g_free (path);
}



void frame_on_exit (GtkWidget *button, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;

	frame_table_remove (f->parent, f);
}



frame_t *frame_new (char *type)
{
	gboolean	deprecated	= FALSE;
	frame_t		*frame		= g_malloc (sizeof (frame_t));
	plugin_t	*pt			= plugin_list_find (type);

	*(void **) (&(frame->build_func_list)) = plugin_get_symbol (pt, "build_func_list", TRUE);
	if (!frame->build_func_list) {
		g_warning ("frame_new: Failed to get mandatory symbol build_func_list");
		g_free (frame);
		return NULL;
	}

	*(void **) (&(frame->build_time_periods)) = plugin_get_symbol (pt, "build_time_periods", TRUE);
	if (!frame->build_time_periods) {
		g_warning ("frame_new: Failed to get mandatory symbol build_time_periods");
		g_free (frame);
		return NULL;
	}

	*(void **) (&(frame->is_refreshing_needed)) = plugin_get_symbol (pt, "is_refreshing_needed", TRUE);
	if (!frame->is_refreshing_needed) {
		g_warning ("frame_new: Failed to get mandatory symbol is_refreshing_needed");
		g_free (frame);
		return NULL;
	}

	*(void **) (&(frame->refresh_content)) = plugin_get_symbol (pt, "refresh_content", TRUE);
	if (!frame->refresh_content) {
		g_warning ("frame_new: Failed to get mandatory symbol refresh_content");
		g_free (frame);
		return NULL;
	}

	*(void **) (&(frame->init_private)) = plugin_get_symbol (pt, "init_private", TRUE);
	if (!frame->init_private) {
		g_warning ("frame_new: Failed to get mandatory symbol init_private");
		g_free (frame);
		return NULL;
	}

	*(void **) (&(frame->free_private)) = plugin_get_symbol (pt, "free_private", TRUE);
	if (!frame->free_private) {
		g_warning ("frame_new: Failed to get mandatory symbol free_private");
		g_free (frame);
		return NULL;
	}

	*(void **) (&(frame->display_machine_changed)) = plugin_get_symbol (pt, "display_machine_changed", TRUE);
	if (!frame->display_machine_changed) {
		g_warning ("frame_new: Failed to get mandatory symbol display_machine_changed");
		g_free (frame);
		return NULL;
	}

	*(void **) (&(frame->time_period_changed)) = plugin_get_symbol (pt, "time_period_changed", TRUE);
	if (!frame->time_period_changed) {
		g_warning ("frame_new: Failed to get mandatory symbol time_period_changed");
		g_free (frame);
		return NULL;
	}

	*(void **) (&(frame->frame_size_changed)) = plugin_get_symbol (pt, "frame_size_changed", TRUE);
	if (!frame->frame_size_changed) {
		g_warning ("frame_new: Failed to get mandatory symbol frame_size_changed");
		g_free (frame);
		return NULL;
	}

	*(void **) (&(frame->draw_area)) = plugin_get_symbol (pt, "draw_area", TRUE);
	if (!frame->draw_area) {
		g_warning ("frame_new: Failed to get mandatory symbol draw_area");
		g_free (frame);
		return NULL;
	}

	*(void **) (&(frame->get_display_name)) = plugin_get_symbol (pt, "get_display_name", TRUE);
	if (!frame->get_display_name) {
		g_warning ("frame_new: Failed to get mandatory symbol get_display_name");
		g_free (frame);
		return NULL;
	}


	*(void **) (&(frame->button_press_handler)) = plugin_get_symbol (pt, "button_press_handler", FALSE);
	if (!frame->button_press_handler) {
		g_warning ("frame_new: Failed to get optional symbol button_press_handler");
		deprecated = TRUE;
	}
	
	*(void **) (&(frame->button_release_handler)) = plugin_get_symbol (pt, "button_release_handler", FALSE);
	if (!frame->button_release_handler) {
		g_warning ("frame_new: Failed to get optional symbol button_release_handler");
		deprecated = TRUE;
	}
	
	*(void **) (&(frame->restore_private)) = plugin_get_symbol (pt, "restore_private", FALSE);
	if (!frame->restore_private) {
		g_warning ("frame_new: Failed to get optional symbol restore_private");
		deprecated = TRUE;
	}
	
	*(void **) (&(frame->save_private)) = plugin_get_symbol (pt, "save_private", FALSE);
	if (!frame->save_private) {
		g_warning ("frame_new: Failed to get optional symbol save_private");
		deprecated = TRUE;
	}


	if (deprecated)
		g_warning ("frame_new: plugin %s uses a deprecated API version", type);

	frame->type = g_strdup (type);
	return frame;
}



void frame_free (frame_t *f)
{
	if (f) {
		f->compute = FALSE;
		g_cond_signal (f->cond);
		g_thread_join (f->computing_thread);
		g_mutex_free (f->mutex);
		g_cond_free (f->cond);
		f->free_private (f);
		if (f->spinner_stop_src)
			g_source_remove (f->spinner_stop_src);
		if (f->spinner_start_src)
			g_source_remove (f->spinner_start_src);
		g_object_unref (f->signal_box);
		g_object_unref (f->builder);
		g_free (f->type);
		g_free (f);
	}
}



void frame_free_uninitialised (frame_t *f)
{
	if (f) {
		g_free (f->type);
		g_free (f);
	}
}



double frame_timed_refresh_content (frame_t *f, int *call_result)
{
	GTimer *chrono = g_timer_new ();

	if (call_result != NULL)
		*call_result = f->refresh_content (f);
	else
		f->refresh_content (f);

	g_timer_stop (chrono);

	double result = g_timer_elapsed (chrono, NULL);

	g_timer_destroy (chrono);
	
	return result;
}



void frame_update_size_button (frame_t *f)
{
	GtkWidget 			*button 		= NULL;	
	GtkWidget 			*size_image		= NULL;	

	button = (GtkWidget *) gtk_builder_get_object (f->builder, "size_toggle");

	if (f->size == NORMAL)
		size_image = gtk_image_new_from_stock ("gtk-zoom-in", GTK_ICON_SIZE_BUTTON);
	else if (f->size == LARGE)
		size_image = gtk_image_new_from_stock ("gtk-zoom-out", GTK_ICON_SIZE_BUTTON);
	else // FULL SCREEN
		size_image = gtk_image_new_from_stock ("gtk-leave-fullscreen", GTK_ICON_SIZE_BUTTON);

	gtk_button_set_image (GTK_BUTTON (button), size_image);
}



void frame_machine_changed (GtkComboBox *box, gpointer user_data)
{
	frame_t *f = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model;

	f = (frame_t *) user_data;

	if (gtk_combo_box_get_active_iter (box, &iter)) {
		model = gtk_combo_box_get_model (box);
		gtk_tree_model_get (model, &iter, 1, &f->display_machine, -1);
	}

	if (f->last_err == ERROR_MACHINE)
		frame_unset_error (f);

	f->display_machine_changed (f);
}



void frame_time_period_changed (GtkComboBox *box, gpointer user_data)
{
	frame_t *f = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model;

	f = (frame_t *) user_data;

	if (gtk_combo_box_get_active_iter (box, &iter)) {
		model = gtk_combo_box_get_model (box);
		gtk_tree_model_get (model, &iter, 1, &f->display_period.type, 2, &f->display_period.value, -1);
	}

	if (f->last_err == ERROR_PERIOD)
		frame_unset_error (f);

	f->time_period_changed (f);
}



gboolean frame_button_event (GtkWidget *area, 
                                GdkEventButton *event,
                                gpointer user_data)
{
	frame_t *frame = user_data;
	// XXX v0.1 plugins compatibility
	if (event->type == GDK_BUTTON_PRESS) {
		if (frame->button_press_handler)
			frame->button_press_handler (frame, event);
		else
			g_warning ("frame_button_event: No button_press_handler function in frame %u, deprecated plugin (%s) format (0.3.1)", frame->id, frame->type);
	} else if (event->type == GDK_BUTTON_RELEASE) {
		if (frame->button_release_handler)
			frame->button_release_handler (frame, event);
		else
			g_warning ("frame_button_event: No button_release_handler function in frame %u, deprecated plugin (%s) format (0.3.1)", frame->id, frame->type);
	}

	return FALSE;
}



gboolean frame_expose_handler (GtkWidget *area,
								GdkEventExpose *event,
								gpointer user_data)
{
	frame_t *f = NULL;
	cairo_t *cr = NULL;

	f = (frame_t *) user_data;

	cr = gdk_cairo_create (area->window);
	cairo_rectangle (cr, event->area.x, event->area.y, 
					event->area.width, event->area.height);
	cairo_clip (cr);

	f->draw_area (f, cr);

	cairo_destroy (cr);

	return FALSE;
}



void frame_size_changed_event (GtkWidget *button, gpointer user_data)
{
	g_return_if_fail (user_data != NULL);

	frame_t			*f			= user_data;
	GtkWidget		*area		= NULL;
	size_ratio_t	new_size;

	if (f->size == NORMAL)
		new_size = LARGE;
	else 
		new_size = NORMAL;

	// Telling the table to change the frame's size, and doing the size change
	table_frame_size_changed (f, new_size);

	// Updating the frame itself: drawing area and size change button
	frame_update_size_button (f);
	area = (GtkWidget *) gtk_builder_get_object (f->builder, "drawing_area");
	gtk_widget_set_size_request (area, frame_get_width (f), frame_get_height (f));
	gtk_widget_queue_resize (frame_get_root_widget (f));

	f->frame_size_changed (f);
}



static gboolean frame_spinner_stop (gpointer data)
{
	frame_t	*f	= (frame_t *) data;
	g_signal_emit_by_name (f->signal_box, "spinner_stop", data);
	return FALSE;
}



static gboolean frame_spinner_start (gpointer data)
{
	frame_t	*f	= (frame_t *) data;
	g_signal_emit_by_name (f->signal_box, "spinner_start", data);
	return FALSE;
}



gpointer frame_computing_thread (gpointer data)
{
	synema_instance_t	*inst		= synema_instance ();
	frame_t				*f			= (frame_t *) data;
	gboolean			do_refresh	= FALSE;
	GTimeVal			timeval;

	while (f->compute) {
		g_mutex_lock (f->mutex);

		g_get_current_time (&timeval);
		if (inst->settings->refresh_freq == REFRESH_FREQ_AUTO) {
			/* When timeout is 0, we decrease it X times by 1, so we can simulate
			 * X passes of an algorithm that will calculate the total waiting time
			 * before a refreshing was needed. Once we did that X times, we can
			 * divide the sum by X to get an average waiting time between each
			 * refresh. Then, we will set timeout to 10 minutes and decrease it
			 * each time by the average time found.
			 */
			if (f->avg_refresh_timeout <= 0) {
				// Initialising the calculation mode
				GTimer *elapsed = g_timer_new ();
				g_timer_start (elapsed);
				if (f->avg_refresh_timeout == 0)
					f->avg_refresh_time = 0;

				// Calculating the time till next refresh
				while (!f->is_refreshing_needed (f)) {
					g_get_current_time (&timeval);
					g_time_val_add (&timeval, 250000);
					g_cond_timed_wait (f->cond, f->mutex, &timeval);
				}
				
				g_timer_stop (elapsed);
				f->avg_refresh_time += g_timer_elapsed (elapsed, NULL);
				g_timer_destroy (elapsed);
				do_refresh = TRUE;

				// Now set timeout and go back into normal mode, if the number of
				// passes has been reached (remember we start and 0 and decrease
				// by one in each pass, so there have been X passes if we have
				// timeout at - (X-1))
				if (f->avg_refresh_timeout == -(AVG_REFRESH_TIME_NB_PASS - 1)) {
					f->avg_refresh_timeout = AUTO_REFRESH_FREQ_TIMEOUT_LEN;
					f->avg_refresh_time /= AVG_REFRESH_TIME_NB_PASS;
				} else {
					f->avg_refresh_timeout--;
				}
			}
			//Timeout > 0, remaining time before next recalculation of average refresh freq
			else {
				f->avg_refresh_timeout -= f->avg_refresh_time;
				if (f->avg_refresh_timeout < 0)
					f->avg_refresh_timeout = 0; //Dont go in negatives to not screw the recalculation algorithm
				g_time_val_add (&timeval, f->avg_refresh_time * G_USEC_PER_SEC);
				g_cond_timed_wait (f->cond, f->mutex, &timeval);
				do_refresh = f->is_refreshing_needed (f);
			}
		}
			// We use a fixed refresh frequency here
		else {
			g_time_val_add (&timeval, inst->settings->refresh_freq * 1000); //Turn into usec
			g_cond_timed_wait (f->cond, f->mutex, &timeval);
			do_refresh = f->is_refreshing_needed (f);
		}

		// This while statement allows requesting new recomputes while the content is
		// already being refreshed
		if ((f->compute) && (do_refresh) && (!f->hidden)) {
			f->show_spinner = TRUE;
			f->spinner_start_src = gdk_threads_add_idle (frame_spinner_start, data);
			do {
				f->computing_requested = FALSE;
				f->refresh_content (f);
			} while (f->computing_requested);
			f->show_spinner = FALSE;
			f->spinner_stop_src = gdk_threads_add_idle (frame_spinner_stop, data);
		}

		g_mutex_unlock (f->mutex);
	}

	return NULL;
}
