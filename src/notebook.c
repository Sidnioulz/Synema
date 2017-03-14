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
 * \file notebook.c
 * \brief Source code for functions related to notebook management
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains the source code of all the functions related to the frame
 * table notebook.
 */
#include <gtk/gtk.h>

#include "application-data.h"
#include "notebook.h"



static gint frame_table_cmp_func (gconstpointer a, gconstpointer b)
{
	return ((frame_table_t *)a)->table_box != b;
}



void notebook_on_page_switch (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data)
{
	synema_instance_t	*inst	= synema_instance ();
	GList				*item	= g_list_find_custom (inst->tables_list,
													gtk_notebook_get_nth_page (notebook, page_num),
													frame_table_cmp_func);
	if (item) {
		frame_table_hide_frames (inst->current_table);	
		inst->current_table = item->data;
		frame_table_show_frames (inst->current_table);
	}
}



void notebook_remove_page (GtkToolButton *donotuse, gpointer user_data)
{
	// Beware, don't change the order of these declarations
	synema_instance_t	*inst		= synema_instance ();
	frame_table_t		*tab		= (frame_table_t *) user_data;
	GtkWidget			*frame_box	= (GtkWidget *) gtk_builder_get_object (inst->builder, "notebook");
	gint				index		= gtk_notebook_page_num (GTK_NOTEBOOK (frame_box), tab->table_box);

	if (tab == inst->current_table)
		notebook_on_page_switch (GTK_NOTEBOOK (frame_box), NULL, 0, NULL);

	gtk_notebook_remove_page (GTK_NOTEBOOK (frame_box), index);
	inst->tables_list = g_list_remove (inst->tables_list, tab);
	if (!inst->tables_list)
		inst->current_table = NULL;
	frame_table_free (tab);
}



const gchar *notebook_get_tab_label (GtkNotebook *notebook, GtkWidget *child)
{
	const gchar	*buffer		= NULL;
	GtkWidget	*box		= gtk_notebook_get_tab_label (notebook, child);
	GList		*children	= gtk_container_get_children (GTK_CONTAINER (box));
	GList		*iter		= children;

	if (iter) {
		do {
			if (GTK_IS_EVENT_BOX (iter->data)) {
				iter = gtk_container_get_children (GTK_CONTAINER (iter->data));
				buffer = gtk_label_get_text (GTK_LABEL (iter->data));
				g_list_free (iter);
				g_list_free (children);
				return buffer; 
			}
		} while ((iter = g_list_next (iter)) != NULL);
	}

	g_list_free (children);
	return NULL;
}



static GtkWidget *notebook_new_page_label ()
{
	synema_instance_t	*inst		= synema_instance ();

	gchar *text = g_strdup_printf ("Unnamed %u", inst->labelcount);
	GtkWidget *label = gtk_label_new (text);
	g_free (text);

	return label;
}



static gboolean notebook_edit_tab_label (GtkEventBox *box, GdkEventButton *event, gpointer user_data)
{
	if (event->type == GDK_2BUTTON_PRESS) {
		synema_instance_t	*inst	= synema_instance ();
		GtkWidget			*label	= (GtkWidget *) user_data;
		GtkWidget			*dialog	= gtk_dialog_new_with_buttons ("Rename Tab",
									GTK_WINDOW (gtk_builder_get_object (inst->builder, "synema_window")),
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
									GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
									NULL);
		GtkWidget			*entry	= gtk_entry_new ();
		gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
		gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);
		gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area (GTK_DIALOG (dialog))), entry);
		gtk_entry_set_text (GTK_ENTRY (entry), gtk_label_get_text (GTK_LABEL (label)));
		gtk_widget_show_all (dialog);

		gint result = gtk_dialog_run (GTK_DIALOG (dialog));
		switch (result) {
			case GTK_RESPONSE_ACCEPT:
				gtk_label_set_text (GTK_LABEL (label), gtk_entry_get_text (GTK_ENTRY (entry)));
		}

		gtk_widget_destroy (dialog);
		return TRUE;
	}

	return FALSE;
}



frame_table_t *_notebook_add_page (GtkWidget *frame_box, const gchar *tablabel)
{
	synema_instance_t	*inst		= synema_instance ();
	frame_table_t		*tmptab		= frame_table_new ();
	GtkWidget			*button		= gtk_button_new ();
	GtkWidget			*eventbox	= gtk_event_box_new ();
	GtkWidget			*hbox		= gtk_hbox_new (FALSE, 0);
	GtkWidget			*icon		= gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
	GtkWidget			*label		= NULL;

	// Making sure to have a label
	inst->labelcount++;
	label = (tablabel==NULL) ? notebook_new_page_label () : gtk_label_new (tablabel);

	// Adding the label to the box and connecting it to click signals
	gtk_container_add (GTK_CONTAINER (eventbox), label);
	gtk_event_box_set_visible_window (GTK_EVENT_BOX (eventbox), FALSE);
	gtk_widget_set_events (eventbox, GDK_BUTTON_PRESS_MASK);
	g_signal_connect (eventbox, "button_press_event", G_CALLBACK (notebook_edit_tab_label), label);

	// Setting the button up
	gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);
	gtk_container_add (GTK_CONTAINER (button), icon);
	g_signal_connect (button, "clicked", G_CALLBACK (notebook_remove_page), tmptab);

	// Putting the widgets in the hbox	
	gtk_box_pack_start (GTK_BOX (hbox), eventbox, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
	gtk_widget_show_all (hbox);
	gtk_widget_show_all (tmptab->table_box);

	// Adding the page and tab to the notebook
	gtk_notebook_append_page (GTK_NOTEBOOK (frame_box), tmptab->table_box, hbox);
	gtk_notebook_set_current_page (GTK_NOTEBOOK (frame_box), -1);
	tmptab->position = gtk_notebook_get_current_page (GTK_NOTEBOOK (frame_box));
	gtk_widget_show (frame_box);
	inst->tables_list = g_list_prepend (inst->tables_list, tmptab);
	frame_table_hide_frames (inst->current_table);
	inst->current_table = tmptab;

	return tmptab;
}



void notebook_add_page (GtkToolButton *addbutton, gpointer user_data)
{
	_notebook_add_page (user_data, NULL);
}



void notebook_on_page_added (GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer user_data)
{
	if (gtk_notebook_get_n_pages (notebook) > 1) {
		gtk_notebook_set_show_tabs (notebook, TRUE);
	} else if (gtk_notebook_get_n_pages (notebook) == 1) {
		gtk_notebook_set_show_tabs (notebook, FALSE);
	}
}



void notebook_on_page_removed (GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer user_data)
{
	if (gtk_notebook_get_n_pages (notebook) > 1) {
		gtk_notebook_set_show_tabs (notebook, TRUE);
	} else if (gtk_notebook_get_n_pages (notebook) == 1) {
		gtk_notebook_set_show_tabs (notebook, FALSE);
	}
}



void notebook_remove_all ()
{
	synema_instance_t	*inst	= synema_instance ();

	while (inst->tables_list)
		notebook_remove_page (NULL, inst->tables_list->data);
}
