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
 * \file notebook.h
 * \brief Header for functions related to notebook management
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains the declaration of all the functions related to the frame
 * table notebook.
 */
#ifndef __NOTEBOOK_H
#define __NOTEBOOK_H
#ifdef __cplusplus
extern "C" {
#endif
#include <gtk/gtk.h>

#include "frame-table.h"



void notebook_on_page_switch (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data);
void notebook_remove_page (GtkToolButton *donotuse, gpointer user_data);
const gchar *notebook_get_tab_label (GtkNotebook *notebook, GtkWidget *child);
frame_table_t *_notebook_add_page (GtkWidget *frame_box, const gchar *tablabel);
void notebook_add_page (GtkToolButton *addbutton, gpointer user_data);
void notebook_on_page_added (GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer user_data);
void notebook_on_page_removed (GtkNotebook *notebook, GtkWidget *child, guint page_num, gpointer user_data);
void notebook_remove_all ();

#ifdef __cplusplus
}
#endif
#endif
