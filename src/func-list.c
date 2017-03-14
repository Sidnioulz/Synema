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
 * \file application-data.c
 * \brief Source for frame menu items constructors and utilities
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This header file contains functions for the func_list_t structure that
 * represents a frame menu item.
 */
#include <glib.h>

#include "frame-common.h"
#include "func-list.h"



void func_list_add_new_func (GList **list, const char *name, void (*func)(GtkMenuItem *, gpointer))
{
	func_list_t *f = g_malloc (sizeof (func_list_t));

	f->type = FUNCTION;
	f->name = g_strdup (name);
	f->data.func = func;

	*list = g_list_prepend (*list, f);
}



void func_list_add_new_submenu (GList **list, const char *name, GList *children)
{
	func_list_t *f = g_malloc (sizeof (func_list_t));

	f->type = SUBMENU;
	f->name = g_strdup (name);
	f->data.children = children;

	*list = g_list_prepend (*list, f);
}



void func_list_add_new_separator (GList **list)
{
	func_list_t *f = g_malloc (sizeof (func_list_t));

	f->type = SEPARATOR;
	f->name = NULL;

	*list = g_list_prepend (*list, f);
}



void func_list_t_free (func_list_t *fn)
{
	if (fn) {
		g_free (fn->name);
		if (fn->type == SUBMENU)
			g_list_free (fn->data.children);
	}
	g_free (fn);
}



GtkWidget *func_list_t_setup_submenu (func_list_t *parent, frame_t *f)
{
	g_return_val_if_fail (parent != NULL, NULL);
	g_return_val_if_fail (parent->type == SUBMENU, NULL);

	GList		*iter		= parent->data.children;
	GtkWidget	*menu		= gtk_menu_new ();
	GtkWidget	*childmenu	= NULL;

	if (iter) {
		do {
			func_list_t *fn = iter->data;
			GtkWidget *item = NULL;
			
			switch (fn->type) {
				case FUNCTION:
					item = gtk_menu_item_new_with_label (fn->name);

					gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

					g_signal_connect (item, "activate", G_CALLBACK (fn->data.func), (gpointer) f);
					gtk_widget_show (item);
					break;
				case SEPARATOR:
					item = gtk_separator_menu_item_new ();
					gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
					gtk_widget_show (item);
					break;
				case SUBMENU:
					item = gtk_menu_item_new_with_label (fn->name);
					gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
					
					childmenu = func_list_t_setup_submenu (fn, f);
					gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), childmenu);

					gtk_widget_show (item);
					break;
			}
			func_list_t_free (fn);
		} while ((iter = g_list_next (iter)) != NULL);

		g_list_free (parent->data.children);
		parent->data.children = NULL;
	}

	return menu;
}
