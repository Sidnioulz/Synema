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
 * \file settings-manager.c
 * \brief Source code for the settings_t structure and the settings loading and
 * saving functions.
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains the code for the settings loading and
 * saving functions.
 */
#include <gio/gio.h>

#include "application-data.h"
#include "frame-actions.h"
#include "settings-manager.h"



static gboolean settings_load_key_file (GKeyFile *keyfile, const gchar *config_dir, const gchar *data_dir)
{
	char		*path			= NULL;
	int			file_loaded		= 0;
	GError		*err			= NULL;

	// Try to open the configuration file
	path = g_strdup_printf ("%s/"APP_CONF_FILE, config_dir);
	g_key_file_load_from_file (keyfile, path, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &err);
	g_free (path);

	if (err == NULL) {
		file_loaded = 1;
	}
	// The config directory doesn't have a conf file, try to copy one from the data directory
	else {
		g_clear_error (&err);
		
		path = g_strdup_printf ("%s/"APP_CONF_FILE, data_dir);
		g_key_file_load_from_file (keyfile, path, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &err);
		g_free (path);
		
		if (err == NULL) {
			file_loaded = 2;
		} else {
			g_warning ("settings_load_from_file: %s", err->message);
			g_clear_error (&err);
		}
	}

	return file_loaded;
}



settings_t *settings_load (const gchar *config_dir, const gchar *data_dir)
{
	GError		*err			= NULL;
	GKeyFile	*keyfile		= g_key_file_new ();
	settings_t	*settings		= NULL;

	// Allocate a settings_t struct	
	settings = g_malloc (sizeof (settings_t));
	
	// If a configuration file has been found and loaded
	if (settings_load_key_file (keyfile, config_dir, data_dir)) {
		// Refresh frequency
		settings->refresh_freq = g_key_file_get_integer (keyfile,
							SETTINGS_GROUP_FRAMES, SETTINGS_REFRESH_FREQ_NAME, &err);
		if (err) {
			g_warning ("settings_load_from_file: Error while getting refresh frequency (%s)", err->message);
			settings->refresh_freq = SETTINGS_REFRESH_FREQ_DEFAULT;
			g_clear_error (&err);
		}

		// Frames automatic saving
		settings->autosave_frames = g_key_file_get_boolean (keyfile,
							SETTINGS_GROUP_FRAME_TABLES, SETTINGS_AUTOSAVE_FRAMES_NAME, &err);
		if (err) {
			g_warning ("settings_load_from_file: Error while checking for frame automatic saving (%s)", err->message);
			settings->autosave_frames = SETTINGS_AUTOSAVE_FRAMES_DEFAULT;
			g_clear_error (&err);
		}

		// Tab to set as current when frames saving is enabled
		settings->last_current_tab = g_key_file_get_integer (keyfile,
							SETTINGS_GROUP_FRAME_TABLES, SETTINGS_LAST_CURRENT_TAB_NAME, &err);
		if (err) {
			g_warning ("settings_load_from_file: Error while getting the last current tab (%s)", err->message);
			settings->last_current_tab = SETTINGS_LAST_CURRENT_TAB_DEFAULT;
			g_clear_error (&err);
		}

		// Frame actions: what to do on unhandled machine
		settings->unhandled_action_machine = g_key_file_get_integer (keyfile,
							SETTINGS_GROUP_FRAME_ACTIONS, SETTINGS_ACTION_SET_MACHINE_NAME, &err);
		if (err) {
			g_warning ("settings_load_from_file: Error while checking what to do on unhandled machine in a frame action (%s)", err->message);
			settings->unhandled_action_machine = SETTINGS_ACTION_SET_MACHINE_DEFAULT;
			g_clear_error (&err);
		}

		// Frame actions: what to do on unhandled time period
		settings->unhandled_action_period = g_key_file_get_integer (keyfile,
							SETTINGS_GROUP_FRAME_ACTIONS, SETTINGS_ACTION_SET_PERIOD_NAME, &err);
		if (err) {
			g_warning ("settings_load_from_file: Error while checking what to do on unhandled period in a frame action (%s)", err->message);
			settings->unhandled_action_period = SETTINGS_ACTION_SET_PERIOD_DEFAULT;
			g_clear_error (&err);
		}
		
	}
	// No conf file loaded, using default settings
	else {
		g_warning ("settings_load_from_file: No settings file found, returning default hardcoded settings.");
		settings->refresh_freq = SETTINGS_REFRESH_FREQ_DEFAULT;
		settings->autosave_frames = SETTINGS_AUTOSAVE_FRAMES_DEFAULT;
		settings->last_current_tab = SETTINGS_LAST_CURRENT_TAB_DEFAULT;
		settings->unhandled_action_machine = SETTINGS_ACTION_SET_MACHINE_DEFAULT;
		settings->unhandled_action_period = SETTINGS_ACTION_SET_PERIOD_DEFAULT;
	}

	g_key_file_free (keyfile);
	return settings;
}



gint settings_save (settings_t *settings)
{
	synema_instance_t	*inst		= synema_instance ();
	gchar				*path		= NULL;
	gchar				*buffer		= NULL;
	GError				*err		= NULL;
	GFile				*file		= NULL;
	GFileOutputStream	*stream		= NULL;
	GKeyFile			*keyfile	= g_key_file_new ();
	gsize				buff_len	= 0;

	// We load the current settings file if there is one so the user comments in the file are saved
	settings_load_key_file (keyfile, inst->config_dir, inst->data_dir);

	// Save settings into the KeyFile
	g_key_file_set_integer (keyfile, SETTINGS_GROUP_FRAMES, SETTINGS_REFRESH_FREQ_NAME, settings->refresh_freq);
	g_key_file_set_boolean (keyfile, SETTINGS_GROUP_FRAME_TABLES, SETTINGS_AUTOSAVE_FRAMES_NAME, settings->autosave_frames);
	g_key_file_set_integer (keyfile, SETTINGS_GROUP_FRAME_TABLES, SETTINGS_LAST_CURRENT_TAB_NAME, settings->last_current_tab);
	g_key_file_set_integer (keyfile, SETTINGS_GROUP_FRAME_ACTIONS, SETTINGS_ACTION_SET_MACHINE_NAME, settings->unhandled_action_machine);
	g_key_file_set_integer (keyfile, SETTINGS_GROUP_FRAME_ACTIONS, SETTINGS_ACTION_SET_PERIOD_NAME, settings->unhandled_action_period);

	// Turn the KeyFile into raw data
	buffer = g_key_file_to_data (keyfile, &buff_len, &err);
	g_key_file_free (keyfile);
	if (err) {
		g_warning ("settings_save: %s", err->message);
		g_clear_error (&err);
		return -1;
	}

	// Write the data to a file via an OutputStream
	path = g_strdup_printf ("%s/"APP_CONF_FILE, inst->config_dir);
	file = g_file_new_for_path (path);
	g_free (path);

	stream = g_file_replace (file, NULL, TRUE, G_FILE_CREATE_NONE, NULL, &err);
	if (err) {
		g_warning ("settings_save: %s", err->message);
		g_clear_error (&err);
		g_free (buffer);
		g_object_unref (file);
		return -1;
	}
	
	if (g_output_stream_write (G_OUTPUT_STREAM (stream), buffer, buff_len, NULL, &err) == -1) {
		g_warning ("settings_save: %s", err->message);
		g_clear_error (&err);
		g_free (buffer);
		g_object_unref (stream);
		g_object_unref (file);
		return -1;
	}
	
	g_free (buffer);
	g_object_unref (stream);
	g_object_unref (file);
	return 0;
}



static void settings_autosave_frames_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
	synema_instance_t	*inst			= synema_instance ();
	inst->settings->autosave_frames = gtk_toggle_button_get_active (togglebutton);
}



static void settings_act_machine_radio_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
	synema_instance_t	*inst			= synema_instance ();
	
	// This is connected to the Do nothing radio button
	if (gtk_toggle_button_get_active (togglebutton)) {
		inst->settings->unhandled_action_machine = UA_DO_NOTHING;
	} else {
		inst->settings->unhandled_action_machine = UA_ERROR_MESSAGE;
	}
}



static void settings_act_period_radio_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
	synema_instance_t	*inst		= synema_instance ();

	if (gtk_toggle_button_get_active (togglebutton)) {
		inst->settings->unhandled_action_period = UA_DO_NOTHING;
	} else if (gtk_toggle_button_get_active (user_data)) {
		inst->settings->unhandled_action_period = UA_ERROR_MESSAGE;
	} else {
		inst->settings->unhandled_action_period = UA_TRY_NEAREST;
	}
}



static void settings_auto_freq_radio_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
	synema_instance_t	*inst			= synema_instance ();
	if (gtk_toggle_button_get_active (togglebutton)) {
		inst->settings->refresh_freq = REFRESH_FREQ_AUTO;
	} else {
		inst->settings->refresh_freq = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (user_data));
	}
}



static void settings_fixed_freq_spin_changed (GtkSpinButton *spinbutton, gpointer user_data)
{
	synema_instance_t	*inst			= synema_instance ();
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gtk_builder_get_object (inst->builder, "settings_fixed_freq_radio")), TRUE);
	inst->settings->refresh_freq = gtk_spin_button_get_value_as_int (spinbutton);
}



void settings_dialog_show (GtkToolButton *addbutton, gpointer user_data)
{
	synema_instance_t	*inst			= synema_instance ();
	gchar				*dlg_objects[]	= {"settings_dialog", "settingsfreqadjustment", NULL};
	gchar				*path			= NULL;
	GError				*err			= NULL;


	// Add a new settings dialog to the builder
	path = g_strdup_printf ("%s/window.ui", inst->data_dir);
	gtk_builder_add_objects_from_file (inst->builder, path, dlg_objects, &err);
	g_free (path);
	if (err) {
		g_warning ("settings_dialog_show: %s", err->message);
		g_clear_error (&err);
		return;
	}


	GtkWidget		*am_nothing		= (GtkWidget *) gtk_builder_get_object (inst->builder, "settings_act_machine_nothing");
	GtkWidget		*am_errmsg		= (GtkWidget *) gtk_builder_get_object (inst->builder, "settings_act_machine_errmsg");
	GtkWidget		*ap_nothing		= (GtkWidget *) gtk_builder_get_object (inst->builder, "settings_act_period_nothing");
	GtkWidget		*ap_errmsg		= (GtkWidget *) gtk_builder_get_object (inst->builder, "settings_act_period_errmsg");
	GtkWidget		*ap_nearest		= (GtkWidget *) gtk_builder_get_object (inst->builder, "settings_act_period_nearest");
	GtkWidget		*auto_freq		= (GtkWidget *) gtk_builder_get_object (inst->builder, "settings_auto_freq_radio");
	GtkWidget		*dialog			= NULL;
	GtkWidget		*fixed_freq		= (GtkWidget *) gtk_builder_get_object (inst->builder, "settings_fixed_freq_radio");
	GtkWidget		*freq_spin		= (GtkWidget *) gtk_builder_get_object (inst->builder, "settings_fixed_freq_spin");
	GtkWidget		*save_frames	= (GtkWidget *) gtk_builder_get_object (inst->builder, "settings_autosave_frames");


	// Setting the dialog widgets' values accordingly to the current state of the settings
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (save_frames),
								inst->settings->autosave_frames);

	if (inst->settings->refresh_freq == REFRESH_FREQ_AUTO)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (auto_freq), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fixed_freq), TRUE);

	gtk_spin_button_set_value (GTK_SPIN_BUTTON (freq_spin),
		((inst->settings->refresh_freq > SETTINGS_REFRESH_FREQ_MIN) &&
		(inst->settings->refresh_freq < SETTINGS_REFRESH_FREQ_MAX)) ?
				inst->settings->refresh_freq : SETTINGS_REFRESH_FREQ_DEFAULT);
	gtk_spin_button_update (GTK_SPIN_BUTTON (freq_spin));

	if (inst->settings->unhandled_action_machine == UA_DO_NOTHING)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (am_nothing), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (am_errmsg), TRUE);


	if (inst->settings->unhandled_action_period == UA_DO_NOTHING)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ap_nothing), TRUE);
	else if (inst->settings->unhandled_action_period == UA_ERROR_MESSAGE)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ap_errmsg), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ap_nearest), TRUE);
	
	

	// Connect each setting widget to its handler
	// Frame automatic saving
	g_signal_connect (save_frames, "toggled", G_CALLBACK (settings_autosave_frames_toggled), NULL);

	// Refresh frequency
	g_signal_connect (auto_freq, "toggled", G_CALLBACK (settings_auto_freq_radio_toggled), freq_spin);
	g_signal_connect (freq_spin, "value-changed", G_CALLBACK (settings_fixed_freq_spin_changed), fixed_freq);

	// Unhandled machine on frame action
	g_signal_connect (am_nothing, "toggled", G_CALLBACK (settings_act_machine_radio_toggled), NULL);

	// Unhandled period on frame action - got to connect to all but one radio buttons to get all the toggled signals
	g_signal_connect (ap_nothing, "toggled", G_CALLBACK (settings_act_period_radio_toggled), ap_errmsg);
	g_signal_connect_swapped (ap_errmsg, "toggled", G_CALLBACK (settings_act_period_radio_toggled), GTK_TOGGLE_BUTTON (ap_nothing));



	// Getting the dialog to display
	dialog = (GtkWidget *) gtk_builder_get_object (inst->builder, "settings_dialog");


	// We can destroy the dialog after it's been run, any widget handler will change the settings immediately so there is nothing to save
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}
