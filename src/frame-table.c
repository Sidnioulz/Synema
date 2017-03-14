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
 * \file frame-table.c
 * \brief Source code for the tables containing frames
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This source code file implements the functions described in frame-table.h
 */
#include <gtk/gtk.h>

#include "application-data.h"
#include "frame-private.h"
#include "frame-table.h"
#include "table-utilities.h"



gboolean hidden_bar_add_frame (frame_table_t *tab, frame_t *f)
{
	g_return_val_if_fail (tab != NULL, FALSE);
	g_return_val_if_fail (f != NULL, FALSE);

	const gchar	*sizelabel	= NULL;
	gchar		*timelabel	= NULL;
	gchar		*tooltip	= NULL;
	GtkToolItem	*item		= NULL;

	item = gtk_tool_button_new (NULL, f->type);
	gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON (item), "synema-window");
	gtk_toolbar_insert (GTK_TOOLBAR (tab->hidden_bar), item, -1);
	gtk_widget_show (GTK_WIDGET (item));

	switch (f->size) {
		case NORMAL:
			sizelabel = NORMAL_LABEL;
			break;
		case LARGE:
			sizelabel = LARGE_LABEL;
			break;
		case FULL:
			sizelabel = FULL_LABEL;
			break;
		default:
			sizelabel = NULL;
	}

	timelabel = time_period_get_display_name (&f->display_period, TRUE);
	tooltip = g_strdup_printf ("<big>\t%s</big>\n\n<b>Machine:</b>\t\t %s\n<b>Time Period:</b>\t %s\n<b>Size:</b>\t\t\t %s",
								f->get_display_name (f),
								machine_get_display_name (f->display_machine),
								timelabel,
								sizelabel);
	gtk_tool_item_set_tooltip_markup (item, tooltip);
	g_free (tooltip);
	g_free (timelabel);

	f->hidden_item = item;

	return TRUE;
}



gboolean hidden_bar_remove_frame (frame_table_t *tab, frame_t *f)
{
	g_return_val_if_fail (tab != NULL, FALSE);
	g_return_val_if_fail (f != NULL, FALSE);
	g_return_val_if_fail (f->hidden_item != NULL, FALSE);

	gtk_container_remove (GTK_CONTAINER (tab->hidden_bar), GTK_WIDGET (f->hidden_item));
	f->hidden_item = NULL;

	return TRUE;
}



gint frame_table_list_cmp_by_position (gconstpointer a, gconstpointer b)
{
	return ((frame_table_t *)a)->position - ((frame_table_t *)b)->position;
}



/** TODO doc
 */
static void frame_table_toggle_frame_hidden_flag (frame_table_t *tab)
{
	if (tab == NULL)
		return;

	GList	*iter	= tab->frames_list;
	if (iter) {
		do {
			frame_t *frame = iter->data;
			frame->hidden ^= TRUE;
		} while ((iter = g_list_next (iter)) != NULL);
	}
}



void frame_table_hide_frames (frame_table_t *tab)
{
	frame_table_toggle_frame_hidden_flag (tab);
}



void frame_table_show_frames (frame_table_t *tab)
{
	frame_table_toggle_frame_hidden_flag (tab);
}



int frame_table_add (frame_table_t *tab, frame_t *newelem)
{
	g_return_val_if_fail (tab != NULL, -1);
	g_return_val_if_fail (newelem != NULL, -2); 

	frame_set_id (newelem, tab->frames_list);
	tab->frames_list = g_list_prepend (tab->frames_list, newelem);
	newelem->parent = tab;

	return !table_add (tab, newelem); // table_add returns TRUE(1) on success
}



int frame_table_remove (frame_table_t *tab, frame_t *elem)
{
	g_return_val_if_fail (tab != NULL, -1);
	g_return_val_if_fail (elem != NULL, -2);

	GList			*iter;

	iter = g_list_find (tab->frames_list, elem);
	
	if (iter) {
		int x =	!(table_remove (tab, elem, TRUE)); // Returns TRUE on success, we return 0 on success
		elem->parent = NULL;
		frame_free (elem);
		tab->frames_list = g_list_remove (tab->frames_list, elem);
		return x;
	} else {
		return -1;
	}
}



/** \fn static void machine_free_func (gpointer, gpointer)
 * \brief Frees a machine_t struct (GFunc version)
 *
 * \warning This is a private function.
 *
 * This function is a machine_free alias, that can be used as a GFunc.
 * @param[out] data the machine to free
 * @param[out] user_data a NULL gpointer
 */
static void frame_free_func (gpointer data, gpointer user_data)
{
	frame_free ((frame_t *) data);	
}



void frame_list_free (GList *list)
{
	if (list == NULL)
		return;

	g_list_foreach (list, frame_free_func, NULL);
	g_list_free (list);
}



void frame_table_free (frame_table_t *tab)
{
	g_free (tab->slot_data);
	stack_free (tab->hidden_stack);
	frame_list_free (tab->frames_list);
	// The GtkWidgets have been freed when the main window was destroyed.
	g_free (tab);
}



frame_table_t *frame_table_new ()
{
	frame_table_t	*tab	= g_malloc (sizeof (frame_table_t));
	GtkToolItem		*item	= gtk_tool_item_new ();

	tab->position = -1;


	// Set the HBox containing the table and bar
	tab->table_box = gtk_hbox_new (FALSE, 0);


	// Setup the table
	tab->table = gtk_table_new (1, 1, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (tab->table), 10);
	gtk_table_set_col_spacings (GTK_TABLE (tab->table), 10);
	gtk_table_set_homogeneous (GTK_TABLE (tab->table), TRUE);
	g_signal_connect (tab->table, "size-allocate", G_CALLBACK (table_size_changed), tab);

	tab->slot_data = NULL;
	tab->height = 0;
	tab->width = 0;
	
	gtk_box_pack_start (GTK_BOX (tab->table_box), tab->table, TRUE, TRUE, 0);
 
	
	// Set the hidden frames toolbar
	tab->hidden_bar = gtk_toolbar_new ();
	gtk_toolbar_set_orientation (GTK_TOOLBAR (tab->hidden_bar), GTK_ORIENTATION_VERTICAL);
/*	gtk_toolbar_set_icon_size (GTK_TOOLBAR (tab->hidden_bar), GTK_ICON_SIZE_SMALL_TOOLBAR);*/
	gtk_tool_item_set_expand (item, TRUE);
	gtk_toolbar_insert (GTK_TOOLBAR (tab->hidden_bar), item, 0);
	gtk_box_pack_start (GTK_BOX (tab->table_box), tab->hidden_bar, FALSE, TRUE, 0);


	// Begin with an empty frames list
	tab->frames_list = NULL;
	tab->hidden_stack = stack_new ();

	return tab;
}
