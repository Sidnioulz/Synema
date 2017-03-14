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
#ifndef __LSCORRELATION_H
#define __LSCORRELATION_H
#ifdef __cplusplus
extern "C" {
#endif

#include <pcre.h>

#include "frame-common.h"
#include "time-period.h"
#define NUMBER_OF_LINES 16

#define OVECCOUNT 33 


/** \fn void correlation_draw_axis (frame_t *f, cairo_t *cr, time_t current_time, int period_type);
 * \brief Displays axis 
 *
 * This function displays x and y axis for the activities graph.
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] current_time the date to put at the end of xaxis
 * @param[in] period_type number of the time_period
 */
void correlation_draw_axis (frame_t *f, cairo_t *cr, time_t current_time, int period_type);


/** \fn char * correlation_get_stub_name(frame_t * f, int i);
 * \brief Return the name of ystubs
 *
 * This function returns the name of a sensor, which will be displayed
 * as ystub name
 * @param[in] f the frame which needs to refresh its content
 * @param[in] i number of the sensor
 */
char * correlation_get_stub_name(frame_t * f, int i);


/** \fn int correlation_draw_xstubs (frame_t *f, cairo_t *cr, time_t current_time, int period_type);
 * \brief Draw xstubs 
 *
 * This function draw xstubs (separation between txo stubs depends of time period)
 * The stub name is print after a call to correlation_get_xstub_name().
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] current_time the date to put at the end of xaxis
 * @param[in] period_type number of the time_period
 */
int correlation_draw_xstubs (frame_t *f, cairo_t *cr, time_t current_time, int period_type);


/** \fn char * correlation_get_xstub_name(int period_type, struct tm * time_tm);
 * \brief Return the name of xstubs
 *
 * This function returns the date to be displayed as xstub name
 * in a format chosen accordingly to the time period
 * @param[in] period_type number of the time_period
 * @param[in] time_tm a structure with informations about the date displayed
 */
char * correlation_get_xstub_name(int period_type, struct tm * time_tm);


/** \fn char * correlation_get_period_name (int period_type);
 * \brief Return the duration of time period
 *
 * This function returns the number of seconds in the time period 
 * given as a parameter 
 * @param[in] period_name name of the time_period
 */
int correlation_get_period (int period_name);


/** \fn char * correlation_get_xaxis_title (int period_type);
 * \brief Return the name of xaxis
 *
 * This function returns the name to be displayed as yaxis title
 * @param[in] period_type number of the time_period
 */
char * correlation_get_xaxis_title (int period_type);


/** \fn char * correlation_get_yaxis_title (int period_type);
 * \brief Return the name of yaxis
 *
 * This function returns the name to be displayed as yaxis title
 * @param[in] period_type number of the time_period
 */
char * correlation_get_yaxis_title (int period_type);


/** \fn char * correlation_get_period_name (int period_type);
 * \brief Return the name of time period
 *
 * This function returns the name of the time period 
 * given as a parameter
 * @param[in] period_type number of the time_period
 */
char * correlation_get_period_name (int period_type);


/** \fn void draw_arrows_for_map (frame_t *f, cairo_t *cr);
 * \brief Draws a couple of arrows inside the frame.
 *
 * This function displays an image of arrows used to go
 * backward and forward in time when displaying graphs.  
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
void draw_arrows_for_map (frame_t *f, cairo_t *cr);



/** \fn int show_actual_date (frame_t * f, cairo_t * cr, int period_type, struct tm * time_tm);
 * \brief Display the current time interval.
 *
 * This function displays the date shown in the current graph.
 * The date is in different format depending of the selected time period.
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] period_type the time period selected in the menu
 * @param[in] time_tm a structure with informations about the date displayed
 */
int show_actual_date (frame_t * f, cairo_t * cr, int period_type, struct tm * time_tm);


/** \fn int print_title (frame_t * f, cairo_t * cr, char * title);
 * \brief Display frame title.
 *
 * This function displays the name of the selected menu
 * item in the frame.
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] title the string to print
 */
int print_title (frame_t * f, cairo_t * cr, char * title);


#endif
#ifdef __cplusplus
}
#endif
