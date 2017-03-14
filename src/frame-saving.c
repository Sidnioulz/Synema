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
 * \file frame-saving.c
 * \brief Source code for saving and restoring Synema frames
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This source code file contains functions for saving and loading Synema
 * frames.
 */

#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <libtar.h>

#include "application-data.h"
#include "frame-common.h"
#include "frame-private.h"
#include "frame-saving.h"
#include "frame-utilities.h"
#include "machine.h"
#include "notebook.h"
#include "time-period.h"



/** TODO doc */
static void frame_save_private (frame_t *frame, GKeyFile *file)
{
	// XXX v0.1 plugins compatibility
	if (frame->save_private)
		frame->save_private (frame, file);
	else
		g_warning ("frame_save_private: No save_private function in frame %u, deprecated plugin (%s) format", frame->id, frame->type);
}



/** TODO doc */
static void frame_restore_private (frame_t *frame, GKeyFile *file)
{
	// XXX v0.1 plugins compatibility
	if (frame->restore_private)
		frame->restore_private (frame, file);
	else
		g_warning ("frame_restore_private: No restore_private function in frame %u, deprecated plugin (%s) format", frame->id, frame->type);
}



GKeyFile *frame_to_key_file (frame_t *frame, guint position)
{
	gchar		*size_label		= NULL;
	GKeyFile	*keyfile		= g_key_file_new ();
	gint		*list_buffer	= g_malloc (sizeof (gint) * 2);

	// Save frame settings
	g_key_file_set_string (keyfile, FTGKF_GROUP_FRAME, FTGKF_TYPE, frame->type);

	switch (frame->size) {
		case LARGE:
			size_label = g_strdup ("LARGE");
			break;
		case NORMAL:
			size_label = g_strdup ("NORMAL");
			break;
		case FULL:
			size_label = g_strdup ("FULL");
			break;
		default:
			size_label = g_strdup ("INVALID");
			break;
	}
	g_key_file_set_string (keyfile, FTGKF_GROUP_FRAME, FTGKF_SIZE, size_label);
	g_free (size_label);

	g_key_file_set_string (keyfile, FTGKF_GROUP_FRAME, FTGKF_MACHINE,
							machine_get_folder_name (frame->display_machine));

	list_buffer[0] = frame->display_period.type;
	list_buffer[1] = frame->display_period.value;
	g_key_file_set_integer_list (keyfile, FTGKF_GROUP_FRAME, FTGKF_PERIOD, list_buffer, 2);


	// Save positioning
	g_key_file_set_integer (keyfile, FTGKF_GROUP_POSITION, FTGKF_POSITION, position);


	// Private data saving
	frame_save_private (frame, keyfile);
	
	g_free (list_buffer);
	return keyfile;
}



int frame_table_save_to_files (const char *savedirpath, frame_table_t *tab)
{
	synema_instance_t	*inst		= synema_instance ();
	const gchar			*tablabel	= notebook_get_tab_label (GTK_NOTEBOOK (gtk_builder_get_object (inst->builder, "notebook")), tab->table_box);
	gchar				*buffer		= NULL;
	gchar				*framepath	= NULL;
	gchar				*path		= NULL;
	GError				*err		= NULL;
	gint				nbindex		= gtk_notebook_page_num (GTK_NOTEBOOK (gtk_builder_get_object (inst->builder, "notebook")), tab->table_box);
	GFile				*file		= NULL;
	GFileOutputStream	*stream		= NULL;
	GKeyFile			*keyfile	= NULL;
	guint				position, index = 0;
	gsize				len			= 0;
	GList				*iter		= tab->frames_list;

	path = g_strdup_printf ("%s/%i__%s", savedirpath, nbindex, tablabel);

	if (g_mkdir_with_parents (path, 0777)) {
		g_warning ("frame_table_save_to_file: Could not make the saved frames directory (%s)", g_strerror (errno));
		g_free (path);
		return -1;
	}

	if (iter) {
		do {
			if (!frame_get_position (iter->data, &position)) {
				g_warning ("frame_table_save_to_file: A frame could not be saved (not in the table) (err0)");
			} else {
				keyfile = frame_to_key_file (iter->data, position);
				buffer = g_key_file_to_data (keyfile, &len, &err);
				if (err) {
					g_warning ("frame_table_save_to_file: A frame could not be saved (%s) (err1)", err->message);
					g_clear_error (&err);
				} else {
					framepath = g_strdup_printf ("%s/%03d"FRAME_SAVE_SUFFIX, path, index);
					index++;
					file = g_file_new_for_path (framepath);
					g_free (framepath);

					stream = g_file_replace (file, NULL, TRUE, G_FILE_CREATE_NONE, NULL, &err);
					if (err) {
						g_warning ("frame_table_save_to_file: A frame could not be saved (%s) (err2)", err->message);
						g_clear_error (&err);
					} else {
						if (g_output_stream_write (G_OUTPUT_STREAM (stream), FRAME_HEADER, g_utf8_strlen (FRAME_HEADER, -1), NULL, &err) == -1) {
							g_warning ("frame_table_save_to_file: A frame could not be saved (%s) (err3)", err->message);
							g_clear_error (&err);
						}
						if (g_output_stream_write (G_OUTPUT_STREAM (stream), buffer, len, NULL, &err) == -1) {
							g_warning ("frame_table_save_to_file: A frame could not be saved (%s) (err4)", err->message);
							g_clear_error (&err);
						}
						g_object_unref (stream);
					}

					g_object_unref (file);
				}
				
				g_key_file_free (keyfile);
				g_free (buffer);
			}
		} while ((iter = g_list_next (iter)) != NULL);
	}

	g_free (path);
	return 0;
}



frame_t *file_to_frame (const char *path)
{
	synema_instance_t	*inst			= synema_instance ();
	frame_t				*frame			= NULL;
	gchar				*plugin_type	= NULL;
	gchar				*buffer			= NULL;
	GError				*err			= NULL;
	gint				*list_buffer	= NULL;
	GKeyFile			*keyfile		= g_key_file_new ();
	gsize				list_len		= 0;
	machine_t			*machine		= NULL;
	size_ratio_t		size;
	time_period_t		period			= {0,0};


	// Loading the file
	g_key_file_load_from_file (keyfile, path, G_KEY_FILE_NONE, &err);
	if (err) {
		g_warning ("file_to_frame: Opening file %s failed (%s)", path, err->message);
		g_clear_error (&err);
		return NULL;
	}


	// Plugin type (get the value and check its validity)
	plugin_type = g_key_file_get_string (keyfile, FTGKF_GROUP_FRAME, FTGKF_TYPE, &err);
	if (err) {
		g_warning ("file_to_frame: Could not read plugin type (%s)", err->message);
		g_clear_error (&err);
		g_key_file_free (keyfile);
		return NULL;
	}
	if ((frame = frame_new (plugin_type)) == NULL) {
		g_warning ("file_to_frame: Unsupported plugin type %s", plugin_type);
		g_key_file_free (keyfile);
		g_free (plugin_type);
		return NULL;
	}
	g_free (plugin_type);

	
	// Size ratio
	buffer = g_key_file_get_string (keyfile, FTGKF_GROUP_FRAME, FTGKF_SIZE, &err);
	if (err) {
		g_warning ("file_to_frame: Could not read size ratio (%s)", err->message);
		g_clear_error (&err);
		g_key_file_free (keyfile);
		frame_free_uninitialised (frame);
		return NULL;
	}

	if (g_strcmp0 (buffer, "LARGE") == 0)
		size = LARGE;
	else if (g_strcmp0 (buffer, "NORMAL") == 0)
		size = NORMAL;
	else if (g_strcmp0 (buffer, "FULL") == 0)
		size = FULL;
	else {
		g_warning ("file_to_frame: Unsupported size ratio %s", buffer);
		g_key_file_free (keyfile);
		frame_free_uninitialised (frame);
		g_free (buffer);
		return NULL;
	}
	g_free (buffer);

	
	// Machine name
	buffer = g_key_file_get_string (keyfile, FTGKF_GROUP_FRAME, FTGKF_MACHINE, &err);
	if (err) {
		g_warning ("file_to_frame: Could not read machine name (%s)", err->message);
		g_clear_error (&err);
		g_key_file_free (keyfile);
		frame_free_uninitialised (frame);
		return NULL;
	}
	if ((machine = machine_list_find_by_name (inst->machines_list, buffer)) == NULL) {
		g_warning ("file_to_frame: Could not find machine %s", buffer); 
		g_key_file_free (keyfile);
		frame_free_uninitialised (frame);
		g_free (buffer);
		return NULL;
	}
	g_free (buffer);


	// Plugin type
	list_buffer = g_key_file_get_integer_list (keyfile, FTGKF_GROUP_FRAME, FTGKF_PERIOD, &list_len, &err);
	if (err) {
		g_warning ("file_to_frame: Could not read time period (%s)", err->message);
		g_clear_error (&err);
		g_key_file_free (keyfile);
		frame_free_uninitialised (frame);
		return NULL;
	}
	if (list_len == 2) {
		period.type = list_buffer[0];
		period.value = list_buffer[1];
	} else {
		g_warning ("file_to_frame: Invalid time period setting");
		g_key_file_free (keyfile);
		frame_free_uninitialised (frame);
		g_free (list_buffer);
		return NULL;
	}
	g_free (list_buffer);


	// Position in the table
	frame->restore_position = g_key_file_get_integer (keyfile, FTGKF_GROUP_POSITION, FTGKF_POSITION, &err);
	if (err) {
		g_warning ("file_to_frame: Could not read frame position (%s)", err->message);
		g_clear_error (&err);
		g_key_file_free (keyfile);
		frame_free_uninitialised (frame);
		return NULL;
	}


	// Create the frame
	if (frame_initialise (frame, size, machine, &period) != 0) {
		frame_free (frame);
		g_key_file_free (keyfile);
		g_warning ("file_to_frame: Frame could not be initialised");
		return NULL;
	}


	// Private data
	frame_restore_private (frame, keyfile);

	g_key_file_free (keyfile);
	return frame;
}



int frame_table_load_from_files (const char *subdirpath, frame_table_t *tab)
{
	frame_t			*frame		= NULL;
	gchar			*framepath	= NULL;
	GError			*err		= NULL;
	GFile			*dirfile	= NULL;
	GFileEnumerator	*direnum	= NULL;
	GFileInfo		*dirinfo	= NULL;
	GFileInfo		*current	= NULL;
	GList			*iter		= NULL;
	GList			*list		= NULL;
	
	dirfile = g_file_new_for_path (subdirpath);
	
	tab->position = g_ascii_strtod (g_strrstr (subdirpath, "/") + 1, NULL);

	dirinfo = g_file_query_info (dirfile, "standard::type", G_FILE_QUERY_INFO_NONE, NULL, &err);
	if (err) {
		//g_warning ("frame_table_load_from_files: Error while querying the "SAVED_FRAMES_DIR" file (%s)", err->message);
		// This may happen if the file doesn't exist, in which case we just don't restore any frame
		g_clear_error (&err);
		g_object_unref (dirfile);
		return -1;
	}
	if (g_file_info_get_file_type (dirinfo) != G_FILE_TYPE_DIRECTORY) {
		// This is not an error, we just can't restore frames since there is no saved frames directory
		return -1;
	}
	g_object_unref (dirinfo);

	direnum = g_file_enumerate_children (dirfile, "standard::name", G_FILE_QUERY_INFO_NONE, NULL, &err);
	if (err) {
		g_warning ("frame_table_load_from_files: Error while reading the saved frames directory (%s)", err->message);
		g_clear_error (&err);
		g_object_unref (dirfile);
		return -1;
	}


	while ((current = g_file_enumerator_next_file (direnum, NULL, &err)) != NULL) {
		if (err) {
			g_warning ("frame_table_load_from_files: Error while reading a file of the saved frames (%s)", err->message);
			g_clear_error (&err);
		} else {
			if (g_str_has_suffix (g_file_info_get_name (current), FRAME_SAVE_SUFFIX)) {
				framepath = g_strdup_printf ("%s/%s", subdirpath, g_file_info_get_name (current));
				frame = file_to_frame (framepath);
				g_free (framepath);
				if (frame) {
					list = g_list_prepend (list, frame);
				}
			}
			g_object_unref (current);
		}
	}
	g_object_unref (direnum);
	g_object_unref (dirfile);

	//TODO sort the list according to pos

	if (list) {
		iter = list;
		do {
			frame = iter->data;
			frame_table_add (tab, frame);
		} while ((iter = g_list_next (iter)) != NULL);
	}

	return 0;
}



void save_frames_dialog_show (GtkToolButton *button, gpointer user_data)
{
	synema_instance_t	*inst			= synema_instance ();
	gchar				*path			= NULL;
	gchar				*tarpath		= NULL;
	gchar				*taruri			= NULL;
	gchar				*dlg_objects[]	= {"save_frames_dialog", NULL};
	GError				*err			= NULL;
	GFile				*tmp			= NULL;
	GList				*iter			= NULL;
	GtkRecentManager	*manager		= NULL;
	GtkWidget			*dialog			= NULL;
	TAR					*tarhandle		= NULL;


	// Add a new dialog to the builder
	path = g_strdup_printf ("%s/window.ui", inst->data_dir);
	gtk_builder_add_objects_from_file (inst->builder, path, dlg_objects, &err);
	g_free (path);
	if (err) {
		g_warning ("frame_on_new: %s", err->message);
		g_clear_error (&err);
		return;
	}


	// Now running the dialog
	dialog = (GtkWidget *) gtk_builder_get_object (inst->builder, "save_frames_dialog");
	gint result = gtk_dialog_run (GTK_DIALOG (dialog));


	// Exploiting the result here
	switch (result) {
		case GTK_RESPONSE_OK:
			gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
			path = g_strdup_printf ("%s/"FRAME_SAVING_DIR, inst->tmp_dir);
			if (g_mkdir_with_parents (path, 0777)) {
				g_warning ("save_frames_dialog_show: Could not create a temporary directory in which frames could be written (%s)", g_strerror (errno));
				error_dialog ("<b>Failed to save file</b>\n\nCould not create a temporary  directory in which frames could be written (%s)", g_strerror (errno));
				recursive_remove (path);
				g_free (path);
				break;
			}
			
			// If all tabs should be saved
			if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (inst->builder, "save_frames_all_tabs")))) {
				iter = inst->tables_list;
				if (iter) {
					do {
						frame_table_save_to_files (path, iter->data);
					} while ((iter = g_list_next (iter)) != NULL);
				}
			}
			// If only the current tab should be saved
			else {
				frame_table_save_to_files (path, inst->current_table);
			}

			tarpath = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
			if (!g_str_has_suffix (tarpath, TAB_SAVE_SUFFIX)) {
				char *tmp = g_strdup_printf ("%s"TAB_SAVE_SUFFIX, tarpath);
				g_free (tarpath);
				tarpath = tmp;
			}
			
			if (tar_open (&tarhandle, tarpath, NULL, O_CREAT | O_TRUNC | O_WRONLY, 0666, 0)) {
				g_warning ("save_frames_dialog_show: Could not create a tar archive to save the frames (%s)", g_strerror (errno));
				error_dialog ("<b>Failed to save file</b>\n\nCould not create a tar archive to save the frames (%s)", g_strerror (errno));
				g_free (tarpath);
				recursive_remove (path);
				g_free (path);
				break;
			}
			
			if (tar_append_tree (tarhandle, path, SAVED_FRAMES_DIR)) {
				g_warning ("save_frames_dialog_show: Could not put frames into the tar archive (%s)", g_strerror (errno));
				error_dialog ("<b>Failed to save file</b>\n\nCould not put frames into the tar archive (%s)", g_strerror (errno));
				tar_close (tarhandle);
				g_free (tarpath);
				g_free (path);
				break;
			}
			recursive_remove (path);
			g_free (path);

			if (tar_append_eof (tarhandle)) {
				g_warning ("save_frames_dialog_show: Could not set the EOF of the tar archive (%s)", g_strerror (errno));
				error_dialog ("<b>Failed to save file</b>\n\nCould not set the EOF of the tar archive (%s)\nThis is most likely a libtar bug", g_strerror (errno));
				tar_close (tarhandle);
				g_free (tarpath);
				break;
			}

			if (tar_close (tarhandle)) {
				g_warning ("save_frames_dialog_show: Error on closing the tar archive (%s)", g_strerror (errno));
				error_dialog ("<b>Failed to save file</b>\n\nError on closing the tar archive (%s)\nThis is most likely a libtar bug", g_strerror (errno));
				g_free (tarpath);
				break;
			}
			manager = gtk_recent_manager_get_default ();
			tmp = g_file_new_for_path (tarpath);
			taruri = g_file_get_uri (tmp);
			gtk_recent_manager_add_item (manager, taruri);
			g_free (tarpath);
			g_free (taruri);
			g_object_unref (tmp);
		default:
			break;
	}

	gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);
	gtk_widget_destroy (dialog);
}
