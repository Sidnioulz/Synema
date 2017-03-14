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
 * \file frame-table.h
 * \brief Header for the tables containing frames
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This header file describes the frame table and it's associated data, as well
 * as table related functions.
 */
#ifndef __FRAME_TABLE_H
#define __FRAME_TABLE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

#include "frame-common.h"
#include "stack.h"



/** \struct frame_table_t
 * \brief Data structure representing a table and the frames it contains
 *
 * This struct contains a GtkTable, which is the container of a list of frame_t's
 * drawing areas. It also contains data structure useful to table manipulation,
 * and the list of frames attached to the table, as well as the hidden frames
 * stack associated to the table
 */
typedef struct __frame_table_t {
	frame_t			**slot_data;		/**< \brief Represents the slots in the table. Getters and setters can be used
											 to tell which frame is in each slot */
	GList			*frames_list;		/**< \brief The list of frames currently in use in the application */
	gshort			position;			/**< \brief The position of the frame in the notebook */
	GtkWidget 		*table_box;			/**< \brief The box containing the table and bar below*/
	GtkWidget 		*table;				/**< \brief The table in which frames are contained */
	GtkWidget 		*hidden_bar;		/**< \brief The toolbar containing buttons for each hidden frame */
	guint			height;				/**< \brief Real height of the frame table (usually the same as the GtkTable, unless = 0) */
	guint			width;				/**< \brief Real width of the frame table (usually the same as the GtkTable, unless = 0) */
	Stack			*hidden_stack;		/**< \brief A stack for the currently hidden frames */
} frame_table_t;



/** \fn gboolean hidden_bar_add_frame (frame_table_t *, frame_t *)
 * \brief Adds a tool item to the hidden frames bar representing a frame
 *
 * This function, given a frame and a frame table, adds a visual representation
 * of the frame in the frame table's hidden frames toolbar, which consists of
 * a toolbutton with a tooltip.
 * @param[in,out] tab the frame table containing the hidden frames bar
 * @param[in] f the frame to add to the bar
 * @return TRUE in case of success, FALSE on error
 */
gboolean hidden_bar_add_frame (frame_table_t *, frame_t *);



/** \fn gboolean hidden_bar_remove_frame (frame_table_t *, frame_t *)
 * \brief Removes a given frame from the hidden frames bar
 *
 * This function, given a frame and a frame table, removes the frame's tool item
 * from the hidden frames toolbar. provided the frame has such an item set.
 * @param[in,out] tab the frame table containing the hidden frames bar
 * @param[in] f the frame to remove from the bar
 * @return TRUE in case of success, FALSE on error
 */
gboolean hidden_bar_remove_frame (frame_table_t *, frame_t *);



/** \fn gint frame_table_list_cmp_by_position (gconstpointer, gconstpointer)
 * \brief Compares two frame tables' position
 *
 * This function compares two frame tables. It returns the difference between
 * their position in the notebook.
 * @param[in] a a pointer to the first table
 * @param[in] b a pointer to the second table
 * @return the difference between the tables' position
 */
gint frame_table_list_cmp_by_position (gconstpointer, gconstpointer);



/** \fn void frame_table_hide_frames (frame_table_t *)
 * \brief Hides the frames of a frame table
 *
 * This functions sets the hidden flag of frames associated to a frame table to
 * TRUE.
 * @param[out] tab the frame table whose frames must be hidden
 */
void frame_table_hide_frames (frame_table_t *);



/** \fn void frame_table_show_frames (frame_table_t *)
 * \brief Shows the frames of a frame table
 *
 * This functions sets the hidden flag of frames associated to a frame table to
 * FALSE.
 * @param[out] tab the frame table whose frames must be shown
 */
void frame_table_show_frames (frame_table_t *);



/** \fn int frame_table_add (frame_table_t *, frame_t *)
 * \brief Adds a frame to the frames list
 *
 * This function adds a frame to a frames list, and puts it in the table linked
 * to this list.
 * @param[in,out] tab the frame table struct containing the table and frames list
 * @param[in] newelem a pointer to the new frame
 * @return 0 if adding the frame to the list and table was successful, another value on error
 */
int frame_table_add (frame_table_t *, frame_t *);



/** \fn int frame_table_remove (frame_table_t *, frame_t *)
 * \brief Removes a frame from the frames list
 *
 * This function removes a frame from the table given as a parameter,
 * frees it and removes it from the list associated to this table.
 * @param[in,out] tab the frame table containing the frame
 * @param[in] elem a pointer to the frame
 * @return 0 if removing the frame from the list and table was successful, another value on error
 */
int frame_table_remove (frame_table_t *, frame_t *);



/** \fn void frame_list_free (GList *)
 * \brief Frees a list of frames and it's content
 *
 * This function frees all the frames in a list and frees the list too.
 * @param[out] list the pointer to the list
 */
void frame_list_free (GList *);



/** \fn void frame_table_free (frame_table_t *)
 * \brief Frees a frame table, it's list of frames and it's internal data
 *
 * This function frees a frame table and all the frames it contains.
 * @param[out] tab a pointer to the frame table
 */
void frame_table_free (frame_table_t *);



/** \fn frame_table_t *frame_table_new ()
 * \brief Creates a frame table with an empty GtkTable
 *
 * This function allocates the memory for a new frame_table_t that should be freed
 * with frame_table_free, and initialises it's GtkTable and various private data
 * structures.
 * @return a new frame table structure
 */
frame_table_t *frame_table_new ();
#ifdef __cplusplus
}
#endif
#endif
