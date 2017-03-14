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
 * \file correlation.h
 * \brief Correlation plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 *
 * This header file contains functions for the SELinux plugin.
 */
#ifndef __LSTEXTMODE_H
#define __LSTEXTMODE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <pcre.h>

#include "frame-common.h"
#include "time-period.h"
#define NUMBER_OF_LINES 16

#define OVECCOUNT 33 

/** \fn int display_latest_text_alerts (frame_t *f, cairo_t *cr)
 * \brief Displays a specified number of alerts
 *
 * This function displays NUMBER_OF_LINES text alerts 
 * located data->line_number_offset lines before the EOF
 * for the current time period.
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] sensor the sensor for the log file being displayed
 */
int display_latest_text_alerts(frame_t *f, cairo_t *cr, char * sensor);


/** \fn int count_alerts (char * path)
 * \brief Counts the number of alerts in a log file.
 *
 * This function counts the number of alerts in a log file
 * @param[in] path absolute path to the log file 
 * @param[in] sensor the sensor for the log file being read
 */
int count_alerts (char * path, char * sensor);


/** \fn void draw_alert_level_bar (frame_t *f, cairo_t *cr, double alert_level)
 * \brief Draws a bar with a gradient overlay according to the alert level.
 *
 * This function draws a bar with cairo that represents the current 
 * alert level in the displayed text logs. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] alert_level the current alert_level in the displayed text logs.
 */
void draw_alert_level_bar (frame_t *f, cairo_t *cr, double alert_level);


/** \fn void draw_scroll (frame_t *f, cairo_t *cr)
 * \brief Draws a scrolling area inside the frame.
 *
 * This function displays an image for the scrolling area 
 * inside the frame when text logs are enabled.
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
void draw_scroll (frame_t *f, cairo_t *cr);


/** \fn char * get_log (char * sensor, char * log_line, FILE * log_file);
 * \brief Get a log from a log file
 *
 * This function extracts a log from a log file and returns it.
 * @param[in] sensor the sensor corresponding to the log file
 * @param[in] log_line the string to fill with the extracted log
 * @param[in] log_file the file from which the log must be extracted
 */
char * get_log (char * sensor, char * log_line, FILE * log_file);


/** \fn int move_to_next_line (frame_t *f, cairo_t * cr, double x);
 * \brief Go to next line
 *
 * This function goes to next line when displaying text with cairo
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] x the x position of the new line
 */
int move_to_next_line (frame_t *f, cairo_t * cr, double x);


/** \fn int show_text (frame_t *f, cairo_t * cr, char * text);
 * \brief Show text with cairo
 *
 * This function is called when the frame is a text mode. 
 * It displays text in the frame and go to the next line when needed. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] text the string to print in the frame
 */
int show_text (frame_t *f, cairo_t * cr, char * text);


/** \fn int set_file_cursor (frame_t * f, char * path, char * sensor, FILE * file, int lines_already_read);
 * \brief Show text with cairo
 *
 * This function is called when the frame is a text mode. 
 * It displays text in the frame and go to the next line when needed. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] path the path to the log file
 * @param[in] sensor the sensor of the lol file
 * @param[in] file the log file which will be read
 * @param[in] lines_already_read number of lines that have already been read and that we don't need to read anymore
 */
int set_file_cursor (frame_t * f, char * path, char * sensor, FILE * file, int lines_already_read);




#endif
#ifdef __cplusplus
}
#endif
