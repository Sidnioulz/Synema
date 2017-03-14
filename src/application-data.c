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
 * \file application-data.c
 * \brief Source code for the application's functions
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains functions manipulating Synema instances.
 */
#include <errno.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "application-data.h"
#include "frame-saving.h"
#include "log-player-dbus.h"
#include "machine.h"
#include "notebook.h"
#include "plugins.h"



static void _error_dialog (GtkWindow *parent, const gchar *buffer)
{
	GtkWidget *dialog = gtk_message_dialog_new (parent,
												GTK_DIALOG_DESTROY_WITH_PARENT,
												GTK_MESSAGE_ERROR,
												GTK_BUTTONS_OK,
												NULL);

	gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog), buffer);
	gtk_dialog_run (GTK_DIALOG (dialog));

	gtk_widget_destroy (dialog);

}


void error_dialog (const gchar *message, ...)
{
	synema_instance_t	*inst	= synema_instance ();
	gchar				*buffer	= NULL;
	GtkWindow			*parent = NULL;
	va_list				args;
	
	parent = GTK_WINDOW (gtk_builder_get_object (inst->builder, "synema_window"));

	va_start (args, message);
	buffer = g_strdup_vprintf (message, args);
	va_end (args);

	_error_dialog (parent, buffer);
	g_free (buffer);
}



void about_dialog (GtkButton *button, gpointer user_data)
{
	synema_instance_t	*inst			= synema_instance ();
	gchar				*dlg_objects[]	= {"aboutdialog", NULL};
	gchar				*path			= NULL;
	GError				*err			= NULL;
	GtkWidget			*dialog			= NULL;


	// Add a new about dialog to the builder
	path = g_strdup_printf ("%s/window.ui", inst->data_dir);
	gtk_builder_add_objects_from_file (inst->builder, path, dlg_objects, &err);
	g_free (path);
	if (err) {
		g_warning ("settings_dialog_show: %s", err->message);
		g_clear_error (&err);
		return;
	}

	// Getting the dialog to display
	dialog = (GtkWidget *) gtk_builder_get_object (inst->builder, "aboutdialog");

	// We can destroy the dialog after it's been run
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}



void toggle_panels (GtkToolButton *button, gpointer user_data)
{
	synema_instance_t	*inst		= synema_instance ();
	gint				paneltype	= GPOINTER_TO_INT (user_data);
	GtkWidget	 		*panel		= (GtkWidget *) gtk_builder_get_object (inst->builder, "top_panel");
	GtkWidget 			*top_box	= (GtkWidget *) gtk_builder_get_object (inst->builder, "top_box");
	GtkWidget 			*frame_box	= (GtkWidget *) gtk_builder_get_object (inst->builder, "notebook");
	GtkWidget			*pn_open	= (GtkWidget *) gtk_builder_get_object (inst->builder, "open_vbox");
	GtkWidget			*pn_actions	= (GtkWidget *) gtk_builder_get_object (inst->builder, "actions_vbox");
	GtkWidget			*pn_player	= (GtkWidget *) gtk_builder_get_object (inst->builder, "player_vbox");


	// We just show the asked panel here
	if (paneltype == PANEL_PLAYER) {
		gtk_widget_show (pn_player);
		gtk_widget_hide (pn_open);
		gtk_widget_hide (pn_actions);
	} else if (paneltype == PANEL_OPEN) {
		gtk_widget_show (pn_open);
		gtk_widget_hide (pn_player);
		gtk_widget_hide (pn_actions);
	} else if (paneltype == PANEL_ACTIONS) {
		gtk_widget_show (pn_actions);
		gtk_widget_hide (pn_player);
		gtk_widget_hide (pn_open);
	}


	// If the panel was hidden we move the necessary widgets to make it visible
	if (gtk_widget_get_parent (frame_box) != panel) {
		g_object_ref (frame_box);
		gtk_container_remove (GTK_CONTAINER (top_box), frame_box);
		gtk_container_add (GTK_CONTAINER (top_box), panel);
		gtk_paned_add2 (GTK_PANED (panel), frame_box);
		g_object_unref (frame_box);
		g_object_unref (panel);
	}
	// If we toggle the current panel the we must hide it
	else if (paneltype == inst->current_panel) {
		g_object_ref (panel);
		g_object_ref (frame_box);
		gtk_container_remove (GTK_CONTAINER (panel), frame_box);
		gtk_container_remove (GTK_CONTAINER (top_box), panel);
		gtk_container_add (GTK_CONTAINER (top_box), frame_box);
		g_object_unref (frame_box);
	}

	// We set the current panel to the last toggled one
	inst->current_panel = paneltype;
}



gint post_init_restore_frames ()
{
	synema_instance_t	*inst			= synema_instance ();
	frame_table_t		*tmptab			= NULL;
	gchar				*dirpath		= NULL;
	gchar				*subdirpath		= NULL;
	gchar				*ptr			= NULL;
	GError				*err			= NULL;
	GFile				*dirfile		= NULL;
	GFileEnumerator		*direnum		= NULL;
	GFileInfo			*current		= NULL;
	GFileInfo			*dirinfo		= NULL;
	GList				*iter			= NULL;
	GtkWidget			*frame_box		= NULL;

	frame_box = (GtkWidget *) gtk_builder_get_object (inst->builder, "notebook");

	dirpath = g_strdup_printf ("%s/"SAVED_FRAMES_DIR, inst->config_dir);
	dirfile = g_file_new_for_path (dirpath);

	dirinfo = g_file_query_info (dirfile, "standard::type", G_FILE_QUERY_INFO_NONE, NULL, &err);
	if (err) {
		//g_warning ("frame_table_load_from_files: Error while querying the "SAVED_FRAMES_DIR" file (%s)", err->message);
		// This may happen if the file doesn't exist, in which case we just don't restore any frame
		g_clear_error (&err);
		g_object_unref (dirfile);
		g_free (dirpath);
		return -1;
	}
	if (g_file_info_get_file_type (dirinfo) != G_FILE_TYPE_DIRECTORY) {
		// This is not an error, we just can't restore frames since there is no saved frames directory
		g_free (dirpath);
		return -1;
	}
	g_object_unref (dirinfo);

	direnum = g_file_enumerate_children (dirfile, "standard::type,standard::name", G_FILE_QUERY_INFO_NONE, NULL, &err);
	if (err) {
		g_warning ("post_init_restore_frames: Error while reading the "SAVED_FRAMES_DIR" directory (%s)", err->message);
		g_free (dirpath);
		g_clear_error (&err);
		g_object_unref (dirfile);
		return -1;
	}


	// Let's fetch the tabs
	while ((current = g_file_enumerator_next_file (direnum, NULL, &err)) != NULL) {
		if (err) {
			g_warning ("post_init_restore_frames: Error while reading a subdirectory of "SAVED_FRAMES_DIR" (%s)", err->message);
			g_clear_error (&err);
		} else {
			if (g_file_info_get_file_type (current) == G_FILE_TYPE_DIRECTORY) {
				subdirpath = g_strdup_printf ("%s/%s", dirpath, g_file_info_get_name (current));
				ptr = g_strstr_len (g_strrstr (subdirpath, "/"), -1, "__");
				if (!ptr) {
					g_warning ("post_init_restore_frames: Malformed directory name %s", subdirpath);
				} else {
					tmptab = _notebook_add_page (frame_box, ptr + 2);
					if (frame_table_load_from_files (subdirpath, tmptab) != 0) {
						notebook_remove_page (NULL, tmptab);
					}
				}
				g_free (subdirpath);
			}
			g_object_unref (current);
		}
	}

	g_free (dirpath);
	g_object_unref (direnum);
	g_object_unref (dirfile);


	// Now we can sort the tabs and put them in the right order.
	inst->tables_list = g_list_sort (inst->tables_list, frame_table_list_cmp_by_position);
	if (inst->tables_list) {
		iter = inst->tables_list;
		do {
			tmptab = iter->data;
			gtk_notebook_reorder_child (GTK_NOTEBOOK (frame_box), tmptab->table_box, tmptab->position);
		} while ((iter = g_list_next (iter)) != NULL);
	}


	// We set the tab counter to the current number of pages to avoid having several "Unnamed X" tabs in most cases
	inst->labelcount = gtk_notebook_get_n_pages (GTK_NOTEBOOK (frame_box));

	gtk_notebook_set_current_page (GTK_NOTEBOOK (frame_box), inst->settings->last_current_tab);

	return 0;
}



void pre_quit_save_frames (GtkObject *object, gpointer user_data)
{
	synema_instance_t	*inst		= synema_instance ();
	frame_table_t		*tab		= NULL;
	gchar				*path		= g_strdup_printf ("%s/"SAVED_FRAMES_DIR, inst->config_dir);
	GList				*iter		= inst->tables_list;

	inst->settings->last_current_tab = inst->current_table->position;

	if (recursive_remove (path))
		g_warning ("pre_quit_save_frames: Could not fully remove previously saved frames.");

	if ((iter) && (inst->settings->autosave_frames)) {
		do {
			tab = iter->data;
			frame_table_save_to_files (path, tab);
		} while ((iter = g_list_next (iter)) != NULL);
	}

	g_free (path);
	gtk_main_quit ();
}



gint recursive_remove (const gchar *path)
{
	gchar				*subpath	= NULL;
	GError				*err		= NULL;
	GFile				*file		= NULL;
	GFileEnumerator		*fileenum	= NULL;
	GFileInfo			*current	= NULL;
	GFileInfo			*fileinfo	= NULL;

	file = g_file_new_for_path (path);

	if (!g_file_query_exists (file, NULL))
		return 0;

	fileinfo = g_file_query_info (file, "standard::type", G_FILE_QUERY_INFO_NONE, NULL, &err);
	if (err) {
		g_warning ("recursive_remove: Could not get file info for %s (%s)", path, err->message);
		g_clear_error (&err);
		g_object_unref (file);
		return -1;
	} else {
		if (g_file_info_get_file_type (fileinfo) == G_FILE_TYPE_DIRECTORY) {
			fileenum = g_file_enumerate_children (file, "standard::name, standard::type", G_FILE_QUERY_INFO_NONE, NULL, &err);
			if (err) {
				g_warning ("recursive_remove: Could not enumerate files in %s (%s)", path, err->message);
				g_clear_error (&err);
			} else {
				while ((current = g_file_enumerator_next_file (fileenum, NULL, &err)) != NULL) {
					if (err) {
						g_warning ("recursive_remove: Could not get next file to remove in %s (%s)", path, err->message);
						g_clear_error (&err);
					} else {
						subpath = g_strdup_printf ("%s/%s", path, g_file_info_get_name (current));
						if (recursive_remove (subpath))
							g_warning ("recursive_remove: Could not remove file %s", subpath);
						g_free (subpath);
						g_object_unref (current);
					}
				}
			}
			g_object_unref (fileenum);
		}
		g_object_unref (fileinfo);
		if (g_remove (path)) {
			g_warning ("recursive_remove: Could not remove file %s (%s)", path, g_strerror (errno));
			g_object_unref (file);
			return -1;
		}
	}
	
	g_object_unref (file);
	return 0;
}



guint unique_frame_id_generator (GList *list)
{
	synema_instance_t	*inst	= synema_instance ();
	frame_t				*f		= NULL;
	gboolean			found	= FALSE;
	GList				*iter	= list;
	guint				val;

	while (!found) {
		val = g_rand_int (inst->rand);
		found = TRUE;

		if (iter) {
			do {
				f = iter->data;
				found = (f->id != val);
			} while (((iter = g_list_next (iter)) != NULL) && (found));
		}
	}

	return val;
}



/** \fn static synema_instance_t *synema_new ()
 * \brief Creates a new Synema instance
 *
 * \warning This is a private function.
 *
 * This function creates a new Synema instance, which means load a
 * GtkBuilder for the main GUI, load a table that will welcome frames, create an
 * empty list of frames and load a list of available machines.
 * @return a valid Synema instance
 */
static synema_instance_t *synema_new ()
{
	const gchar* const	*data_dirs		= g_get_system_data_dirs ();
	gboolean			create_lock		= FALSE;
	gboolean			locked			= FALSE;
	gchar				*checkout		= NULL;
	gchar				*envbuff		= NULL;
	gchar				lockbuff[10]	= {0,0,0,0,0,0,0,0,0,0};
	gchar				*main_objects[]	= {"synema_window", "find_player_image",
											"time_period_liststore",
											"speedscaleadjustment",
											"adjustment_hour", "adjustment_minute",
											"adjustment_play_len", NULL};
	gchar				*path			= NULL;
	GError				*err			= NULL;
	GFile				*lock			= NULL;
	GFileInputStream	*instream		= NULL;
	GFileOutputStream	*outstream		= NULL;
	gint				i				= 0;
	synema_instance_t	*inst			= g_malloc (sizeof (synema_instance_t));
	struct stat			buff;


	// Initialising the tmp directory (either user's cache dir or /tmp)
	envbuff = g_strdup (getenv (ENV_TMP));
	if (envbuff)
		inst->tmp_dir = envbuff;
	else
		inst->tmp_dir = g_strdup_printf ("%s/"APP_NAME_LOWER, g_get_user_cache_dir ());
	if (g_stat (inst->tmp_dir, &buff) == 0) {
		if (!S_ISDIR (buff.st_mode)) {
			g_free (inst->tmp_dir);
			inst->tmp_dir = NULL;
			g_warning ("synema_new: The tmp directory is not an actual folder.");			
		} else if (!(S_IWUSR & buff.st_mode)) {
			g_free (inst->tmp_dir);
			inst->tmp_dir = NULL;
			g_warning ("synema_new: Does not have the right to write in the user cache directory.");			
		}
	} else {
		g_free (inst->tmp_dir);
		inst->tmp_dir = NULL;
	}
	if (inst->tmp_dir == NULL) {
		if (g_get_user_config_dir () != NULL)
			inst->tmp_dir = g_strdup_printf ("%s/"APP_NAME_LOWER, g_get_user_cache_dir ());
		else
			inst->tmp_dir = g_strdup (APP_TMP_DIR);
		if (g_mkdir_with_parents (inst->tmp_dir, 0777) != 0) {
			g_critical ("synema_new: Attempted to use %s as a cache directory but it failed (%s)",
						inst->tmp_dir,
						g_strerror (errno));
			g_free (inst->tmp_dir);
			inst->tmp_dir = NULL; // Avoid a double free error on synema_instance_free
		} else {
			g_debug ("Created the %s directory for tmp.\n", inst->tmp_dir);
		}
	}


	// Initialising the config folder
	envbuff = g_strdup (getenv (ENV_CONFIG)); //dup'ing now because getenv is not reentrant
	if (envbuff)
		inst->config_dir = envbuff;
	else
		inst->config_dir = g_strdup_printf ("%s/"APP_NAME_LOWER, g_get_user_config_dir ());
	if (g_stat (inst->config_dir, &buff) == 0) {
		if (!S_ISDIR (buff.st_mode)) {
			g_free (inst->config_dir);
			inst->config_dir = NULL;
			g_warning ("synema_new: The config directory is not an actual folder.");			
		}
	} else {
		g_free (inst->config_dir);
		inst->config_dir = NULL;
		//g_warning ("synema_new: Could not stat the config directory (%s)", g_strerror (errno));
	}
	if (inst->config_dir == NULL) {
		if (g_get_user_config_dir () != NULL)
			inst->config_dir = g_strdup_printf ("%s/"APP_NAME_LOWER, g_get_user_config_dir ());
		else
			inst->config_dir = g_strdup (APP_TMP_DIR);
		if (g_mkdir_with_parents (inst->config_dir, 0777) != 0) {
			g_warning ("synema_new: Attempted to use %s as a config directory but it failed (%s)",
						inst->config_dir,
						g_strerror (errno));
			g_free (inst->config_dir);
			inst->config_dir = NULL; // Avoid a double free error on synema_instance_free
		} else {
			g_debug ("Created the %s directory for config.\n", inst->config_dir);
		}
	}


	// Initialising the machines directory (~/.config/.../machines/)
	envbuff = g_strdup (getenv (ENV_MACHINES));
	if (envbuff)
		inst->machines_dir = envbuff;
	else
		inst->machines_dir = g_strdup_printf ("%s/"APP_NAME_LOWER"/"MACHINES_DIR, g_get_user_config_dir ());
	if (g_stat (inst->machines_dir, &buff) == 0) {
		if (!S_ISDIR (buff.st_mode)) {
			g_free (inst->machines_dir);
			inst->machines_dir = NULL;
			g_warning ("synema_new: The machines directory is not an actual folder.");			
		}
	} else {
		g_free (inst->machines_dir);
		inst->machines_dir = NULL;
		//g_warning ("synema_new: Could not stat the machines directory (%s)", g_strerror (errno));
	}
	if (inst->machines_dir == NULL) {
		if (g_get_user_config_dir () != NULL)
			inst->machines_dir = g_strdup_printf ("%s/"APP_NAME_LOWER"/"MACHINES_DIR, g_get_user_config_dir ());
		else
			inst->machines_dir = g_strdup (APP_TMP_DIR"/"MACHINES_DIR);
		if (g_mkdir_with_parents (inst->machines_dir, 0777) != 0) {
			g_warning ("synema_new: Attempted to use %s as a machines directory but it failed (%s)",
						inst->machines_dir,
						g_strerror (errno));
			g_free (inst->machines_dir);
			inst->machines_dir = NULL; // Avoid a double free error on synema_instance_free
		} else {
			g_debug ("Created the %s directory for machines.\n", inst->machines_dir);
		}
	}


	// Initialising the data directory (/usr/share/data/...)
	envbuff = g_strdup (getenv (ENV_DATA));
	if (envbuff)
		inst->data_dir = envbuff;
	else
		inst->data_dir = NULL;
	while (!inst->data_dir && data_dirs[i]) {
		checkout = g_strdup_printf ("%s/"APP_NAME_LOWER, data_dirs[i]);
		if (g_stat (checkout, &buff) == 0) {
			if (S_ISDIR (buff.st_mode))
				inst->data_dir = checkout;
			else
				g_free (checkout);
		} else {
			g_free (checkout);
		}
		i++;
	}
	if (inst->data_dir == NULL) {
		inst->data_dir = g_strdup (APP_TMP_DIR"/"DATA_DIR);
		if (g_mkdir_with_parents (inst->data_dir, 0777) != 0)
			g_error ("synema_new: No data directory, aborting (%s)",
						g_strerror (errno));
	}


	// Initialising the lib directory (/usr/share/lib/synema/...)
	envbuff = g_strdup (getenv (ENV_PLUGINS));
	if (envbuff)
		inst->plugins_dir = envbuff;
	else
		inst->plugins_dir = g_strdup (PATH_TO_PLUGINS_DIR);


	// Main GUI's GtkBuilder
	inst->builder = gtk_builder_new ();
	path = g_strdup_printf ("%s/window.ui", inst->data_dir);
	gtk_builder_add_objects_from_file (inst->builder, path, main_objects, &err);
	g_free (path);
	if (err) {
		g_error ("synema_new: %s", err->message);
		g_clear_error (&err);
		exit (EXIT_FAILURE);
	}


	// Creating a lock file to avoid having the application launched twice.
	path = g_strdup_printf ("%s/"APP_LOCK_FILE, inst->tmp_dir);
	lock = g_file_new_for_path (path);
	g_free (path);
	
	if (g_file_query_exists (lock, NULL)) {
		//READ the pid, and check if the Process is running, if so, display errmsg, else just replace the pid in the file by the app's one.
		instream = g_file_read (lock, NULL, &err);
		if (err) {
			g_warning ("synema_new: Could not open the lock file while it's meant to exist (%s)", err->message);
			g_clear_error (&err);
			locked = TRUE;
		}
		
		g_input_stream_read (G_INPUT_STREAM (instream), lockbuff, 10, NULL, &err);
		if (err) {
			g_warning ("synema_new: Could not read the lock file (%s)", err->message);
			g_clear_error (&err);
			locked = TRUE;
		}
		g_object_unref (instream);
		
		path = g_strdup_printf ("/proc/%i", (int)g_ascii_strtod (lockbuff, NULL));
		if (g_stat (path, &buff) == 0) {
			locked = TRUE;
		} else {
			g_warning ("synema_new: A lock file exists but the process that created it is not running anymore. This can happen when "APP_NAME" previously crashed.");
			create_lock = TRUE;
		}
		g_free (path);
		
		if (locked) {
			_error_dialog (NULL, "Can't launch "APP_NAME" because another instance is already running.");
			exit (EXIT_FAILURE);
		}
	} else {
		create_lock = TRUE;
	}
	
	if (create_lock) {
		outstream = g_file_replace (lock, NULL, FALSE, G_FILE_CREATE_NONE, NULL, &err);
		if (err) {
			g_warning ("synema_new: Could not create a lock file (%s)", err->message);
			g_clear_error (&err);
		} else {
			g_snprintf (lockbuff, 10, "%u", (unsigned int) getpid());
			if (g_output_stream_write (G_OUTPUT_STREAM (outstream), lockbuff, 10, NULL, &err) == -1) {
				g_warning ("synema_new: Could not write the pid in the lock file (%s)", err->message);
				g_clear_error (&err);
			}
			g_object_unref (outstream);
		}
	}
	g_object_unref (lock);


	// Pseudo random generator for IDs, and counter for tab labels
	inst->rand = g_rand_new ();
	inst->labelcount = 0;

	// Tables list - tables will be added after the instance is fully created
	inst->tables_list = NULL;
	inst->current_table = NULL;

	// Machines and plugins lists
	inst->plugins_list = plugin_list_new (inst->plugins_dir);
	inst->machines_list = machine_list_new (inst->machines_dir, inst->plugins_list);

	// Log player
	inst->player_status = PLAYER_STOPPED;
	inst->player_curr_time = 0;
	if (!player_dbus_connect (&(inst->dbus_connection), &(inst->dbus_proxy))) {
		g_warning ("synema_new: Could not connect to DBus.");
	}

	// Application settings
	inst->settings = settings_load (inst->config_dir, inst->data_dir);
	if (inst->settings == NULL) {
		g_error ("synema_new: Couldn't load the settings.");
		exit (EXIT_FAILURE);
	}
	
	inst->current_panel = PANEL_NONE;

	return inst;
}



/** \fn synema_instance_t *_synema_instance (int)
 * \brief Returns a pointer to the current Synema instance
 *
 * \warning You should not call this function unless you just freed the instance
 *
 * This function returns a pointer to the current instance of Synema (it
 * is implemented as a synema_instance_t static struct). If the instance
 * doesn't exist, it is created beforehand.
 * @param[in] setnull if not 0, then the instance will be set to NULL instead of returned
 * @return the current Synema instance, properly initialised
 */
static synema_instance_t *_synema_instance (int setnull)
{
	static synema_instance_t	*params		= NULL;
	static gboolean				building	= FALSE;

	if (building)
		g_error ("Someone attempted to access the Synema instance while it is building. This reflects a fatal bug in the application.");

	if (setnull)
		params = NULL;
	else if (params == NULL) {
		building = TRUE;
		params = synema_new ();
		building = FALSE;
	}

	return params;
}



synema_instance_t *synema_instance ()
{
	return _synema_instance (0);
}



void synema_instance_free ()
{
	synema_instance_t	*inst	= synema_instance ();

	settings_save (inst->settings);

	if (inst->tables_list) {
		GList *iter = inst->tables_list;
		do {
			frame_table_free (iter->data);
		} while ((iter = g_list_next (iter)) != NULL);
	}
	g_list_free (inst->tables_list);

	g_rand_free (inst->rand);
	machine_list_free (inst->machines_list);
	plugin_list_free (inst->plugins_list);
	g_object_unref (inst->builder);
	g_free (inst->settings);

	recursive_remove (inst->tmp_dir);

	g_free (inst->tmp_dir);
	g_free (inst->data_dir);
	g_free (inst->config_dir);
	g_free (inst->machines_dir);
	g_free (inst->plugins_dir);
	

	g_free (inst);
	_synema_instance (1); // set the instance to NULL
}
