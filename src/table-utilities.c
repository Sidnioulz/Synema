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
 * \file table-utilities.c
 * \brief Source code for the functions related to LiveSecurity's frames table
 * \author Steve Dodier <sidnioulz@gmail.com>
 * \author Timothée Ravier <timothee.ravier.romain@gmail.com>
 *
 * This source code file contains functions useful for manipulating the GtkTable
 * containing all the frames in LiveSecurity's GUI.
 */
#include <gtk/gtk.h>

#include "frame-table.h"
#include "frame-utilities.h"
#include "table-utilities.h"
#include "stack.h"
#include "application-data.h"



static frame_t *hidden_stack_pop (frame_table_t *tab)
{
	g_return_val_if_fail (tab != NULL, NULL);

	gboolean 	ret		= FALSE;
	frame_t		*elem	= stack_pop (tab->hidden_stack);
	if (elem)
		ret = hidden_bar_remove_frame (tab, elem);

	if (ret)
		return elem;
	else
		return NULL;
}



static void hidden_stack_push (frame_table_t *tab, frame_t *elem)
{
	stack_push (tab->hidden_stack, elem);
	hidden_bar_add_frame (tab, elem);
}



void size_ratio_to_slots (frame_table_t *tab, size_ratio_t ratio, guint *width, guint *height)
{
	g_print ("\t CALLED: size_ratio_to_slots (%p, %d, %d, %d)\n", tab, ratio, *width, *height);
	g_return_if_fail (tab != NULL);

	switch (ratio) {
		case NORMAL:
			*width = NORMAL_WIDTH_SLOTS;
			*height = NORMAL_HEIGHT_SLOTS;
			break;
		case LARGE:
			*width = LARGE_WIDTH_SLOTS;
			*height = LARGE_HEIGHT_SLOTS;
			break;
		case FULL:
			*width = tab->width;
			*height = tab->height;
			break;
		default:
			g_warning ("size_ratio_to_slots: Invalid size ratio");
			*width = 0;
			*height = 0;
			break;
	}
	g_print ("\t END: size_ratio_to_slots\n");
}



frame_t *slot_data_get (frame_table_t *tab, guint col, guint row)
{
	g_print ("\t CALLED: slot_data_get (%p, %d, %d)\n", tab, col, row);
	g_return_val_if_fail (tab != NULL, NULL);

	if ((col >= tab->width) || (row >= tab->height)) {
 		g_warning ("slot_data_get: (%d;%d) is out of bounds for table %p with size (%d;%d)", col, row,
 					tab, tab->width, tab->height);
		return NULL;
	}

	g_print ("\t END: slot_data_get\n");
	return tab->slot_data[tab->width * row + col];
}



gboolean slot_data_is_empty (frame_table_t *tab, guint col, guint row, guint width, guint height)
{
	g_print ("\t CALLED: slot_data_is_empty (%p, %d, %d, %d, %d)\n", tab, col, row, width, height);
	g_return_val_if_fail (tab != NULL, FALSE);
	g_return_val_if_fail (width > 0, FALSE);
	g_return_val_if_fail (height > 0, FALSE);

	gint	i, j;


	// Checking for out of bounds addresses
	if ((col + width > tab->width) || (row + height > tab->height)) {
 		g_warning ("slot_data_is_empty: (%d-%d;%d-%d) is out of bounds for table %p with size (%d;%d)",
 					col, col+width, row, row+height,
 					tab, tab->width, tab->height);
		return FALSE;
	}


	// Checking for all the slots
	for (i=0; i<height; i++) {
		for (j=0; j<width; j++) {
			if (tab->slot_data[tab->width * (row+i) + (col+j)] != NULL) {
				g_debug ("slot_data_is_empty: slot (%d;%d) in table %p is not empty", row + i, col + j, tab);
				return FALSE;
			}
		}
	}
	
	g_debug ("slot_data_is_empty: Slots (%d;%d) to (%d;%d) in table %p are empty",
				col, row, col+width-1, row+height-1, tab);
	g_print ("\t END: slot_data_is_empty\n");
	return TRUE;
}



gboolean slot_data_set (frame_table_t *tab, frame_t *elem, guint col, guint row, guint width, guint height)
{
	g_print ("\t CALLED: slot_data_set (%p, %p, %d, %d, %d, %d)\n", tab, elem, col, row, width, height);
	g_return_val_if_fail (tab != NULL, FALSE);
	g_return_val_if_fail (width > 0, FALSE);
	g_return_val_if_fail (height > 0, FALSE);

	frame_t		*elem_tmp	= NULL;
	gint		i, j;


	// Checking for out of bounds addresses
	if ((col + width > tab->width) || (row + height > tab->height)) {
 		g_warning ("slot_data_set: (%d-%d;%d-%d) is out of bounds for table %p with size (%d;%d)",
 					col, col+width, row, row+height,
 					tab, tab->width, tab->height);
		return FALSE;
	}


	// Removing previously existing elements - not setting the new one now because
	// of possible side effects from table_remove, only if not setting the slots
	// to NULL
	if (elem) {
		for (i=0; i<height; i++) {
			for (j=0; j<width; j++) {
				elem_tmp = tab->slot_data[tab->width * (row + j) + (col + i)];
				if ((elem_tmp != NULL) && (elem != elem_tmp)) {
					g_debug ("slot_data_set: Slot (%d;%d) is already in use by frame %p, removing it from table %p",
								col+j, row+i, elem_tmp, tab);
					table_remove (tab, elem_tmp, FALSE);
					hidden_stack_push (tab, elem_tmp);
				}
			}
		}
	}


	// Now setting the new slot data
	for (i=0; i<height; i++) {
		for (j=0; j<width; j++) {
			tab->slot_data[tab->width * (row + i) + (col + j)] = elem;
		}
	}

	g_debug ("slot_data_set: Frame %p set in slots (%d;%d) to (%d;%d) in table %p",
				elem, col, row, col+width-1, row+height-1, tab);
	g_print ("\t END: slot_data_set\n");
	return TRUE;
}



gboolean slot_data_unset (frame_table_t *tab, guint col, guint row, guint width, guint height)
{
	g_print ("\t CALLED: slot_data_unset (%p, %d, %d, %d, %d)\n", tab, col, row, width, height);
	return slot_data_set (tab, NULL, col, row, width, height);
}



void slot_data_resize (frame_table_t *tab, guint old_width, guint old_height, guint width, guint height)
{
	g_print ("\t CALLED: slot_data_resize (%p, %d, %d, %d, %d)\n", tab, old_width, old_height, width, height);
	g_return_if_fail (tab != NULL);


	frame_t	*elem;
	frame_t	**slot_data_tmp = g_malloc (sizeof (frame_t *) * height * width);
	gint 	i, j;


	// Setting up a new table
	for (i=0; i<height; i++) {
		for (j=0; j<width; j++) {
			slot_data_tmp[width * i + j] = NULL;
		}
	}


	// Cleaning the old one by pushing all the frames to the stack. Calling functions
	// must put the frames back themselves.
	for (i=old_height-1; i>=0; i--) {
		for (j=old_width-1; j>=0; j--) {
			elem = table_remove_with_coords (tab, j, i);
			if (elem != NULL) {
				hidden_stack_push (tab, elem);
			}
		}
	}


	// Now putting the new slot_data in the table_frame_t
	g_free (tab->slot_data);	
	tab->slot_data = slot_data_tmp;

	g_print ("\t END: slot_data_resize\n");
}



gboolean table_try_pack_frame (frame_table_t *tab, frame_t *elem, guint col, guint row)
{
	g_print ("\tCALLED: table_try_pack_frame (%p, %p, %d, %d)\n", tab, elem, col, row);
	g_return_val_if_fail (tab != NULL, FALSE);
	g_return_val_if_fail (elem != NULL, FALSE);
	g_return_val_if_fail (elem->parent == tab, FALSE);

	guint		width, height;


	// Checking for out of bounds addresses
	size_ratio_to_slots (tab, elem->size, &width, &height);
	if ((col + width > tab->width) || (row + height > tab->height)) {
 		g_debug ("table_try_pack_frame: (%d-%d;%d-%d) is out of bounds for table %p with size (%d;%d)",
 					col, col+width, row, row+height,
 					tab, tab->width, tab->height);
		g_print ("\t END: table_try_pack_frame\n");
		return FALSE;
	}


	if (!slot_data_is_empty (tab, col, row, width, height)) {
		g_print ("\t END: table_try_pack_frame\n");
		return FALSE;
	} else {
		// Won't fail due to out of bounds, already checked above
		slot_data_set (tab, elem, col, row, width, height);
		gtk_table_attach_defaults (GTK_TABLE (tab->table), frame_get_root_widget (elem),
									col, col+width, row, row+height);
		elem->hidden = FALSE;
		elem->top_slot = row;
		elem->left_slot = col;
		g_print ("\t END: table_try_pack_frame\n");
		return TRUE;
	}
}



gboolean table_get_next_empty_slot (frame_table_t *tab, guint startcol, guint startrow,
												guint *emptycol, guint *emptyrow)
{
	g_print ("\tCALLED: table_get_next_empty_slot (%p, %d, %d, %d, %d)\n", tab, startcol, startrow, *emptycol, *emptyrow);
	g_return_val_if_fail (tab != NULL, FALSE);
	g_return_val_if_fail (startcol >= 0, FALSE);
	g_return_val_if_fail (startrow >= 0, FALSE);
	g_return_val_if_fail (emptycol != NULL, FALSE);
	g_return_val_if_fail (emptyrow != NULL, FALSE);

	gint		i, j;


	// Checking for out of bounds addresses
	if ((startcol > tab->width) || (startrow > tab->height)) {
 		g_warning ("table_get_next_empty_slot: Position (%d;%d) is out of bounds for table %p with size (%d;%d)",
 					startcol, startrow,
 					tab, tab->width, tab->height);
		return FALSE;
	}

	for (i=startrow; i<tab->height; i++) {
		for (j=startcol; j<tab->width; j++) {
			if (tab->slot_data[tab->width * i + j] == NULL) {
				*emptycol = j;
				*emptyrow = i;
				return TRUE;
			}
		}
	}

	g_print ("\t END: table_get_next_empty_slot\n");
	return FALSE;
}



frame_t *table_grab_next_frame_to_pack (frame_table_t *tab,
					guint startcol, guint startrow, guint *emptycol, guint *emptyrow)
{
	g_print ("\tCALLED: table_grab_next_frame_to_pack (%p, %d, %d, %d, %d)\n", tab, startcol, startrow, *emptycol, *emptyrow);
	g_return_val_if_fail (tab != NULL, NULL);
	g_return_val_if_fail (startcol >= 0, NULL);
	g_return_val_if_fail (startrow >= 0, NULL);
	g_return_val_if_fail (startcol < tab->width, NULL);
	g_return_val_if_fail (startrow < tab->height, NULL);


	frame_t		*frame			= NULL;
	frame_t		*tmp			= NULL;
	gboolean	found_empty		= FALSE;
	guint		i = 0, j = 0;


	while ((i < tab->height) && !frame) {
		j = 0;
		while ((j < tab->width) && !frame) {
			if (!found_empty) {
				if (tab->slot_data[tab->width * i + j] == NULL) {
					found_empty = TRUE;
					*emptyrow = i;
					*emptycol = j;
				}
			} else {
				tmp = tab->slot_data[tab->width * i + j];
				if (tmp) {
					if ((tmp->left_slot == j) && (tmp->top_slot == i)) {
						frame = tmp;
						table_remove (tab, tmp, FALSE);
					}
				}
			}
			j++;
		}
		i++;
	}


	if (!frame && !stack_empty (tab->hidden_stack) && (found_empty)) {
		frame = hidden_stack_pop (tab);
	}

	g_print ("\t END: table_grab_next_frame_to_pack\n");
	return frame;
}



void table_fill_empty_slots (frame_table_t *tab)
{
	g_print ("\t CALLED: table_fill_empty_slots (%p)\n", tab);
	g_return_if_fail (tab != NULL);

	if (tab->width == 0 || tab->height == 0)
		return;


	frame_t		*frame			= NULL;			/**< \brief A pointer to the next frame to pack */
	gboolean	can_pack		= TRUE;			/**< \brief Tells if there is still room to repack unpacked frames */
	gboolean	slots_available	= FALSE;		/**< \brief Tells if there is an empty slot available for packing */
	GList		*iter			= NULL;			/**< \brief A list iterator */
	GList		*unpacked		= NULL;			/**< \brief A list to store unpackable frames */
	guint		emptycol = 0, emptyrow = 0;		/**< \brief The index of the next empty slot in the table */
	guint		startcol = 0, startrow = 0;		/**< \brief The starting col and row indexes for looking for a frame to pack */


	// While there is an empty slot and while we have a frame to pack
	while ((frame = table_grab_next_frame_to_pack (tab, startcol, startrow, &emptycol, &emptyrow)) != NULL) {
		// If packing the frame fails, we store it in a list of unpacked frames, sorted by adding time
		if (!table_try_pack_frame (tab, frame, emptycol, emptyrow)) {
			unpacked = g_list_append (unpacked, frame);
		}
		// If it succeeds, we update the starting col and row, and try to repack the previously failed frames
		else {
			startcol = emptycol;
			startrow = emptyrow;
			
			if (unpacked) {
				can_pack = TRUE;
				slots_available = FALSE;
				iter = unpacked;
				do {
					// Getting a new empty slot
					if (!slots_available) {
						slots_available = table_get_next_empty_slot (tab,
														startcol, startrow,
														&emptycol, &emptyrow);
						// We really are out of empty slots, time to stop
						if (!slots_available) {
							can_pack = FALSE;
						}
					}

					// If a slot was found, trying to pack the next frame
					if (slots_available) {
						frame = iter->data;
						// When it succeeds, we notify we used the empty slot
						if (table_try_pack_frame (tab, frame, emptycol, emptyrow)) {
							iter = g_list_next (iter);
							unpacked = g_list_remove (unpacked, frame);
							startcol = emptycol;
							startrow = emptyrow;
							slots_available = FALSE;
						} else {
							iter = g_list_next (iter);
						}
					}
				} while (can_pack && (iter != NULL));
			}
		}
	}


	// All the frames that are still unpacked are left in the hidden frames stack
	if (unpacked) {
		iter = unpacked;
		do {
			frame = iter->data;
			hidden_stack_push (tab, frame);
		} while ((iter = g_list_next (iter)) != NULL);
	}

	g_list_free (unpacked);
	g_print ("\t END: table_fill_empty_slots\n");
}



void table_size_changed (GtkWidget *table, GtkAllocation *allocation, gpointer user_data)
{
	g_print ("\t CALLED: table_size_changed (%p)\n", user_data);
	g_return_if_fail (user_data != NULL);

	frame_table_t	*tab	= user_data;
	gint 			height, width;

	// Retrieving the new width and height
	// TODO remove SUPPOSED_FRAME_BORDER_SIZE
	height = allocation->height / (FRAME_HEIGHT_NORMAL + SUPPOSED_FRAME_BORDER_SIZE);
	width = allocation->width / (FRAME_WIDTH_NORMAL + SUPPOSED_FRAME_BORDER_SIZE);

	// Checking if the size did change
	if ((height != tab->height) || (width != tab->width)) {
		slot_data_resize (tab, tab->width, tab->height, width, height);

		tab->width = width;
		tab->height = height;

		// A GtkTable can't have a 0x0 size
		height = height ? height : 1;
		width = width ? width : 1;
		gtk_table_resize (GTK_TABLE (table), height, width);

		// Calling the function that fills empty slots only when the table is not empty (to optimise launch)
		if (tab->frames_list)
			table_fill_empty_slots (tab);
	}

	g_print ("\t END: table_size_changed\n");
}



gboolean table_add (frame_table_t *tab, frame_t *elem)
{
	g_print ("\t CALLED: table_add (%p, %p)\n", tab, elem);
	g_return_val_if_fail (tab != NULL, FALSE);
	g_return_val_if_fail (elem != NULL, FALSE);

	gboolean	added		= FALSE;
	gboolean	inbounds	= TRUE;
	guint		height, width;
	guint		col = 0, row = 0;

	// Retrieving the number of slots needed for the frame
	size_ratio_to_slots (tab, elem->size, &width, &height);


	// Looking for a satisfying empty slot
	while (table_get_next_empty_slot (tab, col, row, &col, &row) && !added && inbounds) {
		if (col + width > tab->width) {
	 		g_debug ("table_add: (%d-%d;%d-%d) is out of bounds for table %p with size (%d;%d)",
	 					col, col+width, row, row+height,
	 					tab, tab->width, tab->height);
			// Go to next line, it won't fit in this one
			col = 0;
			row++; // note that if this row is out of bounds, next test will spot it
		}
		
		if (row + height > tab->height) {
	 		g_debug ("table_add: (%d-%d;%d-%d) is out of bounds for table %p with size (%d;%d)",
	 					col, col+width, row, row+height,
	 					tab, tab->width, tab->height);
	 		// There is no way for the frame to fit, stop here
	 		inbounds = FALSE;
		} else if (slot_data_is_empty (tab, col, row, width, height)) {
			slot_data_set (tab, elem, col, row, width, height);
			gtk_table_attach_defaults (GTK_TABLE (tab->table), frame_get_root_widget (elem), col, col+width, row, row+height);
			elem->hidden = FALSE;
			elem->top_slot = row;
			elem->left_slot = col;
			added = TRUE;
		}
	}

	// If there was no room in the table, we shall put the frame in the hidden frames bar
	if (!added) {
		elem->top_slot = -1;
		elem->left_slot = -1;
		hidden_stack_push (tab, elem);
/*		table_stack_case1 (tab);*///FIXME
	}

	g_print ("\t END: table_add\n");
	return TRUE;
}



gboolean table_remove (frame_table_t *tab, frame_t *elem, gboolean fill_slots)
{
	g_print ("\t CALLED: table_remove (%p, %p, %s)\n", tab, elem, fill_slots? "TRUE":"FALSE");
	g_return_val_if_fail (tab != NULL, FALSE);
	g_return_val_if_fail (elem != NULL, FALSE);
	g_return_val_if_fail (elem->parent == tab, FALSE);

	GtkWidget	*elemwidget	= frame_get_root_widget (elem);
	guint		height, width;


	// Checking if the frame really is in the table
	if (elem->left_slot == -1) { //Suffices as it's always the case for frames outside of a table
		g_warning ("table_remove: Frame %p is not in table %p", elem, tab);
		return FALSE; 
	}
	if (gtk_widget_get_parent (elemwidget) != tab->table) {
		g_warning ("table_remove: Frame  %p is not in table %p (and has misleading slot values: (%d;%d))", elem, tab, elem->left_slot, elem->top_slot);
		return FALSE;
	}


	// Doing the actual removal
	size_ratio_to_slots (tab, elem->size, &width, &height);
	slot_data_unset (tab, elem->left_slot, elem->top_slot, width, height);

	g_object_ref (elemwidget);
	gtk_container_remove (GTK_CONTAINER (tab->table), elemwidget);
	elem->hidden = TRUE;
	elem->left_slot = -1;
	elem->top_slot = -1;
	
	if (fill_slots) {
		table_fill_empty_slots (tab);
	}

	g_print ("\t END: table_remove\n");
	return TRUE;
}



frame_t *table_remove_with_coords (frame_table_t *tab, guint col, guint row)
{
	g_print ("\t CALLED: table_remove_with_coords (%p, %d, %d)\n", tab, col, row);
	frame_t		*elem = slot_data_get (tab, col, row);

	if (elem != NULL)
		table_remove (tab, elem, FALSE);

	g_print ("\t END: table_remove_with_coords\n");
	return elem;
}









//table_reorganise_slots_to_make_larger_frame_fit
/*
gint table_stack_case1 (frame_table_t *tab)
{
	g_print ("\t CALLED: table_stack_case1\n");
	g_critical ("This function must be re-written");
	frame_t			*elem = NULL;
	guint			col, row, width, height;
	
	
	size_ratio_to_slots (tab, LARGE, &width, &height);
	
	col = tab->width - width;
	row = tab->height - height;
	
	
	g_print ("\n\n\nXXXXXXXXXXXXXXX table_stack_case1 %d ", width);
	if (!stack_empty (tab->hidden_stack)
		&& slot_data_is_empty (tab, col+1, row, 1, 1)
		&& slot_data_is_empty (tab, col, row+1, width, 1)
		&& slot_data_is_empty (tab, col-1, row+1, 1, 1)) {
		
		elem = slot_data_get (tab, col, row);
		table_remove (tab, elem, FALSE);
		table_add_with_coords (tab, elem, col-1, row+1);
		table_add_with_coords (tab, stack_pop (tab->hidden_stack), col, row);
	}
	
	return 0;
}
*/









gboolean table_frame_size_changed (frame_t *elem, size_ratio_t new_size)
{
	g_print ("\t CALLED: table_frame_size_changed (%p, %d)\n", elem, new_size);
	g_return_val_if_fail (elem != NULL, FALSE);
	g_return_val_if_fail (elem->parent != NULL, FALSE);
	g_return_val_if_fail (elem->top_slot != -1, FALSE);

	frame_t			*tmp_frame	= NULL;
	frame_table_t	*tab		= elem->parent;
	guint			height, width;
	guint			new_height, new_width;
	guint			tmp_height, tmp_width;
	gint			i,j;


	// Getting numbers of slots from size ratios
	size_ratio_to_slots (tab, new_size, &new_width, &new_height);
	size_ratio_to_slots (tab, elem->size, &width, &height);


	// When the size is not bigger than the previous one, we just change it and fill the empty slots
	if ((new_width <= width) && (new_height <= height)) {
		slot_data_unset (tab, elem->left_slot, elem->top_slot, width, height);
		slot_data_set (tab, elem, elem->left_slot, elem->top_slot, new_width, new_height);
		gtk_container_remove (GTK_CONTAINER (tab->table), frame_get_root_widget (elem));
		gtk_table_attach_defaults (GTK_TABLE (tab->table), frame_get_root_widget (elem),
									elem->left_slot, elem->left_slot + new_width,
									elem->top_slot, elem->top_slot + new_height);
		elem->size = new_size;
		table_fill_empty_slots (tab);
	}

	// When the size is bigger, we need to move all the frames after elem
	// To do so, we remove all of them and then use the fill_empty_slots function
	else {
		// We move from the bottom right slot to the current frame's slot
		for (i=tab->height-1; i>=elem->top_slot; i--) {
			for (j=tab->width-1; j>=(i==elem->top_slot ? elem->left_slot : 0); j--) {
				tmp_frame = slot_data_get (tab, j, i);
				// We make sure the frame isn't before the current one, and we remove the 
				// current one only outside of the loop to avoid race conditions by modifying elem->left/top_slot
				if (tmp_frame != NULL) {
					if ((tmp_frame->top_slot == i) && (tmp_frame->left_slot == j) && (tmp_frame != elem)) {
						size_ratio_to_slots (tab, tmp_frame->size, &tmp_width, &tmp_height);
						table_remove (tab, tmp_frame, FALSE);
						hidden_stack_push (tab, tmp_frame);
					}
				}
			}
		}

		table_remove (tab, elem, FALSE);
		hidden_stack_push (tab, elem);
		elem->size = new_size;
		table_fill_empty_slots (tab);
	}

	g_print ("\t END: table_frame_size_changed\n");
	return TRUE;
}




void table_swap_children (frame_table_t *tab, frame_t *child1, frame_t *child2)
{
	g_print ("\t CALLED: table_swap_children\n");
	g_critical ("Function not implemented");
	//TODO Make it !
}
