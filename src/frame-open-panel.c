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
 * \file frame-open-panel.c
 * \brief Source code for functions related to the frame opening panel.
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This source file contains the definition of functions related to the frame
 * opening panel.
 */
#include <errno.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <libtar.h>

#include "application-data.h"
#include "frame-open-panel.h"
#include "frame-saving.h"
#include "notebook.h"



gboolean frame_open_try_load_frame (GFile *file)
{
	synema_instance_t	*inst		= synema_instance ();
	frame_t				*frame		= NULL;
	gchar				*buffer		= NULL;
	GError				*err		= NULL;
	GFileInputStream	*stream		= NULL;
	guint				len			= g_utf8_strlen (FRAME_HEADER, -1) + 1;


	// Getting a read stream for the file
	stream = g_file_read (file, NULL, &err);
	if (err) {
		g_debug ("frame_open_try_load_frame: Could not open the file for reading (%s)", err->message);
		g_clear_error (&err);
		return FALSE;
	}


	// Getting the first bits of the frame file to compare against a frame header
	buffer = g_malloc (len);
	g_input_stream_read (G_INPUT_STREAM (stream), buffer, len, NULL, &err);
	if (err) {
		g_debug ("frame_open_try_load_frame: Could not read the file (%s)", err->message);
		g_clear_error (&err);
		return FALSE;
	}


	// Checking the buffer begins with the frame header
	if (!g_str_has_prefix (buffer, FRAME_HEADER)) {
		g_free (buffer);
		g_debug ("frame_open_try_load_frame: File is not a frame");
		return FALSE;
	}
	g_free (buffer);


	// Reading the file for real
	buffer = g_file_get_path (file);
	frame = file_to_frame (buffer);
	g_free (buffer);
	if (frame == NULL) {
		g_warning ("frame_open_try_load_frame: File is a frame but is corrupted");
		return FALSE;
	} else {
		if (frame_table_add (inst->current_table, frame) != 0) {
			g_warning ("frame_open_try_load_frame: Frame could not be added to the current table");
			return FALSE;
		} else {
			g_debug ("frame_open_try_load_frame: Frame loaded");
			return TRUE;
		}
	}
}



gboolean frame_open_try_load_tab (gchar *filename)
{
	synema_instance_t	*inst		= synema_instance ();
	frame_table_t		*tmptab		= NULL;
	gchar				*destpath	= NULL;
	gchar				*framesdir	= NULL;
	gchar				*ptr		= NULL;
	gchar				*subdirpath	= NULL;
	gchar				*tmpname	= NULL;
	GError				*err		= NULL;
	GFile				*file		= NULL;
	GFileEnumerator		*fileenum	= NULL;
	GFileInfo			*current	= NULL;
	GList				*iter		= NULL;
	GtkWidget 			*frame_box	= NULL;
	guint				tabcount;
	TAR					*tarhandle	= NULL;



	// Getting a TAR handle for the file
	if (tar_open (&tarhandle, filename, NULL, O_RDONLY, 0, 0)) {
		g_debug ("frame_open_try_load_tab: Could not get a TAR handle for the file (%s)", g_strerror (errno));
		return FALSE;
	}


	// Extracting the TAR file's data to a temporary folder
	destpath = g_strdup_printf ("%s/"FRAME_OPENING_DIR, inst->tmp_dir);
	if (tar_extract_all (tarhandle, destpath) != 0) {
		g_debug ("frame_open_try_load_tab: Could not extract the contents of the file (%s)", g_strerror (errno));
		g_free (destpath);
		tar_close (tarhandle);
		return FALSE;
	}
	tar_close (tarhandle);
	framesdir = g_strdup_printf ("%s/"SAVED_FRAMES_DIR, destpath);
	file = g_file_new_for_path (framesdir);
	current = g_file_query_info (file, "standard::type", G_FILE_QUERY_INFO_NONE, NULL, &err);
	if (err) {
		g_debug ("frame_open_try_load_tab: Archive does not contain a "SAVED_FRAMES_DIR" directory (%s)", err->message);
		g_clear_error (&err);
		return FALSE;
	}

	if (!g_file_info_get_file_type (current) == G_FILE_TYPE_DIRECTORY) {
		g_debug ("frame_open_try_load_tab: File "SAVED_FRAMES_DIR" in the archive is not a directory (%s)", err->message);
		g_clear_error (&err);
		return FALSE;
	}


	// Reading the data in the destination folder
	fileenum = g_file_enumerate_children (file, "standard::name", G_FILE_QUERY_INFO_NONE, NULL, &err);
	if (err) {
		g_warning ("frame_open_try_load_tab: Error while reading the temp directory containing the tabs (%s)", err->message);
		g_clear_error (&err);
		return FALSE;
	}
	
	tabcount = 0;
	frame_box = (GtkWidget *) gtk_builder_get_object (inst->builder, "notebook");
	while ((current = g_file_enumerator_next_file (fileenum, NULL, &err)) != NULL) {
		if (err) {
			g_warning ("frame_open_try_load_tab: Apparently corrupted file in Tar archive (%s)", err->message);
			g_clear_error (&err);
		} else {
			switch (tabcount) {
				case 0: // Save the current tab and wait before it's added
					tmpname = g_strdup (g_file_info_get_name (current));
					break;

				case 1: // There are several tabs, empty current notebook
					notebook_remove_all ();
					// There is no break as we still want to add the current tab to the notebook
					
				default: // Add the current file to the notebook
					subdirpath = g_strdup_printf ("%s/%s", framesdir, g_file_info_get_name (current));
					ptr = g_strstr_len (g_strrstr (subdirpath, "/"), -1, "__");
					if (!ptr) {
						g_warning ("frame_open_try_load_tab: Malformed directory name %s", subdirpath);
						notebook_add_page (NULL, (GtkWidget *) gtk_builder_get_object (inst->builder, "notebook"));
						return FALSE;
					} else {
						tmptab = _notebook_add_page (frame_box, ptr + 2);
						if (frame_table_load_from_files (subdirpath, tmptab) != 0) {
							notebook_remove_page (NULL, tmptab);
						}
					}
					g_free (subdirpath);
					break;
			}

			tabcount++;
			g_object_unref (current);
		}
	}
	g_object_unref (fileenum);


	// If tmpname is set then we can now add the tab to the notebook
	if (tmpname) {
		subdirpath = g_strdup_printf ("%s/%s", framesdir, tmpname);
		g_free (tmpname);
		ptr = g_strstr_len (g_strrstr (subdirpath, "/"), -1, "__");
		if (!ptr) {
			g_warning ("frame_open_try_load_tab: Malformed directory name %s", subdirpath);
		} else {
			tmptab = _notebook_add_page (frame_box, ptr + 2);
			if (frame_table_load_from_files (subdirpath, tmptab) != 0) {
				notebook_remove_page (NULL, tmptab);
			}
		}
		g_free (subdirpath);
	}


	// As in post_init_restore_frames, we need to sort the tabs as they were when saved
	inst->tables_list = g_list_sort (inst->tables_list, frame_table_list_cmp_by_position);
	if (inst->tables_list) {
		iter = inst->tables_list;
		do {
			tmptab = iter->data;
			gtk_notebook_reorder_child (GTK_NOTEBOOK (frame_box), tmptab->table_box, tmptab->position);
		} while ((iter = g_list_next (iter)) != NULL);
	}


	// Removing the temporary files created	
	recursive_remove (destpath);
	g_free (destpath);

	return TRUE;
}



void frame_open_file (GFile *file)
{
	synema_instance_t	*inst		= synema_instance ();
	gboolean			opened		= FALSE;
	gboolean			tested[LAST_FILETYPE_T];
	gchar				*filename	= g_file_get_path (file);
	gint				i;


	// Make the GUI insensitive while the function is in use
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "open_filebutton_file"), FALSE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "open_recentchooser"), FALSE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "open_label_file"), FALSE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "open_label_recent"), FALSE);


	for (i=0; i<LAST_FILETYPE_T; i++) {
		tested[i] = FALSE;
	}


	// If file is likely to be a frame	
	if (g_str_has_suffix (filename, FRAME_SAVE_SUFFIX)) {
		if (frame_open_try_load_frame (file)) {
			g_debug ("frame_open_file: File of type "FRAME_SAVE_SUFFIX" successfully opened");
			opened = TRUE;
		}
		tested[FILE_FRAME] = TRUE;
	}
	// If file is likely to be a frame tab (or several tabs) archive
	else if (g_str_has_suffix (filename, TAB_SAVE_SUFFIX)) {
		if (frame_open_try_load_tab (filename)) {
			g_debug ("frame_open_file: File of type "TAB_SAVE_SUFFIX" successfully opened");
			opened = TRUE;
		}
		tested[FILE_TAB] = TRUE;
	}


	// If the file type assertions were wrong, try every filetype.
	if (!opened && !tested[FILE_FRAME]) {
		if (frame_open_try_load_frame (file)) {
			g_debug ("frame_open_file: File of type "FRAME_SAVE_SUFFIX" successfully opened");
			opened = TRUE;
		}
		tested[FILE_FRAME] = TRUE;
	}
	
	if (!opened && !tested[FILE_TAB]) {
		if (frame_open_try_load_tab (filename)) {
			g_debug ("frame_open_file: File of type "TAB_SAVE_SUFFIX" successfully opened");
			opened = TRUE;
		}
		tested[FILE_TAB] = TRUE;
	}

	if (!opened) {
		g_warning ("frame_open_file: Failed to open file %s", filename);
		error_dialog ("Failed to open file ‘%s’", filename);
	}
	g_free (filename);


	// Make the GUI insensitive while the function is in use
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "open_filebutton_file"), TRUE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "open_recentchooser"), TRUE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "open_label_file"), TRUE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "open_label_recent"), TRUE);


	return;
}



static void frame_open_from_filebutton (GtkFileChooserButton *button, gpointer user_data)
{
	// Beware, don't sort these declarations alphabetically
	gchar				*filename	= gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (button));
	GFile				*file		= g_file_new_for_path (filename);
	gchar				*fileuri	= g_file_get_uri (file);
	GtkRecentManager	*manager	= gtk_recent_manager_get_default ();

	// Adding the file to the recent files manager
	gtk_recent_manager_add_item (manager, fileuri);
	g_free (filename);
	g_free (fileuri);

	// Opening the file
	frame_open_file (file);
	g_object_unref (file);
}



static void frame_open_from_recent (GtkRecentChooser *chooser, gpointer user_data)
{
	gchar			*uri		= NULL;
	GFile			*file		= NULL;
	
	uri = gtk_recent_chooser_get_current_uri (chooser);
	file = g_file_new_for_uri (uri);
	g_free (uri);

	frame_open_file (file);
	g_object_unref (file);
}



void frame_open_setup_widgets ()
{
	synema_instance_t		*inst			= synema_instance ();
	GtkFileFilter			*filter			= NULL;
	GtkRecentFilter			*recfilter		= NULL;
	GtkWidget				*filebutton		= (GtkWidget *) gtk_builder_get_object (inst->builder, "open_filebutton_file");
	GtkRecentChooser		*chooser		= GTK_RECENT_CHOOSER (gtk_builder_get_object (inst->builder, "open_recentchooser"));


	// Set the file filter for the Open from File dialog
	filter = gtk_file_filter_new ();
	gtk_file_filter_add_mime_type (filter, "application/x-tar");
	gtk_file_filter_add_pattern (filter, "*"FRAME_SAVE_SUFFIX);
	gtk_file_filter_set_name (filter, APP_NAME" files (*"FRAME_SAVE_SUFFIX", *."TAB_SAVE_SUFFIX")");
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (filebutton), filter);
	gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (filebutton), filter);

	filter = gtk_file_filter_new ();
	gtk_file_filter_add_pattern (filter, "*");
	gtk_file_filter_set_name (filter, "All files");
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (filebutton), filter);

	filter = gtk_file_filter_new ();
	gtk_file_filter_add_mime_type (filter, "application/x-tar");
	gtk_file_filter_set_name (filter, "Frame tab archives (*"TAB_SAVE_SUFFIX")");
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (filebutton), filter);

	filter = gtk_file_filter_new ();
	gtk_file_filter_add_pattern (filter, "*"FRAME_SAVE_SUFFIX);
	gtk_file_filter_set_name (filter, "Standalone frames (*"FRAME_SAVE_SUFFIX")");
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (filebutton), filter);
	g_signal_connect (GTK_FILE_CHOOSER (filebutton), "file-set", G_CALLBACK (frame_open_from_filebutton), NULL);


	// Set up the recently saved frame tabs tree view
	recfilter = gtk_recent_filter_new ();
	gtk_recent_filter_set_name (recfilter, APP_NAME" files");
	gtk_recent_filter_add_application (recfilter, APP_NAME);
	gtk_recent_chooser_set_filter (chooser, recfilter);
	g_signal_connect (GTK_RECENT_CHOOSER (chooser), "item-activated", G_CALLBACK (frame_open_from_recent), NULL);
}
