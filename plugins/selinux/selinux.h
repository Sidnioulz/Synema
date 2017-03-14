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
#ifndef __LSSELINUX_H
#define __LSSELINUX_H
#ifdef __cplusplus
extern "C" {
#endif

#include "frame-common.h"
#include "time-period.h"
#define NUMBER_OF_LINES 17

char **get_machines_for_tool (char *prefix, char *tool);


/** \fn int selinux_count_alerts (char * path)
 * \brief Counts the number of alerts in a log file.
 *
 * This function counts the number of alerts represented by the 
 * number of lines in a log file
 * @param[in] path absolute path to the log file 
 */
int selinux_count_alerts (char * path);


/** \fn int selinux_generate_machines_script(frame_t *)
 * \brief Generates the ploticus script for all machines graphic
 *
 * This function creates a ploticus script (in the tmp folder) 
 * that is used to plot the graphic where all available machines 
 * are involved.
 */
int selinux_generate_machines_script(frame_t *f);



/** \fn void selinux_draw_alert_level_bar (frame_t *f, cairo_t *cr, double alert_level)
 * \brief Draws a bar with a gradient overlay according to the alert level.
 *
 * This function draws a bar with cairo that represents the current 
 * alert level in the displayed text logs. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] alert_level the current alert_level in the displayed text logs.
 */
void selinux_draw_alert_level_bar (frame_t *f, cairo_t *cr, double alert_level);



/** \fn void selinux_draw_scroll (frame_t *f, cairo_t *cr)
 * \brief Draws a scrolling area inside the frame.
 *
 * This function displays an image for the scrolling area 
 * inside the frame when  text logs are enabled.
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
void selinux_draw_scroll (frame_t *f, cairo_t *cr);



/** \fn char * selinux_ploticus_script_name (frame_t *f)
 * \brief Returns the ploticus script filename to be used.
 *
 * This function returns the name of the ploticus script to be used
 * for the current time period and graphic type.
 * @param[in] f the frame which needs to refresh its content
 */
char * selinux_ploticus_script_name (frame_t *f);



/** \fn int selinux_display_latest_text_alerts (frame_t *f, cairo_t *cr)
 * \brief Displays a specified number of alerts
 *
 * This function displays NUMBER_OF_LINES text alerts 
 * located data->line_number_offset lines before the EOF
 * for the current time period.
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
int selinux_display_latest_text_alerts (frame_t *f, cairo_t *cr);



/** \fn int selinux_display_text_statistics (frame_t *f, cairo_t *cr)
 * \brief Displays the number of alerts for each machine
 *
 * This function displays the number of alerts each mahine had 
 * during the last time period.
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
int selinux_display_text_statistics (frame_t *f, cairo_t *cr);



/** \fn int selinux_graphic_title (frame_t *f, char * title)
 * \brief Selects the graphic title for a graphic type
 *
 * This function selects the graphic title for each graphic type 
 * and puts it in title. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] title the string which contains the title
 */
char * selinux_graphic_title (frame_t * f);


void data_set_graph_type (char * graph_type);

#endif
#ifdef __cplusplus
}
#endif
