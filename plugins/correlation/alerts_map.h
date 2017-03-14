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
 * \file alerts_map.c
 * \brief Correlation plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 *
 * This header file contains functions for the SELinux plugin.
 */
#ifndef __LSALERTSMAP_H
#define __LSALERTSMAP_H
#ifdef __cplusplus
extern "C" {
#endif

#include <pcre.h>

#include "frame-common.h"
#include "time-period.h"
#include "criticality.h"

#define ACTIVITY_TYPE_FILE "activity_type"
#define INTRA_ACTIVITY_FILE "intra_activity_rules"
#define INTER_ACTIVITIES_FILE "inter_activity_rules"


/** \struct Activity_t
 * \brief Structure to store an activity occurrence
 *
 * This is a struct used to store data about an activity occurrence.
 */
typedef struct __Activity_t { 
	int 	sensor;			/**< \brief The sensor who detect the activity */
	time_t 	beginning_date;		/**< \brief The activity beginning date */
	time_t 	end_date; 		/**< \brief The activity end date */
	int	activity_links_number;	/**< \brief Number of intra/inter activities links pointing to the activity */
	int	sequence_links_number;	/**< \brief Number of sequences the activity appears in */
} Activity_t;	
	

/** \struct Activity_info_t
 * \brief Structure to draw intra-activity links
 *
 * This is a struct used to draw intra-activity links. 
 * It stores the beginning of related activities to draw links
 * between nearest activity only. 
 */
typedef struct __Activity_info_t { //Structure permettant de tracer les liens intra activités, inter sondes
	int 	sensor;			/**< \brief The sensor who detect the activity */
	time_t	beginning_date;		/**< \brief The activity beginning date */
} Activity_info_t;


/** \struct Sequence_t
 * \brief Structure to store a sub-sequence
 *
 * This is a struct used to store data about a sub-sequence. 
 * A sequence is store in a list of struct Sequence_t
 */
typedef struct __Sequence_t { 
	int sub_sequence_type; 		/**< \brief Type of the sub-sequence: 0 if it's an activity, 1 if it's a sequence */
	int sub_sequence_number;	/**< \brief Number of the sub-sequence */
	int sub_sequence_index; 	/**< \brief Number of the occurrence of the activity/sequence #sub_sequence_number */
} Sequence_t;	


/** \fn int time_correlation_display_alerts_map (frame_t *f, cairo_t *cr)
 * \brief Draw the alerts map
 *
 * This function draws axis and legend for the alerts map, 
 * then calls the function displaying alerts. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
int time_correlation_display_alerts_map(frame_t *f, cairo_t *cr);



/** \fn int get_sensor_number (frame_t *f, char * sensor);
 * \brief Return the sensor number
 *
 * This function returns the index of sensor it the sensor list
 * @param[in] f the frame which needs to refresh its content
 * @param[in] sensor the sensor 
 * @param[out] the sensor index
 */
int get_sensor_number (frame_t *f, char * sensor);


/** \fn int select_activity_color (cairo_t *cr, char * color);
 * \brief Select a color
 *
 * This function select the color cairo will use. 
 * @param[in] cr cairo surface
 * @param[in] color the color name
 */
int select_activity_color (cairo_t *cr, char * color);


/** \fn int intra_activity_handler (frame_t * f, cairo_t * cr, time_t current_time);
 * \brief Draw intra-activity
 *
 * This function draws intra-activity links, as well as activities themselves
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] current_time the date to put at the end of xaxis
 */
int intra_activity_handler (frame_t * f, cairo_t * cr, time_t current_time);


/** \fn int draw_link (frame_t * f, cairo_t * cr, time_t current_time, int sensor_1, int time_1, int sensor_2, int time_2, int act_1, int act_2, char * color_1, char * color_2);
 * \brief Draw a link
 *
 * This function looks for the place to draw what is asked
 * then, it draws link with cairo
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] current_time the date to put at the end of xaxis
 * @param[in] sensor_1 the sensor for the first activity
 * @param[in] time_1 the date of the first activity
 * @param[in] sensor_2 the sensor for the second activity
 * @param[in] time_2 the date of the second activity
 * @param[in] act_1 the number of the first activity
 * @param[in] act_2 the number of the second activity
 * @param[in] color_1 the color to use to display the first part of the link
 * @param[in] color_2 the color to use to display the second part of the link
 */
int draw_link (frame_t * f, cairo_t * cr, time_t current_time, int sensor_1, int time_1, int sensor_2, int time_2, int act_1, int act_2, char * color_1, char * color_2);


/** \fn int inter_activities_handler(frame_t * f, cairo_t * cr, time_t current_time);
 * \brief Draw inter-activities
 *
 * This function draws inter-activity links
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] current_time the date to put at the end of xaxis
 */
int inter_activities_handler(frame_t * f, cairo_t * cr, time_t current_time);


/** \fn gint sort_by_beginning_date (gconstpointer item1, gconstpointer item2);
 * \brief Compare two dates
 *
 * This function is used to sort a list by date.
 * It compares the dates of two list items 
 * @param[in] item1 the first list item
 * @param[in] item2 the second list item
 */
gint sort_by_beginning_date (gconstpointer item1, gconstpointer item2);


/** \fn int draw_legend (frame_t * f, cairo_t * cr, int activity, char * activity_name);
 * \brief Draw legend items
 *
 * This function draws a legend item
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] activity the item number (will determine item position)
 * @param[in] activity_name the name of the legend item
 */
int draw_legend (frame_t * f, cairo_t * cr, int activity, char * activity_name);


/** \fn int legend_handler (frame_t * f, cairo_t * cr, char * path);
 * \brief Draw legend
 *
 * This function draws a legend by calling draw_legend()
 * If a new item is added, refilled the activities structure
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] path the path of the file containing legend items' names
 */
int legend_handler (frame_t * f, cairo_t * cr, char * path);


/** \fn int display_error_log_file(frame_t *f, cairo_t *cr);
 * \brief Display error file
 *
 * This function is called when debug mode is selected in the menu
 * It displays the file with logs of all that happened during
 * the initialisation of the structures
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
int display_error_log_file(frame_t *f, cairo_t *cr);


/** \fn int draw_activities_confidence_plot (frame_t * f, cairo_t * cr, time_t current_time);
 * \brief Draw activities confidence plot
 *
 * This function draws the activities confidence plot by calling
 * draw_curve()
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] current_time the date to put at the end of xaxis
 */
int draw_activities_confidence_plot (frame_t * f, cairo_t * cr, time_t current_time);


/** \fn int draw_curve (frame_t * f, cairo_t * cr, time_t current_time, time_t time, int links_number);
 * \brief Draw confidence plot
 *
 * This function draws an item of the confidence plot 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] current_time the date to put at the end of xaxis
 * @param[in] time the time used to determine xaxis position 
 * @param[in] links_number the height of the bar
 */
int draw_curve (frame_t * f, cairo_t * cr, time_t current_time, time_t time, int links_number);


/** \fn int *** init_intra_activity_delta (frame_t * f, int *** intra_act_delta_array);
 * \brief Store intra_activity rules deltas
 *
 * This function reads the intra-activity file to find rules deltas
 * and returns them in a array. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] intra_act_delta_array the array in which intra_activity rules deltas are stored
 */
int *** init_intra_activity_delta (frame_t * f, int *** intra_act_delta_array);


/** \fn struct tm * fill_time_struct_for_drawing (frame_t * f, struct tm * end_time);
 * \brief Fill a struct tm to draw xaxis
 *
 * This function modify the content of an struct tm according to the current
 * time period. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] end_time the structure that needs to be filled
 */
struct tm * fill_time_struct_for_drawing (frame_t * f, struct tm * end_time);



#endif
#ifdef __cplusplus
}
#endif
