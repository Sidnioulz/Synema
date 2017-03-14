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
 * \file table-utilities.h
 * \brief Header for the functions related to LiveSecurity's frames table
 * \author Steve Dodier <sidnioulz@gmail.com>
 * \author Timothée Ravier <timothee.romain.ravier@gmail.com>
 *
 * This header file contains functions useful for manipulating the GtkTable
 * containing all the frames in LiveSecurity's GUI.
 */
#ifndef __TABLE_UTILITIES_H
#define __TABLE_UTILITIES_H
#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

#include "frame-common.h"
#include "frame-table.h"



/** \fn size_ratio_to_slots (size_ratio_t, guint *, guint *)
 * \brief Returns the size of a frame, in number of table slots
 *
 * This function fills the size_col and size_row parameters with the amount of
 * table slots needed for a given size ratio.
 * @param[in] tab the table for which the number of slots is wanted
 * @param[in] ratio the size ratio for which the number of slots is wanted
 * @param[out] slots_col the number of slots needed horizontally
 * @param[out] slots_row the number of slots needed vertically
 */
gint frame_get_size (size_ratio_t, guint *, guint *);



/** \fn slot_data_get (frame_table_t *tab, guint, guint)
 * \brief Gets the frame at the given coordinates in a table
 *
 * This function, given a table and coordinates, returns a pointer to the frame
 * contained in the table at these coordinates, or NULL if it is empty
 * @param[in] tab the table from which to get a frame
 * @param col the horizontal index of the frame in the table
 * @param row the vertical index of the frame in the table
 * @return the desired frame, or NULL if there was none
 */
frame_t *slot_data_get (frame_table_t *, guint, guint);



/** \fn gboolean slot_data_is_empty (frame_table_t *, guint, guint, guint, guint)
 * \brief Tells whether a set of slots is empty in a table
 *
 * This function checks whether the slots from top-left slot identified by col
 * and row to bottom-right slot identified by col+width and row+height are all
 * free in the slot_data of the table passed as a parameter.
 * @param[in] tab the table in which to check for empty slots
 * @param[in] col the column index of the top left slot
 * @param[in] row the row index of the top left slot
 * @param[in] width the width of the area in which empty slots are wanted
 * @param[in] height the height of the area in which empty slots are wanted
 * @return TRUE if the slots are empty, FALSE otherwise
 */
gboolean slot_data_is_empty (frame_table_t *tab, guint, guint, guint, guint);



/** \fn slot_data_set (frame_table_t *, frame_t *, guint, guint, guint, guint)
 * \brief Sets the frame passed as a parameter as the one in the identified slots
 * as the one contained in the table
 *
 * \warning This function will remove and send to the stack any frame that was
 * previously in the table in the slots where elem is being added.
 *
 * Set the slot_data structure with the given frame, ensuring no frame is already attached at the given coordinates.
 * @param[in] tab
 * @param elem the frame to attach
 * @param row
 * @param col
 */
gboolean slot_data_set (frame_table_t *, frame_t *, guint, guint, guint, guint);


















//TODO slot_data_unset

/** \fn slot_data_resize (frame_table_t *tab, guint old_rows, guint old_cols, guint rows, guint cols)
 * \brief Resize the slot_data stucture
 *
 * Resize the slot_data structure, without taking into account frame.
 * Frame managment is done in the table_resize func.
 * @param[in] tab
 * @param old_rows
 * @param old_cols
 * @param rows
 * @param cols 
 * @return 0
 */
void slot_data_resize (frame_table_t *, guint, guint, guint, guint);



















/** \fn table_find_slot (frame_table_t *tab, frame_t *elem, guint *row, guint *col)
 * \brief Find a valid slot for a frame, and set coordinates in row and col.
 *
 * Try to find a slot or a couple of free slots to place a frame in the table.
 * @param[in] tab
 * @param elem
 * @param row
 * @param col
 * @return O if ok, else 1
 */
gboolean table_find_slot (frame_table_t *, size_ratio_t, guint *, guint *);



/** \fn table_size_changed (frame_table_t *tab, GtkAllocation *allocation, gpointer user_data)
 * \brief This is a function called by a g_signal_connect which changes table size
 *
 * This function is called by a g_signal_connect connected to size-allocate which warns about widget size changes.
 * It takes care of all moving and stacking stuff.
 * @param[in] table
 * @param allocation
 * @param user_data
 */
void table_size_changed (GtkWidget *, GtkAllocation *, gpointer);



/** \fn table_find_frame (fame_table_t *tab, frame_t *elem, guint *, guint *)
 * \brief Find a frame, starting to lok at the given coordinates.
 *
 * FIXME
 * @param[in] tab
 * @return O
 */
gboolean table_find_frame (frame_table_t *, guint *, guint *);



/** \fn table_frame_fits (frame_table_t *, guint, guint, guint *, guint *)
 * \brief FIXME
 *
 * FIXME
 * @param[in] tab
 * @return O
 */
gboolean table_frame_fits (frame_table_t *, guint, guint, guint *, guint *);



/** \fn table_fill_empty_slots (frame_table_t *tab)
 * \brief Fill the cheese
 *
 * Try to fill every empty slot in the table using the "list" algorithm.
 * @param[in] tab
 * @return O
 */
void table_fill_empty_slots (frame_table_t *tab);



/** \fn table_fill_with_stack (frame_table_t *tab)
 * \brief FIXME
 *
 * FIXME
 * @param[in] tab
 * @return O
 */
gint table_fill_with_stack (frame_table_t *);



/** 
 * \brief FIXME
 *
 * FIXME
 * @param[in] tab
 * @return O
 */
gint table_stack_case1 (frame_table_t *);



/** \fn table_get_optimal_nb_cols (frame_table_t *tab)
 * \brief Calculate the best table columns number.
 *
 * Calculate the best table columns number, using the supposed_frame_border_size constant.
 * @param[in] tab
 * @return The number of optimal columns
 */
gint table_get_optimal_nb_cols (frame_table_t *, guint);



/** \fn table_get_optimal_nb_rows (frame_table_t *tab)
 * \brief Calculate the best table rows number.
 *
 * Calculate the best table rows number, using the supposed_frame_border_size constant.
 * @param[in] tab
 * @return The number of optimal rows
 */
gint table_get_optimal_nb_rows (frame_table_t *, guint);



/** \fn gboolean table_add (frame_table_t *tab, frame_t *elem)
 * \brief Add a frame to the table
 *
 * Add a frame to the table taking care of stacking and moving.
 * @param[in] tab
 * @param elem
 * @return 0
 */
gboolean table_add (frame_table_t *, frame_t *);



/** \fn table_frame_size_changed (GtkWidget *table, frame_t *elem, guint old_size_x, guint old_size_y)
 * \brief Do the frame moving stuff when you change the size of one
 *
 * Manage the table when you change a frame's size.
 * @param[in] table
 * @param allocation
 * @param elem
 * @param user_data
 * @return
 */
gboolean table_frame_size_changed (frame_t *, size_ratio_t);



/** \fn table_remove (frame_table_t *tab, frame_t *elem)
 * \brief Remove the designated frame from the table
 *
 * Remove a frame from the table
 * @param[in] tab
 * @param elem
 * @return 0
 */
gboolean table_remove (frame_table_t *, frame_t *, gboolean);



/** \fn table_remove_with_coords (frame_table_t *tab, frame_t *elem)
 * \brief Remove the frame at the given coordinates, from the table
 *
 * Remove the frame at the given coordinates from the table
 * @param[in] tab
 * @param col
 * @param row
 * @return the removed frame
 */
frame_t * table_remove_with_coords (frame_table_t *, guint, guint);



/** \fn table_swap_children (frame_table_t *tab, frame_t *child1, frame_t *child2)
 * \brief Swaps a frame with another
 *
 * Swaps two frame, for drag and drop stuff
 * @param[in] tab
 * @param child1
 * @param child2
 */
void table_swap_children (frame_table_t *, frame_t *, frame_t *);

#ifdef __cplusplus
}
#endif
#endif
