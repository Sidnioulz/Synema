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
 * \file frame-new-dialog.h
 * \brief Header for the New Frame dialog in Synema
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This header code file contains functions described in frame-new-dialog.h.
 * These functions are used in the New Frame dialog of Synema.
 */
#ifndef __FRAME_NEW_DIALOG_H
#define __FRAME_NEW_DIALOG_H
#ifdef __cplusplus
extern "C" {
#endif

#include "frame-common.h"
#include "frame-private.h"
#include "frame-utilities.h"
#include "plugins.h"



/** \privatesection
 * \struct _frame_new_dialog_t
 * \brief Private data structure used to pass parameters to functions
 *
 * \warning This is a private data structure.
 *
 * This data structure is used for passing several pointers to handler functions
 * that have a fixed signature and can't be given several parameters.
 */
struct _frame_new_dialog_t {
	GtkBuilder	*builder;	/**< \brief The GtkBuilder for the frame dialog */
	frame_t		*frame;		/**< \brief A temp frame used to store some data */
};
/** \publicsection */



/** \fn void frame_populate_time_period_combo (GtkWidget *, plugin_t *, time_period_t *)
 * \brief Fills a combo box with the list of time periods available for a given plugin
 *
 * This function fills a combo box with a list time periods available for a plugin
 * passed in parameter. The combobox's model must be (gchararray, gpointer).
 * The third parameter can either be NULL or a valid time period. If it's not
 * NULL and if a line of the combobox matches it, then this line will be defined
 * as active.
 * @param[in,out] combo the combo box to populate with a list of time periods
 * @param[in] plugin a pointer to the plugin representing the current plugin type
 * @param[in] active_period the time period to set as active in the box, NULL to ignore
 */
void frame_populate_time_period_combo (GtkWidget *, plugin_t *, time_period_t *);



/** \fn void frame_populate_tool_type_combo (GtkWidget *, machine_t *, gchar *)
 * \brief Fills a combo box with the list of plugins available for a given machine
 *
 * This function fills a combo box with a list of plugins available for a machine
 * passed in parameter. The combobox's model must be (gchararray, gchararray).
 * The third parameter can either be NULL or a valid plugin type. If it's not
 * NULL and if a line of the combobox matches it, then this line will be defined
 * as active.
 * @param[in,out] combo the combo box to populate with a list of plugins
 * @param[in] machine the machine whose plugins list will be used
 * @param[in] active_tool the plugin type to set as active in the box, NULL to ignore
 */
void frame_populate_tool_type_combo (GtkWidget *, machine_t *, gchar *);



/** \fn void frame_populate_machine_combo (GtkWidget *, GList *, machine_t *)
 * \brief Fills a combo box with the list of currently available machines
 *
 * This function fills a combo box with the list of machines available from the
 * current Synema instance.
 * @param[in,out] combo the combo box to populate with a list of machines
 * @param[in] machines_list the list of machines to put in the combo box
 * @param[in] active_machine the machine to set as active, NULL to ignore
 */
void frame_populate_machine_combo (GtkWidget *, GList *, machine_t *);



/** \fn void frame_on_new (GtkToolButton *, gpointer)
 * \brief Displays a New Frame dialog
 *
 * This function displays a dialog that allows creating a new frame.
 * @param[in] button the button which was clicked to trigger the handler
 * @param[in] user_data a pointer to the main GUI's GtkBuilder, currently not used
 */
void frame_on_new (GtkToolButton *, gpointer);
#ifdef __cplusplus
}
#endif
#endif
