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
 * \file init_functions.h
 * \brief Correlation plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 *
 * This header file contains functions for the SELinux plugin.
 */
#ifndef __LSINITFUNCTIONS_H
#define __LSINITFUNCTIONS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <pcre.h>

#include "frame-common.h"
#include "time-period.h"
#include "criticality.h"




/** \fn int init_activity_list(frame_t *f, cairo_t *cr, int machine_index, char * machine_name)
 * \brief Init the activity list in f->priv.
 *
 * This function looks for the activities in logs files of all sensors it detects,
 * then stocks them in a structure. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] machine_index if All Machines mode is on, index of the machine for which the function is called 
 * @param[in] machine_name if All Machines mode is on, name of the machine for which the function is called
 */
int init_activity_list(frame_t *f, cairo_t *cr, int machine_index, char * machine_name);


/** \fn int init_color_array (frame_t * f)
 * \brief Init the color list.
 *
 * This function fills an array with color for each activity. 
 * @param[in] f the frame which needs to refresh its content
 */
int init_color_array (frame_t * f);


/** \fn int refresh_activity_list(frame_t *f, cairo_t *cr)
 * \brief Refresh the activity list in f->priv.
 *
 * If new logs have been written in a file, this function updates the
 * activity list. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
int refresh_activity_list(frame_t *f, cairo_t *cr);


/** \fn int free_activities_list (frame_t * f)
 * \brief Free lists in f->priv.
 *
 * @param[in] f the frame which needs to refresh its content
 */
int free_activities_list (frame_t * f);


/** \fn int init_sequences_list(frame_t *f, cairo_t *cr)
 * \brief Init the sequences list in f->priv.
 *
 * This function looks for the sequences in the activities list,
 * by a call to detect_sequences(), and stocks them in a structure. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
int init_sequences_list(frame_t *f, cairo_t *cr, int machine_index);


/** \fn GList * detect_sequences (frame_t * f, FILE * error_log_file, int sequence_nb, char * sequence, GList * sequence_tmp)
 * \brief Detect a sequence.
 *
 * This function looks for the sequences in the activities list,
 * and stocks them in a structure. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] error_log_file the file displayed when in debug mode
 * @param[in] sequence_nb the index of the sequence
 * @param[in] sequence the sub-sequence to detect
 * @param[in] sequence_tmp a list with sub-sequences already matched
 */
GList * detect_sequences (frame_t * f, FILE * error_log_file, int sequence_nb, char * sequence, int sub_sequence_number);


/** \fn int init_all_machines(frame_t *f, cairo_t *cr);
 * \brief Init the criterions arrays in f->priv.
 *
 * This function calls init_sequences_list() for every machine 
 * in order to init the criterions arrays 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
int init_all_machines(frame_t *f, cairo_t *cr);


/** \fn int init_sensors_list (frame_t * f);
 * \brief Init the sensors list in f->priv.
 *
 * This function looks for sensors for which some regex exists
 * It fills the sensors list and returns the number of sensors. 
 * @param[in] f the frame which needs to refresh its content
 */
int init_sensors_list (frame_t * f);


/** \fn int count_configuration_rules (frame_t * f, char * path);
 * \brief Init the sensors list in f->priv.
 *
 * This function counts the number of rules there is in 
 * a configuration file.
 * @param[in] f the frame which needs to refresh its content
 * @param[in] path the file for which rules will be counted
 */
int count_configuration_rules (frame_t * f, char * path);



/** \fn int read_activity (frame_t * f, cairo_t *cr, int value_rule_number, time_t log_time, char * sensor)
 * \brief Read a log activity
 *
 * This function detects a log activity (base on the value it matches)
 * and add it to the activities struct 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] value_rule_number the number og the value matched by the log
 * @param[in] log_time the time when the log was written
 * @param[in] sensor the sensor who wrote the log
 */
int read_activity (frame_t * f, cairo_t *cr, int value_rule_number, time_t log_time, char * sensor);


/** \fn int fill_activity_struct (frame_t * f, time_t log_date, int delta_t, int activity, char * sensor);
 * \brief Fill the activity structure
 *
 * This function modifies the activities list
 * @param[in] f the frame which needs to refresh its content
 * @param[in] log_time the time when the log was written
 * @param[in] delta_t the time interval in which the log can be added to an existing activity
 * @param[in] activity the number of the log activity
 * @param[in] sensor the sensor who wrote the log
 */
int fill_activity_struct (frame_t * f, time_t log_date, int delta_t, int activity, char * sensor);


#endif
#ifdef __cplusplus
}
#endif
