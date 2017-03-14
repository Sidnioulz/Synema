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
 * \file menu_functions.h
 * \brief Correlation plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 *
 * This header file contains functions for the SELinux plugin.
 */
#ifndef __LSMENU_H
#define __LSMENU_H
#ifdef __cplusplus
extern "C" {
#endif

#include "frame-common.h"
#include "time-period.h"

GList *build_func_list ();
GList *build_time_periods ();




/** \fn * (GtkMenuItem *item, gpointer user_data)
 * \brief Change the graphic type.
 *
 * These functions change the graphic type according to the item  
 * selected in the menu.
 * @param[in] item the item selected in the menu
 * @param[in] user_data data of the frame which needs to refresh its content
 */
void correlation_sensor_bars (GtkMenuItem *item, gpointer user_data);
void time_correlation (GtkMenuItem *item, gpointer user_data);
void sequences (GtkMenuItem *item, gpointer user_data);


/** \fn text_* (GtkMenuItem *item, gpointer user_data)
 * \brief Display logs.
 *
 * These functions display the logs for a particular sensor.
 * @param[in] item the item selected in the menu
 * @param[in] user_data data of the frame which needs to refresh its content
 */
void text_osiris (GtkMenuItem *item, gpointer user_data);
void text_selinux (GtkMenuItem *item, gpointer user_data);
void text_snort (GtkMenuItem *item, gpointer user_data);
void text_syslog (GtkMenuItem *item, gpointer user_data);




/** \fn all_matrix (GtkMenuItem *item, gpointer user_data)
 * \brief Display matrix mode for all machines.
 *
 * This function displays every criterion value for each
 * available machine
 * @param[in] item the item selected in the menu
 * @param[in] user_data data of the frame which needs to refresh its content
 */
void all_matrix (GtkMenuItem *item, gpointer user_data);


/** \fn * (GtkMenuItem *item, gpointer user_data)
 * \brief Display matrix for one criterion.
 *
 * These functions display a matrix with one criterion value for
 * each available machine
 * @param[in] item the item selected in the menu
 * @param[in] user_data data of the frame which needs to refresh its content
 */
void criticality_matrix (GtkMenuItem *item, gpointer user_data);
void sequences_number (GtkMenuItem *item, gpointer user_data);
void sequences_criticality (GtkMenuItem *item, gpointer user_data);
void activities_number (GtkMenuItem *item, gpointer user_data);


/** \fn rules_editor_menu (GtkMenuItem *item, gpointer user_data)
 * \brief Open the rules editor.
 *
 * This function open the rules editor in a terminal. It allows the user to
 * create new rules and to edit old ones
 * @param[in] item the item selected in the menu
 * @param[in] user_data data of the frame which needs to refresh its content
 */
void rules_editor_menu (GtkMenuItem *item, gpointer user_data);


/** \fn reload_activities (GtkMenuItem *item, gpointer user_data)
 * \brief Reload activities and sequences structures.
 *
 * This function frees all the activites and sequences in f->priv 
 * and regenerates new ones
 * @param[in] item the item selected in the menu
 * @param[in] user_data data of the frame which needs to refresh its content
 */
void reload_activities (GtkMenuItem *item, gpointer user_data);


/** \fn debug_menu (GtkMenuItem *item, gpointer user_data)
 * \brief Display the file where errors are stocked
 *
 * This function displays the file where the errors who had happened during 
 * activities and sequences generation. It contains also activities and sequences 
 * who are stocked. 
 * @param[in] item the item selected in the menu
 * @param[in] user_data data of the frame which needs to refresh its content
 */
void debug_menu (GtkMenuItem *item, gpointer user_data);

#endif
#ifdef __cplusplus
}
#endif
