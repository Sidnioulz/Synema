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
 * \file frame-new-dialog.c
 * \brief Source code for the functions used in the new frame dialog
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This source code file contains functions described in frame-new-dialog.h.
 * These functions are used in the New Frame dialog of Synema.
 */
#include <gtk/gtk.h>
#include <stdlib.h>

#include "application-data.h"
#include "frame-common.h"
#include "frame-new-dialog.h"
#include "frame-private.h"
#include "frame-utilities.h"
#include "machine.h"
#include "time-period.h"



/** \fn static gboolean time_period_combo_free_each (GtkTreeModel *,
 * GtkTreePath *, GtkTreeIter *, gpointer)
 * \brief Called automatically to free the data in a time period combo box
 *
 * \warning This is a private function.
 *
 * This function is called automatically on each member of the time period combo
 * box of a New Frame dialog, in order to free the data it points to.
 * @param[in] model the Tree Model for the combo box
 * @param[in] path the Tree Path for the combo box
 * @param[in] iter the iterator for the line whose data to free
 * @param[in] data some unneeded data passed by the calling function
 * @return FALSE (to make sure the function keeps being called on next item)
 */
static gboolean time_period_combo_free_each (GtkTreeModel *model, 
											GtkTreePath *path,
											GtkTreeIter *iter,
											gpointer data)
{
	gpointer	ptr;

	gtk_tree_model_get (model, iter, 1, &ptr, -1);
	time_period_free ((time_period_t *) ptr);

	return FALSE;
}



void frame_populate_time_period_combo (GtkWidget *combo, plugin_t *plugin, time_period_t *active_period)
{
	gchar				*name		= NULL;
	GtkListStore 		*store		= GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX(combo)));
	GtkTreeIter			iter		= {0};
	GList				*list		= NULL;
	GList				*list_iter	= NULL;

	gtk_tree_model_foreach (GTK_TREE_MODEL (store),
							time_period_combo_free_each,
							NULL);
	gtk_list_store_clear (store);

	if (plugin)
		list = plugin_get_time_periods (plugin);
	else
		return;

	if (list) {
		list_iter = list;
		do {
			time_period_t *p = list_iter->data;
			name = time_period_get_display_name (p, TRUE);
			gtk_list_store_append (store, &iter);
			gtk_list_store_set (store, &iter, 0, name, 1, (gpointer) p, -1);
			g_free (name);

			if (time_period_cmp (p, active_period) == 0)
				gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo), &iter);
		} while ((list_iter = g_list_next (list_iter)) != NULL);

		g_list_free (list);
	}

	if (gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) == -1)
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
}



/** TODO doc */
static gint g_strcmp0_func (gconstpointer a, gconstpointer b)
{
	return g_strcmp0 (a, b);
}


void frame_populate_tool_type_combo (GtkWidget *combo, machine_t *machine, gchar *active_tool)
{
	synema_instance_t	*inst	= synema_instance ();
	GList				*list	= NULL;
	GtkListStore 		*store	= GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX(combo)));
	GtkTreeIter			iter	= {0};

	gtk_list_store_clear (store);

	if (machine) {
		if (machine_is_all_machines (machine)) {
			char	*name		= NULL;
			char	*prev		= NULL;
			GList	*maciter	= inst->machines_list;
			GList	*allplugins	= g_list_copy (machine->monitoring_tools);
			GList	*pliter		= NULL;

			if (maciter) {
				do {
					machine_t *tmpmac = maciter->data;
					pliter = g_list_copy (tmpmac->monitoring_tools);
					if (pliter)
						allplugins = g_list_concat (allplugins, pliter);
				} while ((maciter = g_list_next (maciter)) != NULL);
			}

			allplugins = g_list_sort (allplugins, g_strcmp0_func);

			if (allplugins) {
				pliter = allplugins;
				do {
					prev = name;
					name = pliter->data;
					if (!g_strcmp0 (prev, name) == 0) {
						gtk_list_store_append (store, &iter);
						gtk_list_store_set (store, &iter, 0, plugin_get_display_name (plugin_list_find (name)), 1, name, -1);
						if (g_strcmp0 (name, active_tool) == 0)
							gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo), &iter);
					}
				} while ((pliter = g_list_next (pliter)) != NULL);
			}
			g_list_free (allplugins);
		
		} else if (machine->monitoring_tools) {
			list = machine->monitoring_tools;
			do {
				char *tool = list->data;
				gtk_list_store_append (store, &iter);
				gtk_list_store_set (store, &iter, 0, plugin_get_display_name (plugin_list_find (tool)), 1, tool, -1);
				if (g_strcmp0 (tool, active_tool) == 0)
					gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo), &iter);
			} while ((list = g_list_next (list)) != NULL);
		}
	}

	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (store), 0, GTK_SORT_ASCENDING);
	if (gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) == -1)
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
}



void frame_populate_machine_combo (GtkWidget *combo, GList *machines_list, machine_t *active_machine)
{
	GList				*list_iter	= NULL;
	GtkListStore 		*store		= GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX(combo)));
	GtkTreeIter			iter		= {0};

	gtk_list_store_clear (store);

	if (machines_list) {
		list_iter = machines_list;
		do {
			machine_t *mac = list_iter->data;
			const char *name = machine_get_display_name (mac);
			gtk_list_store_append (store, &iter);
			gtk_list_store_set (store, &iter, 0, name, 1, (gpointer)mac, -1);

			if (machine_cmp (active_machine, mac) == 0)
				gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo), &iter);
		} while ((list_iter = g_list_next (list_iter)) != NULL);
	}

	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (store), 0, GTK_SORT_ASCENDING);
	if (gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) == -1)
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
}



/** \fn void frame_new_dialog_tool_type_changed (GtkComboBox *, gpointer)
 * \brief Refreshes the time periods combobox when the plugin type combobox changes
 *
 * \warning This is a private function.
 *
 * This function is called when the plugin type combobox changes, and refreshes
 * the time periods combobox to reflect the periods available for this plugin.
 * @param[in] tooltype the combobox which changed
 * @param[in,out] user_data a pointer to the GtkBuilder of the dialog and a temporary frame_t
 */
static void frame_new_dialog_tool_type_changed (GtkComboBox *tooltype, gpointer user_data)
{
	synema_instance_t	*inst			= synema_instance ();
	gchar				*cur_tool		= NULL;
	GtkTreeIter			iter			= {0};
	GtkWidget			*period			= NULL;
	GValue				value			= {0,};
	plugin_t			**plugin		= (plugin_t **) user_data;
	time_period_t		cur_period		= {-1,-1};

	*plugin = NULL;

	if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (tooltype), &iter)) {
		gtk_tree_model_get_value (gtk_combo_box_get_model (GTK_COMBO_BOX(tooltype)),
									&iter, 1, &value);
		cur_tool = g_value_dup_string (&value);
		*plugin = plugin_list_find (cur_tool);
		g_free (cur_tool);
		g_value_unset (&value);
	}

	period = (GtkWidget *) gtk_builder_get_object (inst->builder, "time_combo");
	if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (period), &iter)) {
		gtk_tree_model_get_value (gtk_combo_box_get_model (GTK_COMBO_BOX(period)),
									&iter, 1, &value);
		time_period_t *ptr = (time_period_t *) g_value_get_pointer (&value);
		cur_period.type = ptr->type;
		cur_period.value = ptr->value;
		g_value_unset (&value);
	}

	frame_populate_time_period_combo (period, *plugin, &cur_period);

	g_signal_emit_by_name ((gpointer) period, "changed", user_data);

	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "create_button"),
							(gtk_combo_box_get_active (tooltype) != -1));
}



/** \fn void frame_new_dialog_machine_changed (GtkComboBox *, gpointer )
 * \brief Refreshes the plugin type combobox when the machines combobox changes
 *
 * \warning This is a private function.
 *
 * This function is called when the machines combobox changes, and refreshes
 * the plugin type combobox to reflect the plugins available for this machine.
 * @param[in] machine the combobox which changed
 * @param[in,out] user_data a pointer to the current plugin_t so it can be passed to child functions
 */
static void frame_new_dialog_machine_changed (GtkComboBox *machine, gpointer user_data)
{
	synema_instance_t	*inst			= synema_instance ();
	gchar				*cur_tool		= NULL;
	GtkTreeIter			iter			= {0};
	GtkWidget			*tooltype		= NULL;
	GValue				value			= {0,};
	machine_t			*cur_machine	= NULL;

	tooltype = (GtkWidget *) gtk_builder_get_object (inst->builder, "type_combo");

	if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (machine), &iter)) {
		gtk_tree_model_get_value (gtk_combo_box_get_model (GTK_COMBO_BOX(machine)),
									&iter, 1, &value);
		cur_machine = (machine_t *) g_value_get_pointer (&value);
		g_value_unset (&value);
	}

	if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (tooltype), &iter)) {
		gtk_tree_model_get_value (gtk_combo_box_get_model (GTK_COMBO_BOX(tooltype)),
									&iter, 1, &value);
		cur_tool = g_value_dup_string (&value);
		g_value_unset (&value);
	}

	g_signal_handlers_disconnect_by_func (tooltype,
					G_CALLBACK (frame_new_dialog_tool_type_changed),
					user_data);

	frame_populate_tool_type_combo (tooltype, cur_machine, cur_tool);
	g_free (cur_tool);

	g_signal_connect (tooltype, "changed",
					G_CALLBACK (frame_new_dialog_tool_type_changed),
					user_data);

	g_signal_emit_by_name ((gpointer) tooltype, "changed", user_data);
}



void frame_on_new (GtkToolButton *button, gpointer user_data)
{
	synema_instance_t	*inst			= synema_instance ();
	frame_t				*frame			= NULL;
	gchar				*path			= NULL;
	gchar				*cur_plugin		= NULL;
	gchar				*dlg_objects[]	= {"new_frame_dialog", "type_store", "time_store", "machine_store", "window-new_image", NULL};
	GError				*err			= NULL;
	GtkCellRenderer 	*renderer 		= NULL;
	GtkTreeIter			iter			= {0};
	GtkWidget			*dialog			= NULL;
	GtkWidget			*machine		= NULL;
	GtkWidget			*tooltype		= NULL;
	GtkWidget			*period			= NULL;
	GValue				value			= {0,};
	machine_t			*cur_machine	= NULL;
	plugin_t			*plugin			= NULL;
	size_ratio_t		cur_size		= NORMAL;
	time_period_t		*cur_period		= NULL;


	// Add a new dialog to the builder
	path = g_strdup_printf ("%s/window.ui", inst->data_dir);
	gtk_builder_add_objects_from_file (inst->builder, path, dlg_objects, &err);
	g_free (path);
	if (err) {
		g_warning ("frame_on_new: %s", err->message);
		g_clear_error (&err);
		return;
	}


	// Machines combo box
	machine = (GtkWidget *) gtk_builder_get_object (inst->builder, "machine_combo");
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (machine), renderer, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (machine), renderer,
									"text", 0, NULL);

	frame_populate_machine_combo (machine, inst->machines_list, NULL);
	g_signal_connect (machine, "changed",
					G_CALLBACK (frame_new_dialog_machine_changed),
					(gpointer) &plugin);


	// Monitoring tool combo box
	tooltype = (GtkWidget *) gtk_builder_get_object (inst->builder, "type_combo");
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (tooltype), renderer, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (tooltype), renderer,
									"text", 0, NULL);

	if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (machine), &iter)) {
		gtk_tree_model_get_value (gtk_combo_box_get_model (GTK_COMBO_BOX(machine)),
								&iter, 1, &value);
		cur_machine = (machine_t *) g_value_get_pointer (&value);
		g_value_unset (&value);
	}

	frame_populate_tool_type_combo (tooltype, cur_machine, NULL);
	g_signal_connect (tooltype, "changed",
					G_CALLBACK (frame_new_dialog_tool_type_changed),
					(gpointer) &plugin);


	// Time period combo box
	period = (GtkWidget *) gtk_builder_get_object (inst->builder, "time_combo");
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (period), renderer, FALSE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (period), renderer,
									"text", 0, NULL);

	if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (tooltype), &iter)) {
		gtk_tree_model_get_value (gtk_combo_box_get_model (GTK_COMBO_BOX(tooltype)),
									&iter, 1, &value);
		gchar *cur_tool = g_value_dup_string (&value);
		plugin = plugin_list_find (cur_tool);
		g_free (cur_tool);
		g_value_unset (&value);
	}

	frame_populate_time_period_combo (period, plugin, NULL);

	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object (inst->builder, "create_button"),
							(gtk_combo_box_get_active (GTK_COMBO_BOX (tooltype)) != -1));


	// Now running the dialog
	dialog = (GtkWidget *) gtk_builder_get_object (inst->builder, "new_frame_dialog");
	gint result = gtk_dialog_run (GTK_DIALOG (dialog));


	// Exploiting the result here - it's likely that some events changed the
	// value of each iter during gtk_dialog_run, so we grab the values again now
	// that the dialog is closed.
	switch (result) {
		case GTK_RESPONSE_OK:
			if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (machine), &iter)) {
				gtk_tree_model_get_value (gtk_combo_box_get_model (GTK_COMBO_BOX(machine)),
										&iter, 1, &value);
				cur_machine = (machine_t *) g_value_get_pointer (&value);
				g_value_unset (&value);
			}

			if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (tooltype), &iter)) {
				gtk_tree_model_get_value (gtk_combo_box_get_model (GTK_COMBO_BOX(tooltype)),
											&iter, 1, &value);
				cur_plugin = g_value_dup_string (&value);
				g_value_unset (&value);
			}

			if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (period), &iter)) {
				gtk_tree_model_get_value (gtk_combo_box_get_model (GTK_COMBO_BOX(period)),
											&iter, 1, &value);
				cur_period = (time_period_t *) g_value_get_pointer (&value);
				g_value_unset (&value);
			}

			// Turn the size to large if the large radio button is active
			if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON((GtkWidget *)
						gtk_builder_get_object (inst->builder, "size_large"))))
				cur_size = LARGE;
			frame = frame_new (cur_plugin);
			if (!frame) {
				g_warning ("frame_on_new: Could not create frame of type %s", cur_plugin);
				error_dialog ("<b>Failed to create frame</b>\n\nCould not create a frame for the plugin ‘%s’", cur_plugin);
			} else {
				if (frame_initialise (frame, cur_size, cur_machine, cur_period) == 0) {
					if (frame_table_add (inst->current_table, frame) != 0) {
						g_warning ("frame_on_new: Error while adding the new frame to the table");
						error_dialog ("<b>Failed to create frame</b>\n\nThe frame could not be added to the table");
					}
				} else {
					g_warning ("frame_on_new: Error while initialising the new frame");
					error_dialog ("<b>Failed to create frame</b>\n\nThe frame could not be initialised");
				}
			}
			
			g_free (cur_plugin);
			break;
		default:
			break;
	}

	gtk_tree_model_foreach (gtk_combo_box_get_model (GTK_COMBO_BOX(period)),
							time_period_combo_free_each,
							NULL);
	gtk_widget_destroy (dialog);
}
