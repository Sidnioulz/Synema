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
 * \file selinux.c
 * \brief SELinux plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 * \author Zaina AFOULKI <zaina.afoulki@ensi-bourges.fr>
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

/** \fn selinux_type_change (GtkMenuItem *item, gpointer user_data)
 * \brief Changes the graphic type when Pie/Bar graph is selected in the menu.
 *
 * This function changes the graphics from/to pie/bar when Pie/Bar graph 
 * is selected in the menu.
 * @param[in] item the item selected in the menu
 * @param[in] user_data data of the frame which needs to refresh its content
 */
void selinux_type_change(GtkMenuItem *item, gpointer user_data);



/** \fn selinux_* (GtkMenuItem *item, gpointer user_data)
 * \brief Change the graphic type.
 *
 * These functions change the graphic type according to the item  
 * selected in the menu.
 * @param[in] item the item selected in the menu
 * @param[in] user_data data of the frame which needs to refresh its content
 */
void selinux_type (GtkMenuItem *item, gpointer user_data);
void selinux_tclass (GtkMenuItem *item, gpointer user_data);
void selinux_machines (GtkMenuItem *item, gpointer user_data);
void selinux_tcontext_u (GtkMenuItem *item, gpointer user_data);
void selinux_tcontext_r (GtkMenuItem *item, gpointer user_data);
void selinux_tcontext_t (GtkMenuItem *item, gpointer user_data);
void selinux_scontext_u (GtkMenuItem *item, gpointer user_data);
void selinux_scontext_r (GtkMenuItem *item, gpointer user_data);
void selinux_scontext_t (GtkMenuItem *item, gpointer user_data);
void selinux_user (GtkMenuItem *item, gpointer user_data);
void selinux_result (GtkMenuItem *item, gpointer user_data);



/** \fn selinux_show_text (GtkMenuItem *item, gpointer user_data)
 * \brief Enables the text mode.
 *
 * This function displays the text logs when is  
 * selected in the menu.
 * @param[in] item the item selected in the menu
 * @param[in] user_data data of the frame which needs to refresh its content
 */
void selinux_show_text (GtkMenuItem *item, gpointer user_data);



#endif
#ifdef __cplusplus
}
#endif
