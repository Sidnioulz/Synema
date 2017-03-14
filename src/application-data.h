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
 * \file application-data.h
 * \brief Header for the application's data types and functions
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains data types describing a Synema instance.
 */
#ifndef __APPLICATION_DATA_H
#define __APPLICATION_DATA_H
#ifdef __cplusplus
extern "C" {
#endif
#include <dbus/dbus-glib-bindings.h>
#include <gtk/gtk.h>
#include <glib.h>

#include "data-types.h"
#include "frame-common.h"
#include "frame-table.h"
#include "settings-manager.h"



/** \struct synema_instance_t
 * \brief Data structure containing all the variables needed to run Synema
 *
 * This data structure contains all the variables that can be needed in any part
 * of a Synema instance. It contains lists of available frames and
 * machines, some Gtk objects that can be needed everywhere in the source code,
 * and a mutex for non-thread-safe library calls (such as libploticus).
 */
typedef struct __synema_instance_t {
	GList			*machines_list;		/**< \brief The list of available machines */
	GList			*plugins_list;		/**< \brief The list of available plugins */
	GtkBuilder 		*builder;			/**< \brief The GtkBuilder of the application's GUI */
	GRand	 		*rand;				/**< \brief A pseudo random number generator for frame IDs */
	enum {
		PANEL_NONE,
		PANEL_PLAYER,
		PANEL_OPEN,
		PANEL_ACTIONS
	}				current_panel;		/**< \brief An integer telling which panel is currently in use */

	char			*config_dir;		/**< \brief A folder for user configuration files */
	char			*data_dir;			/**< \brief A folder for the application's data files */
	char			*machines_dir;		/**< \brief A folder where to look for machines */
	char			*plugins_dir;		/**< \brief A folder containing the app's plugins */
	char			*tmp_dir;			/**< \brief A folder for cache data */

	settings_t		*settings;			/**< \brief A struct containing application settings */

	enum {
		PLAYER_STOPPED,
		PLAYER_PLAYING,
		PLAYER_PAUSED
	}				player_status;		/**< \brief The current status of the player */
	guint64			player_curr_time;	/**< \brief Current playing timestamp of the log player */
	DBusGProxy		*dbus_proxy;		/**< \brief The DBus proxy for the log player */
	DBusGConnection	*dbus_connection;	/**< \brief The DBus connection for the log player */

	GList			*tables_list;		/**< \brief A list of frame_table_t structs */
	frame_table_t	*current_table;		/**< \brief A pointer to the currently displayed table */
	guint			labelcount;			/**< \brief An integer used to name new tabs in the notebook */
} synema_instance_t;



/** \fn void error_dialog (const gchar *, ...)
 * \brief Displays an error dialog
 *
 * This function displays an error dialog, and takes as a parameter the message
 * to display, in the same way as printf and affiliated functions.
 * @param[in] message the printf-like format of the message to display
 * @param[in] ... the parameters matching message's format
 */
void error_dialog (const gchar *, ...);



/** \fn void about_dialog (GtkButton *, gpointer)
 * \brief Displays the about dialog of Synema
 *
 * This function displays the application's about dialog.
 * @param[in] button the about button that was clicked to call this function
 * @param[in] user_data useless data
 */
void about_dialog (GtkButton *, gpointer);



/** \fn void toggle_panels (GtkToolButton *, gpointer);
 * \brief Toggles the visibility of the side panel
 * This function toggles the visibility of the side panel containing the log
 * player, or the open frames panel
 * @param[in] button the button that was clicked to trigger the handler
 * @param[in] user_data an identifier to tell which panel to show
 */
void toggle_panels (GtkToolButton *, gpointer);



/** \fn int post_init_restore_frames ()
 * \brief Restores frames previously saved to files. Part of the app's init process
 *
 * This function is called after the Synema instance has been created. It
 * searches for previously saved frames and restores them.
 * @return 0 if restoring succeeded, another value otherwise
 */
gint post_init_restore_frames ();



/** \fn void pre_quit_save_frames (GtkObject *, gpointer)
 * \brief Saves the frames if needed, then quits the GTK main loop
 *
 * This function saves the currently opened frames if automatic frame saving is
 * enabled in the settings, and then calls gtk_main_quit ().
 * @param[in] object the object whose signal emission triggered the handler
 * @param[in] user_data an uninteresting pointer
 */
void pre_quit_save_frames (GtkObject *, gpointer);



/** \fn int recursive_remove (const gchar *)
 * \brief Recursively removes a file or folder pointed by path
 *
 * This function removes a file pointed by path, and if it is a folder, removes
 * the files it contains before removing the folder.
 * @param[in] path the path of the file or folder to remove
 * @return 0 if no error, -1 otherwise
 */
gint recursive_remove (const gchar *);



/** \fn guint unique_frame_id_generator (GList *list)
 * \brief Returns a number that can be used to identify a frame among a list
 *
 * This function returns a number that can be used as an identifier for a frame
 * that is being added to a list, that must be given as a parameter. The function
 * will pick a random number and make sure no other frame uses it.
 * @param[in] list a list of frames that already have an identifier
 * @return an unique identifier for a member of the list
 */
guint unique_frame_id_generator (GList *);



/** \fn synema_instance_t *synema_instance()
 * \brief Returns a pointer to the current Synema instance
 *
 * This function returns a pointer to the current instance of Synema (it
 * is implemented as a synema_instance_t static struct). If the instance
 * doesn't exist, it is created beforehand.
 * @return the current Synema instance, properly initialised
 */
synema_instance_t* synema_instance ();



/** \fn void synema_instance_free()
 * \brief Frees the current Synema instance
 *
 * \warning This function should not be called when the Gtk loop is running.
 *
 * This function frees the whole current Synema instance, by freeing all
 * of its members before freeing the synema_instance_t struct itself.
 */
void synema_instance_free ();
#ifdef __cplusplus
}
#endif
#endif
