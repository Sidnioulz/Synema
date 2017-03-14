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
 * \file frame-actions.c
 * \brief Source code for functions of the frame actions toolbutton menu
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains functions that are linked to the menu items of the
 * frame actions toolbutton menu.
 */

#include <glib.h>

#include "application-data.h"
#include "frame-actions.h"
#include "frame-utilities.h"
#include "plugins.h"



static gint time_period_cmp_func (gconstpointer a, gconstpointer b)
{
	return time_period_cmp (a, b);
}



static void frame_try_set_period (frame_t *frame, gpointer period)
{
	synema_instance_t	*inst		= synema_instance ();
	gboolean			found		= FALSE;
	gchar				*tmp		= NULL;
	GtkComboBox			*combo		= NULL;
	GtkTreeIter			iter;
	GtkTreeModel		*combomodel	= NULL;
	time_period_t		tmp_period	= {-1,-1};

	combo = GTK_COMBO_BOX (gtk_builder_get_object (frame->builder, "display_period"));
	combomodel = gtk_combo_box_get_model (combo);

	if (gtk_tree_model_get_iter_first (combomodel, &iter)) {
		found = FALSE;
		do {
			gtk_tree_model_get (combomodel, &iter, 1, &tmp_period.type, 2, &tmp_period.value, -1);
			if (time_period_cmp (&tmp_period, period) == 0) {
				gtk_combo_box_set_active_iter (combo, &iter);
				found = TRUE;
			}
		} while (!found && (gtk_tree_model_iter_next (combomodel, &iter)));

		if (!found) {
			if (inst->settings->unhandled_action_period == UA_ERROR_MESSAGE) {
				tmp = time_period_get_display_name (period, TRUE);
				frame_display_error_message (frame, ERROR_PERIOD, "This frame can't display data for the %s time period", tmp);
				g_free (tmp);
			} else if (inst->settings->unhandled_action_period == UA_TRY_NEAREST) {
				//TODO
			}
		}
	}

	if ((found) && (frame->last_err == ERROR_PERIOD)) {
		frame_unset_error (frame);
	}

}



static void frame_try_set_machine (frame_t *frame, gpointer machine)
{
	synema_instance_t	*inst		= synema_instance ();
	gboolean			found		= FALSE;
	gpointer			pointer		= NULL;
	GtkComboBox			*combo		= NULL;
	GtkTreeIter			iter;
	GtkTreeModel		*combomodel	= NULL;

	combo = GTK_COMBO_BOX (gtk_builder_get_object (frame->builder, "display_machine"));
	combomodel = gtk_combo_box_get_model (combo);

	if (gtk_tree_model_get_iter_first (combomodel, &iter)) {
		found = FALSE;
		do {
			gtk_tree_model_get (combomodel, &iter, 1, &pointer, -1);
			if (machine_cmp (pointer, machine) == 0) {
				gtk_combo_box_set_active_iter (combo, &iter);
				found = TRUE;
			}
		} while (!found && (gtk_tree_model_iter_next (combomodel, &iter)));

		if (!found && (inst->settings->unhandled_action_machine != UA_DO_NOTHING)) {
			frame_display_error_message (frame, ERROR_MACHINE, "Machine %s is not available for this frame", machine_get_display_name (machine));
		}
	}

	if ((found) && (frame->last_err == ERROR_MACHINE)) {
		frame_unset_error (frame);
	}
}



void frame_action_set_machine (GtkTreeView *view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	synema_instance_t	*inst	= synema_instance ();
	GList				*list	= NULL;
	gpointer			machine	= NULL;
	GtkTreeIter			iter;
	GtkTreeModel		*model	= gtk_tree_view_get_model (view);

	if (gtk_tree_model_get_iter (model, &iter, path)) {
		gtk_tree_model_get (model, &iter, 1, &machine, -1);

		list = inst->current_table->frames_list;
		if (list) {
			do {
				frame_try_set_machine (list->data, machine);
			} while ((list = g_list_next (list)) != NULL);
		}
	}
}



void frame_action_set_period (GtkTreeView *view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	synema_instance_t	*inst	= synema_instance ();
	GList				*list	= NULL;
	gpointer			period	= NULL;
	GtkTreeIter			iter;
	GtkTreeModel		*model	= gtk_tree_view_get_model (view);

	if (gtk_tree_model_get_iter (model, &iter, path)) {
		gtk_tree_model_get (model, &iter, 1, &period, -1);

		list = inst->current_table->frames_list;
		if (list) {
			do {
				frame_try_set_period (list->data, period);
			} while ((list = g_list_next (list)) != NULL);
		}
	}
}



void frame_action_setup_widgets ()
{
	synema_instance_t	*inst		= synema_instance ();
	gchar				*name		= NULL;
	GList				*iter		= NULL;
	GList				*list		= NULL;
	GList				*prev		= NULL;
	GtkCellRenderer 	*renderer	= NULL;
	GtkListStore		*store		= NULL;
	GtkTreeIter			treeiter;
	GtkTreeViewColumn	*column		= NULL;
	GtkWidget			*mac_view	= (GtkWidget *) gtk_builder_get_object (inst->builder, "action_treeview_set_machine");
	GtkWidget			*prd_view	= (GtkWidget *) gtk_builder_get_object (inst->builder, "action_treeview_set_period");


	// Setting up the time period tree view
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Name", renderer,
														"text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (prd_view), column);

	store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_view_set_model (GTK_TREE_VIEW(prd_view), GTK_TREE_MODEL(store));
	g_signal_connect (prd_view, "row-activated", G_CALLBACK (frame_action_set_period), NULL);


	// Getting the list of time periods for all plugins
	iter = inst->plugins_list;
	if (iter) {
		do {
			list = g_list_concat (list, plugin_get_time_periods (iter->data));
		} while ((iter = g_list_next (iter)) != NULL);
	}

	list = g_list_sort (list, time_period_cmp_func);
	iter = list;
	if (iter) {
		name = time_period_get_display_name (iter->data, TRUE);
		gtk_list_store_append (store, &treeiter);
		gtk_list_store_set (store, &treeiter, 0, name, 1, iter->data, -1);
		g_free (name);
		prev = iter;
		while ((iter = g_list_next (iter)) != NULL) {
			if (time_period_cmp (prev->data, iter->data)) {
				name = time_period_get_display_name (iter->data, TRUE);
				gtk_list_store_append (store, &treeiter);
				gtk_list_store_set (store, &treeiter, 0, name, 1, iter->data, -1);
				g_free (name);
			}
			prev = iter;
		}
	}

	g_list_free (list);
	g_object_unref (store);


	// Setting up the machine tree view
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Name", renderer,
														"text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (mac_view), column);

	store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_view_set_model (GTK_TREE_VIEW(mac_view), GTK_TREE_MODEL(store));
	g_signal_connect (mac_view, "row-activated", G_CALLBACK (frame_action_set_machine), NULL);


	// Filling the machine view
	if (inst->machines_list) {
		iter = inst->machines_list;
		do {
			gtk_list_store_append (store, &treeiter);
			gtk_list_store_set (store, &treeiter, 0, machine_get_display_name (iter->data), 1, iter->data, -1);
		} while ((iter = g_list_next (iter)) != NULL);
	}
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (store), 0, GTK_SORT_ASCENDING);

	g_object_unref (store);
}
