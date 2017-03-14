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
 * \file settings-manager.h
 * \brief Header for the settings_t structure and the settings loading and
 * saving functions.
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains the settings_t structure and the settings loading and
 * saving functions.
 */
#ifndef __SETTINGS_MANAGER_H
#define __SETTINGS_MANAGER_H
#ifdef __cplusplus
extern "C" {
#endif
#include "data-types.h"



/** \struct settings_t
 * \brief Data structure storing various application settings
 *
 * This data structure contains the current value of all the application settings.
 */
typedef struct __settings_t {
	refresh_freq_t	refresh_freq;		/**< \brief Global refresh frequency for the frames */

	gboolean		autosave_frames;	/**< \brief Tells if frames should be saved when quitting the app */
	gushort			last_current_tab;	/**< \brief Allows restoring the last current tab when autosave_frames is TRUE */

	gint			unhandled_action_machine;	/**< \brief What to do when an unhandled machine is set on a frame action */
	gint			unhandled_action_period;	/**< \brief What to do when an unhandled time period is set on a frame action */
} settings_t;



/** \fn settings_t *settings_load (const gchar *, const gchar *)
 * \brief Loads the application settings, by trying to read a settings file
 *
 * This function tries to read settings from a settings file in config_dir (and
 * in data_dir if there is no file in config_dir), and if there is no settings
 * file, it loads default hardcoded settings.
 * @param[in] config_dir the user's configuration directory where to look for the file
 * @param[in] data_dir the application's data directory where to look for a default file
 * @return the application's settings
 */
settings_t *settings_load (const gchar *, const gchar *);



/** \fn gint settings_save (settings_t *settings)
 * \brief Saves the application settings into a file in the user's configuration directory
 *
 * This function saves the application settings into a file in the user's
 * configuration directory.
 * @param[in] settings the settings to save
 * @return 0 if the file was actually saved, -1 on error
 */
gint settings_save (settings_t *);




void settings_dialog_show (GtkToolButton *addbutton, gpointer user_data);
#ifdef __cplusplus
}
#endif
#endif
