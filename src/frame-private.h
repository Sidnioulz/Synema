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
 * \file frame-private.h
 * \brief Header for the Synema frames' private functions
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This header file contains private utility functions for Synema frames.
 */
#ifndef __FRAME_PRIVATE_H
#define __FRAME_PRIVATE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "frame-common.h"
#include "frame-utilities.h"



/** \fn void frame_set_id (frame_t *, GList *)
 * \brief Sets the ID of a frame and creates a tmp folder for this frame
 *
 * This function sets an ID for a given frame, that is different from the ones
 * already in use by members of a frames list given as a parameter. Once a
 * suitable ID has been found, the function will create a tmp directory for this
 * frame.
 * @param[out] f the frame that needs an ID
 * @param[in] list the list of frames in which the frame will be put
 */
void frame_set_id (frame_t *, GList *);



/** \fn void frame_on_exit (GtkWidget *, gpointer)
 * \brief Called automatically when the quit button is clicked
 *
 * \warning This is a private function.
 *
 * This function removes the frame from the frames list, its parent container,
 * and then frees it.
 * @param[in] button the button that was clicked and triggered the event
 * @param[out] user_data a pointer to the frame to detach and destroy 
 */
void frame_on_exit (GtkWidget *, gpointer);



/** \fn frame_t *frame_new (char *type)
 * \brief Creates an frame_t struct with mapped functions, not initialised.
 *
 * This function allocates memory for a frame and makes sure to link it's
 * functions to a frame specific set of functions. The frame's type depends on
 * the parameter passed to the function, which is the name of a folder in the
 * plugins directory. It will return NULL if the frame type's plugin cannot
 * be loaded.
 * @param[in] type the type of frame to create
 * @return the created frame or NULL on error
 */
frame_t *frame_new (char *type);



/** \fn void frame_free (frame_t *)
 * \brief Destroys a frame
 *
 * \warning This is a private function.
 *
 * This function calls the free_private function of the frame, then destroys
 * its gtkbuilder, and and frees the frame and its type.
 * @param[out] f the frame to destroy
 */
void frame_free (frame_t *);



/** \fn void frame_free_uninitialised (frame_t *)
 * \brief Destroys a frame that has not been initialised yet
 *
 * \warning This is a private function.
 *
 * This function frees the dynamically allocated members of a frame_t struct
 * that are allocated in frame_new and then frees the frame itself.
 * @param[out] f the frame to destroy
 */
void frame_free_uninitialised (frame_t *);



/** \fn double frame_timed_refresh_content (frame_t *, int *)
 * \brief Refresh the content of a frame and returns the CPU time used
 *
 * \warning This is a private function.
 *
 * This function will call the one in charge of refreshing the content, but
 * it will also calculate the CPU time it takes to execute (with clock()).
 * @param[in,out] f the frame which needs to refresh its content
 * @param[out] call_result the return value of the content refreshing function
 * @return the time, in seconds, that the function took to execute
 */
double frame_timed_refresh_content (frame_t *, int *);



/** \fn void frame_machine_changed (GtkComboBox *, gpointer)
 * \brief Handler for the event of the display machine combo box
 *
 * \warning This is a private function.
 *
 * This function checks the display machine combo box of a frame to find the
 * new machine to display, then updates the display_machine member, and finally
 * calls a simplified handler written by plugin developers.
 * @param[in,out] box the combo box that emitted the signal
 * @param[out] user_data a pointer to the frame_t struct
 */
void frame_machine_changed (GtkComboBox *, gpointer);



/** \fn void frame_update_size_button (frame_t *)
 * \brief Changes the image of the size button in a frame
 *
 * \warning This is a private function.
 *
 * This function makes sure the image used in the size toggle button of a frame
 * is the one depicting the size mode that is not currently in use.
 * @param[in,out] f the frame for which the size button needs to be updated
 */
void frame_update_size_button (frame_t *);



/** \fn void frame_time_period_changed (GtkComboBox *, gpointer)
 * \brief Handler for the event of the time period combo box
 *
 * \warning This is a private function.
 *
 * This function checks the time period combo box of a frame to find the
 * new machine to display, then updates the display_period member, and finally
 * calls a simplified handler written by plugin developers.
 * @param[in,out] box the combo box that emitted the signal
 * @param[out] user_data a pointer to the frame_t struct
 */
void frame_time_period_changed (GtkComboBox *, gpointer);



/** \fn gboolean frame_button_event (GtkWidget *, GdkEventButton *,gpointer)
 * \brief Handler for button press / release events in the drawing area
 *
 * \warning This is a private function.
 *
 * This function handles mouse button events in a frame's drawing area. It
 * identifies the event type and calls the appropriate function written by
 * the plugin developers.
 * @param[in] area the drawing area where the event occurred
 * @param[in] event the button event struct
 * @param[in,out] user_data a pointer to the frame_t struct
 * @return FALSE
 */
gboolean frame_button_event (GtkWidget *, GdkEventButton *, gpointer);



/** \fn gboolean frame_expose_handler (GtkWidget *, GdkEventExpose *, gpointer)
 * \brief Handler for the expose events in the drawing area
 *
 * \warning This is a private function.
 *
 * This function handles expose events in a frame's drawing area. It retrieves
 * a cairo_t from the area's GdkWindow, clips it to the area that needs to be 
 * drawn again, and calls the plugin developers' draw_area function.
 * @param[in] area the drawing area where the event occurred
 * @param[in] event the expose event struct
 * @param[in,out] user_data a pointer to the frame_t struct
 * @return FALSE
 */
gboolean frame_expose_handler (GtkWidget *, GdkEventExpose *, gpointer);



/** \fn void frame_size_changed_event (GtkWidget *, gpointer)
 * \brief Handler for the clicked events on the size button
 *
 * \warning This is a private function.
 *
 * This function handles clicked events in a frame's resizing button. It toggles
 * the size between large and normal, and refreshes the image in the button.
 * @param[in] button the button that needs updating
 * @param[in,out] user_data a pointer to the frame_t struct
 */
void frame_size_changed_event (GtkWidget *, gpointer);



/** \fn gpointer frame_computing_thread (gpointer)
 * \brief Handler for the clicked events on the size button
 *
 * \warning This is a private function.
 *
 * This function is the data computing manager. It is running as a separate
 * thread, launched during initialisation.
 * @param[in,out] data the frame to which the thread is attached
 */
gpointer frame_computing_thread (gpointer);
#ifdef __cplusplus
}
#endif
#endif
