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
 * \file frame-utilities.h
 * \brief Header for the Synema frames' utility functions
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This header file contains utility functions that can be applied to
 * frame_t objects.
 */
#ifndef __FRAME_UTILITIES_H
#define __FRAME_UTILITIES_H
#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

#include "constants.h"
#include "frame-common.h"
#include "machine.h"
#include "time-period.h"



/** \fn frame_display_error_message (frame_t *, frame_error_t, const gchar *, ...)
 * \brief Displays an error message inside the frame
 *
 * This function displays an error message for a given frame.
 * @param[out] frame the frame in which the error should be displayed
 * @param[in] type the type of error that occurred
 * @param[in] message the printf-like format of the message to display
 * @param[in] ... the parameters matching message's format
 * frame
 */
void frame_display_error_message (frame_t *, frame_error_t, const gchar *, ...);



/** \fn double frame_get_height (frame_t *)
 * \brief Returns the height of the drawing area of a frame
 * \todo Implement this function for the Full Screen mode
 *
 * This function returns the height of the drawing area contained in a frame.
 * The height is available as a double.
 * @param[in] f the frame which drawing area's height you want
 * @return the height of the frame, in pixels
 */
double frame_get_height (frame_t *);



/** \fn double frame_get_width (frame_t *)
 * \brief Returns the width of the drawing area of a frame
 * \todo Implement this function for the Full Screen mode
 *
 * This function returns the width of the drawing area contained in a frame.
 * The width is available as a double.
 * @param[in] f the frame which drawing area's width you want
 * @return the width of the frame, in pixels
 */
double frame_get_width (frame_t *);



/** \fn gboolean frame_get_position (frame_t *, guint *)
 * \brief Gives the position of the frame in it's parent frame table's list of
 * frames, or returns FALSE if not attached
 *
 * This function returns whether the frame is in the table, and if it is, sets
 * the position parameter to the position of the frame in the frames list of it's
 * parent table. If it returns FALSE, position is left undetermined.
 * @param[in] frame the frame whose coordinates you want
 * @param[out] position the place where to store the position
 * @return TRUE if the frame is in the table, FALSE otherwise or on error
 */
gboolean frame_get_position (frame_t *, guint *);



/** \fn void frame_request_redraw (frame_t *)
 * \brief Requests the drawing area to be refreshed by GTK
 *
 * This function calls gtk_widget_queue_draw to get the drawing area to be
 * refreshed.
 * @param[in] f the frame which you want to be refreshed
 */
void frame_request_redraw (frame_t *);



/** \fn void frame_request_content_refresh (frame_t *)
 * \brief Requests the application to call the content refreshing function
 *
 * This function asks the main application to call the function that recomputes
 * ploticus images.
 * @param[in] f the frame which you want to recompute content
 */
void frame_request_content_refresh (frame_t *);



/* * \fn void frame_function_menu_used (GtkWidget *, gpointer)
 * \brief Sets a flag to TRUE when a frame's function menu is used.
 *
 * This function sets a frame's flag to TRUE when a frame's function menu is used,
 * and then disconnects the signal handler responsible for calling it.
 * @param[in,out] menu the function menu
 * @param[in] data a pointer to the frame whose menu was used
 */
//void frame_function_menu_used (GtkWidget *, gpointer);



/** \fn void frame_unset_error (frame_t *frame)
 * \brief Unsets the current error in a given frame
 *
 * This function hides the error info bar, shows the frame's drawing area, and
 * sets it's last error to ERROR_NONE
 * @param[in] frame the frame whose last error to unset
 */
void frame_unset_error (frame_t *);



/** \fn int frame_initialise (frame_t *, size_ratio_t, machine_t *, time_period_t *)
 * \brief Initialises a frame with given parameters
 *
 * \remark You should not need this function in your frame specific code.
 *
 * This function initialises a newly allocated frame_t struct. It first loads
 * the GUI via a GtkBuilder, then it assigns values to the struct's members.
 * 
 * It then builds the combo boxes of the frame and finally connects it to various
 * signals.
 *
 * It ends by calling the init_private() function.
 * @param[out] f the frame to initialise
 * @param[in] size the size wanted for the frame
 * @param[in] display_machine a pointer to the machine to display in the frame
 * @param[in] pref_period the time period to use by default for this frame
 * @return 0 if initialisation went well, another value otherwise
 */
int frame_initialise (frame_t *, size_ratio_t, machine_t *, time_period_t *);



/** \fn GtkWidget *frame_get_root_widget (frame_t *)
 * \brief Returns the root widget of a frame's GtkBuilder xml file
 *
 * \remark You should not need this function in your frame specific code.
 *
 * This function returns the widget at the root of the xml file loaded by the
 * GtkBuilder in a given frame_t struct.
 * @param[in] f the frame for which you need the root widget
 * @return the widget at the root of the frame's GtkBuilder
 */
GtkWidget *frame_get_root_widget (frame_t *);



/** \fn char *frame_get_absolute_reports_path (frame_t *, const char *, const char *)
 * \brief Returns the absolute path of a reports folder or file for a given frame
 *
 * This function, given a frame, returns the absolute path to it's reports folder
 * if the relpath parameter is not set, or returns the absolute path to the file
 * identified by relpath (which then is a relative path from the plugin's reports
 * folder). Note that the third parameter allows using a custom machine instead of
 * the frame's one.
 * @param[in] f the frame for which the reports folder's path is wanted
 * @param[in] relpath if set, returns the path to this file instead of the reports
 * @param[in] machine if set, use this machine instead of the frame's current machine
 * folder
 * @return the absolute path to a reports folder or a file in this folder. You
 * must free this path with g_free when you don't need it anymore.
 */
char *frame_get_absolute_reports_path (frame_t *, const char *, const char *);



/** \fn char *frame_get_absolute_data_path (frame_t *, const char *)
 * \brief Returns the absolute path of a data file for a given frame
 *
 * This function, given a frame, returns the absolute path to it's data folder
 * if the relpath parameter is not set, or returns the absolute path to the file
 * identified by relpath (which then is a relative path from the plugin's data
 * folder).
 * @param[in] f the frame for which the data folder's path is wanted
 * @param[in] relpath if set, returns the path to this file instead of the data
 * folder
 * @return the absolute path to a data folder or a file in this folder. You
 * must free this path with g_free when you don't need it anymore.
 */
char *frame_get_absolute_data_path (frame_t *, const char *);



/** \fn char *frame_get_absolute_tmp_path (frame_t *, const char *)
 * \brief Returns the absolute path of a temporary file for a given frame
 *
 * This function, given a frame, returns the absolute path to it's temporary folder
 * if the relpath parameter is not set, or returns the absolute path to the file
 * identified by relpath (which then is a relative path from the plugin's temporary
 * folder).
 * @param[in] f the frame for which the temporary folder's path is wanted
 * @param[in] relpath if set, returns the path to this file instead of the temporary
 * folder
 * @return the absolute path to a temporary folder or a file in this folder. You
 * must free this path with g_free when you don't need it anymore.
 */
char *frame_get_absolute_tmp_path (frame_t *, const char *);

#ifdef __cplusplus
}
#endif
#endif
