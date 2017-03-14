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
 * \file edito.h
 * \brief Correlation plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 *
 * This header file contains functions for the SELinux plugin.
 */
#ifndef __EDITOR_H
#define __EDITOR_H
#ifdef __cplusplus
extern "C" {
#endif

#include <pcre.h>

#include "frame-common.h"
#include "time-period.h"

#define ACTIVITY_FILE_PATH "./plugins/correlation/data/activity_type"
#define INTRA_ACTIVITY_RULES_FILE "./plugins/correlation/data/intra_activity_rules"
#define INTER_ACTIVITY_RULES_FILE "./plugins/correlation/data/inter_activity_rules"
#define SEQUENCES_FILE "./plugins/correlation/data/sequences"

typedef struct
{
	char * sensor;
	char * log_line;
}log_struct;


/** \fn int rules_editor (frame_t * f, int quit);
 * \brief The fuction called when editor is selected in the menu
 *
 * This function handle the rules editor
 * @param[in] f the frame which needs to refresh its content
 * @param[in] quit allows to quit the editor when it is equal to 1
 */
int rules_editor (frame_t * f, int quit);


/** \fn int select_file ()
 * \brief Handle main menu.
 *
 * This function displays the main menu and return the user choice
 */
int select_file ();


/** \fn int file_edit (frame_t * f, int file_to_edit, log_struct ** logs, int log_index)
 * \brief Call the correct function.
 *
 * This function calls the one who will handle the configuration file to edit
 * @param[in] f the frame 
 * @param[in] file_to_edit the number of the selected entry menu
 * @param[in] logs the struct with the displayed logs
 * @param[in] log_index the number of displayed logs
 */
int file_edit (frame_t * f, int file_to_edit, log_struct ** logs, int log_index);


/** \fn int select_time_period ()
 * \brief Handle time_period.
 *
 * This function selects the ime period for which logs will be displayed
 */
int select_time_period();


/** \fn edit_*_file(frame_t * f, log_struct ** logs, int log_index)
 * \brief Edit configuration files.
 *
 * These functions are the ones called when the user edit a configuration file.
 * They can add a new line to a config file, or modified it, by calling other functions
 * @param[in] f the frame 
 * @param[in] logs the struct with the displayed logs
 * @param[in] log_index the number of displayed logs
 */
int edit_regex_file (frame_t * f, log_struct ** logs, int log_index);
int edit_value_file (frame_t * f, char * sensor, int regex_number);
int edit_activity_file (frame_t * f, log_struct ** logs, int log_index);
int edit_intra_activity_file (frame_t * f, log_struct ** logs, int log_index);
int edit_inter_activity_file (frame_t * f, log_struct ** logs, int log_index);
int edit_sequences_file (frame_t * f, log_struct ** logs, int log_index);


/** \fn time_t find_first_date(frame_t * f)
 * \brief Find logs first date.
 *
 * This function returns the first date of the logs it detects
 * to know what is the default beginning date for displaying logs
 * @param[in] f the frame 
 */
time_t find_first_date(frame_t * f);


/** \fn time_t get_older_date (char * sensor, char * log_line, time_t beginning_date)
 * \brief Return the older date.
 *
 * This function returns the older date between the one in a log line
 * and the one given as a parameter
 * @param[in] sensor the sensor of log_line
 * @param[in] log_line a log containing a date to be compared to beginning_date
 * @param[in] beginning_date the date to be compared to the one in log_line
 */
time_t get_older_date (char * sensor, char * log_line, time_t beginning_date);


/** \fn time_t get_date_from_log (char * sensor, char * log_line)
 * \brief Return log date.
 *
 * This function returns the date read in log_line
 * @param[in] sensor the sensor of log_line
 * @param[in] log_line a log containing a date
 */
time_t get_date_from_log (char * sensor, char * log_line);


/** \fn time_t get_user_date (int day, int month, int year)
 * \brief Generate a timestamp from a date.
 *
 * This function returns the timestamp of the date given as parameters
 * Th time used is 00:00:00
 * @param[in] day the day chosen by the user
 * @param[in] month the month chosen by the user
 * @param[in] year the year chosen by the user
 */
time_t get_user_date (int day, int month, int year);


/** \fn int display_logs (frame_t * f, int file_to_edit, time_t first_log_date, int delta, log_struct ** logs, long * log_pos)
 * \brief Display logs
 *
 * This function displays logs for every sensor in a given time period
 * @param[in] file_to_edit the number of the selected entry menu
 * @param[in] first_log_date the first date who will be displayed
 * @param[in] delta the time period to display logs
 * @param[in] logs the struct with the displayed logs
 * @param[in] log_index the number of displayed logs
 */
int display_logs (frame_t * f, int file_to_edit, time_t first_log_date, int delta, log_struct ** logs, long * log_pos, int display);


/** \fn FILE * find_date_position_in_file(char * sensor, char * path, time_t first_date_to_read, long current_offset);
 * \brief Find a date in a log file
 *
 * This function displays logs for every sensor in a given time period
 * @param[in] sensor the sensor of log_line
 * @param[in] path the path to the log file
 * @param[in] first_date_to_read the looked-for date
 * @param[in] current_offset the current position in the log file
 */
FILE * find_date_position_in_file(char * sensor, char * path, time_t first_date_to_read, long current_offset);


/** \fn int create_new_activity (frame_t * f, char * sensor)
 * \brief Create a new activity.
 *
 * This function allows the user to create a new activity
 * @param[in] f the frame 
 * @param[in] sensor the sensor of log_line
 */
int create_new_activity (frame_t * f, char * sensor);


/** \fn int create_new_regex (frame_t * f, char * sensor)
 * \brief Create a new regex.
 *
 * This function allows the user to create a new regex
 * @param[in] f the frame 
 * @param[in] sensor the sensor of log_line
 */
int create_new_regex (frame_t * f, char * sensor);


/** \fn int create_new_value (frame_t * f, char * sensor, int regex_number)
 * \brief Create a new value.
 *
 * This function allows the user to create a new value for a regex
 * @param[in] f the frame 
 * @param[in] sensor the sensor of log_line
 * @param[in] regex_number the number of the sensor for which the value will be added
 */
int create_new_value (frame_t * f, char * sensor, int regex_number);


/** \fn int display_file (char * path, char * sensor, int rule);
 * \brief Display a file
 *
 * This function displays the configuration  file whose path is given as 
 * a parameter in a terminal
 * @param[in] path the path to the log file
 * @param[in] sensor if not NULL, only the lines concerning this sensor will be displayed
 * @param[in] rule if not 0, only the lines concerning this rule will be displayed
 */
int display_file (char * path, char * sensor, int rule);


/** \fn int * find_activity (char * sensor, char * log_line, int display_activity)
 * \brief Find a log activity
 *
 * This function looks for the activity for a log.
 * @param[in] sensor the sensor of log_line
 * @param[in] log_line the log used to extract the activity
 * @param[in] display_activity if ==1, display
 */
int * find_activity (char * sensor, char * log_line, int display_activity);


/** \fn int modify_activity (frame_t * f);
 * \brief Modify an activity.
 *
 * This function allows the user to modify an activity
 * @param[in] f the frame 
 */
int modify_activity (frame_t * f);


/** \fn int rewrite_file (frame_t * f, char * path, int activity_number, int value_number, int delta);
 * \brief Rewrite a file.
 *
 * This function rewrites the activity file when a change was done
 * @param[in] f the frame 
 * @param[in] path the path to the file to rewrite
 * @param[in] activity_number number of the activity to modify
 * @param[in] value_number the new value to add to the activity 
 * @param[in] delta the time period associated to the value
 */
int rewrite_file (frame_t * f, char * path, int activity_number, int value_number, int delta);


/** \fn int create_new_sequence (frame_t * f, log_struct ** logs, int log_index)
 * \brief Create a new sequence.
 *
 * This function allows the user to create a new sequence of activities
 * @param[in] f the frame 
 * @param[in] logs the struct with the displayed logs
 * @param[in] log_index the number of displayed logs
 */
int create_new_sequence (frame_t * f, log_struct ** logs, int log_index);


/** \fn int extract_last_activity (int sequences_number)
 * \brief Extract last activity from a sequence.
 *
 * This function returns the last activity in a sequence. 
 * If the sequence ends with a sub-sequence, it returns the last activity
 * from the sub-sequence
 * @param[in] sequence_number the number of the sequence to read
 */
int extract_last_activity (int sequences_number);


/** \fn int extract_first_activity (int sequences_number)
 * \brief Extract first activity from a sequence.
 *
 * This function returns the first activity in a sequence. 
 * If the sequence begins with a sub-sequence, it returns the first activity
 * from the sub-sequence
 * @param[in] sequence_number the number of the sequence to read
 */
int extract_first_activity (int sub_sequence_number);


/** \fn int extract_first_delta_from_sequence (int sequence_number);
 * \brief Extract first delta from a sequence.
 *
 * This function returns the first delta found in a sequence. 
 * If the sequence begins with a sub-sequence, it returns the first delta
 * of the sub-sequence
 * @param[in] sequence_number the number of the sequence to read
 */
int extract_first_delta_from_sequence (int sequence_number);


/** \fn int create_delta_for_sequence (frame_t * f, log_struct ** logs, int log_index, int sequence_nb)
 * \brief Generate deltas and criticality for a new sequence.
 *
 * This function generates the time intervals between two activities used 
 * in a sequence, using the logs displayed, as well as a default criticality
 * based on sub-sequences criticality. 
 * @param[in] f the frame 
 * @param[in] logs the struct with the displayed logs
 * @param[in] log_index the number of displayed logs
 * @param[in] sequence_nb the number of the sequence
 */
int create_delta_for_sequence (frame_t * f, log_struct ** logs, int log_index, int sequence_nb);


/** \fn int sequence_find_delta (FILE * file_tmp, char * string, log_struct ** logs, int log_index)
 * \brief Find delta for a couple of activities
 *
 * This function allows thes user to chose a delta between several possibilities,
 * based on the logs displayed
 * @param[in] file_tmp a file to stock the sequence
 * @param[in] string the activities string
 * @param[in] logs the struct with the displayed logs
 * @param[in] log_index the number of displayed logs
 */
int sequence_find_delta (frame_t * f, FILE * file_tmp, char * string, log_struct ** logs, int log_index);


/** \fn int get_sequence_criticality (int sub_sequence_nb)
 * \brief Extract criticality from a sequence
 *
 * This function returns the criticality of a sequence. 
 * @param[in] sub_sequence_number the number of the sequence 
 */
int get_sequence_criticality (int sub_sequence_nb);


/** \fn int write_intra_activity_rules_file (frame_t * f, char * path, int index, char * pattern, int delta);
 * \brief Rewrite intra_activity and inter_activity files.
 *
 * This function rewrites the intra and inter activity files when
 * an existing rule is modified. 
 * @param[in] f the frame 
 * @param[in] path the path to the file to rewrite
 * @param[in] index the number of the rule (is 0 when writing intra-activity rules)
 * @param[in] pattern the pattern looked for in the file to determine if a rule already exists
 * @param[in] delta the time period associated to the value
 */
int write_intra_activity_rules_file (frame_t * f, char * path, int index, char * pattern, int delta);


/** \fn char * replace_by_subsequence (frame_t * f, char * new_sequence);
 * \brief Look for sub-sequences.
 *
 * When a new sequence is created, this function go through the sequences configuration
 * file to find any exiting sub-sequence. If one is found, it is replace in the new sequence. 
 * @param[in] f the frame 
 * @param[in] new_sequence the sequence created by the user
 */
char * replace_by_subsequence (frame_t * f, char * new_sequence);


/** \fn char * remove_deltas_from_sequence (char * sequence);
 * \brief Remove all deltas in a sequence.
 *
 * This function clean a sequence from all deltas it contains.
 * @param[in] sequence the sequence to clean
 */
char * remove_deltas_from_sequence (char * sequence);


/** \fn char * is_sensor_activated (frame_t * f, char * sensor);
 * \brief Detect sensors.
 *
 * This function tells if a sensor has at least one regex in the regex file
 * @param[in] f the frame 
 * @param[in] sensor the sensor to test
 */
char * is_sensor_activated (frame_t * f, char * sensor);

#endif
#ifdef __cplusplus
}
#endif
