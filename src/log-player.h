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
 * \file log-player.h
 * \brief Header for functions related to the log player's GUI
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains the signature of functions used by the log player's GUI.
 */
#ifndef __LOG_PLAYER_H
#define __LOG_PLAYER_H
#ifdef __cplusplus
extern "C" {
#endif
#include <gtk/gtk.h>



/** \fn void player_set_status_label (guint64, const gchar *)
 * \brief Sets the player's status label according to the parameters
 *
 * This function changes the log player's status label to something of the form
 * "<prefix>\n<date> at <time>", date and time being calculated from the given
 * timestamp.
 * @param[in] timestamp the time stamp of the date and time to display
 * @param[in] prefix the string to put on the first line of the label
 */
void player_set_status_label (guint64 , const gchar *);



/** \fn void player_on_stop_button_pressed (GtkButton *, gpointer)
 * \brief Called when the stop button of the log player is pressed
 *
 * This function sets the player status to STOPPED and restores the sensitivity
 * of the log player GUI's widgets. It calls the log player to tell it to stop
 * if user_data is set to something different than 0. Use GINT_TO_POINTER to set
 * user_data.
 * @param[in] button the stop button that was pressed
 * @param[in] user_data an int indicating if the app should tell the player to stop (if != 0)
 */
void player_on_stop_button_pressed (GtkButton *, gpointer);


/** \fn guint64 player_date_to_timestamp (GDate *, guint, guint)
 * \brief Calculates a timestamp from a date and time
 *
 * This function calculates a timestamp from a GDate, using a struct tm. It then
 * sets the hour and minute flags of the struct tm before using mktime to get a
 * timestamp.
 * @param[in] date the GDate containing the date
 * @param[in] hours the number of hours in the time
 * @param[in] mins the number of minutes in the time
 */
guint64 player_date_to_timestamp (GDate *, guint, guint);



// TODO doc
void player_update_values_from_dbus ();



/** \fn void player_setup_widgets ()
 * \brief Setups the log player's widgets (connects handlers, etc)
 *
 * This function connects the log player panel's gtk widgets to their respective
 * handlers.
 */
void player_setup_widgets ();
#ifdef __cplusplus
}
#endif
#endif
