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
 * \file init_functions.c
 * \brief Correlation plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 *
 * This file contains the source code for the SELinux plugin.
 */
#define _GNU_SOURCE
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "constants.h"
#include "frame-common.h"
#include "frame-utilities.h"
#include "machine.h"
#include "plugin.h"
#include "time-period.h"
#include "correlation.h"
#include "menu_functions.h"
#include "alerts_map.h"
#include "criticality.h"
#include "editor.h"
#include "init_functions.h"


int init_activity_list(frame_t *f, cairo_t *cr, int machine_index, char * machine_name)
{
	data_t 		* data 				= (data_t *) f->priv;
	data->refreshing_is_allowed = 0;

	char		* path 				= NULL;
	char 		string[256];
	int 		i 				= 0;	
	GSList 		* tmp 				= NULL;
	
	char 		* error_path			= frame_get_absolute_data_path (f, "error.log");

	FILE 		* error_log_file 		= fopen (error_path, "w+");
	if (error_log_file == NULL) { 
		perror("Error openning the error log file\n");
		return -1;
	}

	// Init sensors list and colors array in f->priv
	init_sensors_list (f);
	data->Number_Of_Activities = count_configuration_rules (f, ACTIVITY_FILE_PATH);
	init_color_array (f); 

	// Init the activities list in f->priv
	data->activity_list = malloc (data->Number_Of_Activities * sizeof(GList *));
	for (i=0 ; i < data->Number_Of_Activities ; i++) 
		data->activity_list[i] = NULL;

	char 		* sensor 	= NULL;
	int 		k 		= 0;
	int 		osiris_file_nb 	= 1;
	int 		sensor_nb 	= 0;
	
	tmp = g_slist_copy (data->Sensors_List);

	criticality_struct * criticality;
	criticality = malloc(sizeof(criticality_struct));
	criticality->criticality_time = malloc(sizeof(struct tm));
	
	char * machine_folder = NULL;
	if (machine_name == NULL)
		machine_folder = (char*) machine_get_folder_name (f->display_machine);
	else
		machine_folder = machine_name;
			
	path = g_strdup_printf ("%s/%s/%s/correlation/report", MACHINES_DIR, machine_folder, REPORTS_DIR);
	FILE * report_file = fopen(path,"w+");
	if (report_file == NULL) {
		perror("error report file\n");
		exit(1);
	}

	// For each sensor
	for (k = 0 ; k < data->Number_Of_Sensors ; k++) {
		sensor = g_strdup (tmp->data);
		sensor_nb = g_slist_index (data->Sensors_List, tmp->data);
		// Get the log file name
		if (g_strcmp0 (sensor, OSIRIS) == 0) {
			path = g_strdup_printf ("./%s/%s/%s/%s/%i", MACHINES_DIR
							, machine_folder
							, LOGS_DIR
							, sensor
							, osiris_file_nb);
			osiris_file_nb++;
			k--;
		}
		else if (g_strcmp0 (sensor, SELINUX) == 0) {
			path = g_strdup_printf ("./%s/%s/%s/%s/audit.log", MACHINES_DIR
								, machine_folder
								, LOGS_DIR
								, sensor);
			tmp = g_slist_next(tmp);
		}
		else if (g_strcmp0 (sensor, SNORT) == 0) {
			path = g_strdup_printf ( "./%s/%s/%s/%s/alert", MACHINES_DIR
							, machine_folder
							, LOGS_DIR
							, sensor);
			tmp = g_slist_next(tmp);
		}
		else if (g_strcmp0 (sensor, SYSLOG) == 0) {
			path = g_strdup_printf ("./%s/%s/%s/syslog_ssh/auth.log", MACHINES_DIR
									, machine_folder
									, LOGS_DIR);
			tmp = g_slist_next(tmp);
		}
		else {
			path = g_strdup_printf ("sensor_not_implemented");
			tmp = g_slist_next(tmp);
		}


		if (g_strcmp0 (path, "sensor_not_implemented") != 0) {
			// Open the log file 
			FILE * log_file = fopen(path,"r");
			
			if (log_file == NULL) {
				if (g_strcmp0 (sensor, OSIRIS) == 0) {
					k++;
					tmp = g_slist_next(tmp);
				}
				else {
					printf("%s\n", path);
					perror("1 error log file");
				}
			}
			else {
				criticality->criticality_time->tm_mday = 1; 
				criticality->criticality_time->tm_mon = 1; 
				criticality->criticality_time->tm_year = 1; 
				criticality->criticality_time->tm_hour = 1; 
				criticality->criticality_time->tm_min = 1; 
				criticality->criticality_time->tm_sec = 1;

				g_free (path);
				char log_line[256];
				int ret = 0;
				// For each log 
				while (read_criticality_get_log(sensor, log_line, log_file) != NULL) {
					criticality->value = -1;
					// Find the log criticality and its value rule

					ret = find_pattern (sensor, log_line, criticality, report_file, error_log_file );	

					if (ret == 0) {
						if (criticality->criticality_time->tm_year > 1900) {
							criticality->criticality_time->tm_year = criticality->criticality_time->tm_year - 1900;
							criticality->criticality_time->tm_mon = criticality->criticality_time->tm_mon - 1;
						}
						if (criticality->value_rule_number != 0) {
							// Look for the log activity
							time_t log_time = mktime(criticality->criticality_time);
							read_activity (f, cr
									, criticality->value_rule_number
									, log_time
									, sensor);
						}
					}		
				}

				data->file_end_position[sensor_nb] = ftell (log_file);
				fclose(log_file);
			}
		}
	}

	GList * act_list = NULL;

	// Fill the debugging file with found activities
	for (i=0 ; i < data->Number_Of_Activities ; i++) {
		data->activity_list[i] = g_list_sort(data->activity_list[i], (GCompareFunc)sort_by_beginning_date);
		fprintf (error_log_file, "\nACTIVITY %i\n", i+1);
		act_list = g_list_copy(data->activity_list[i]);
		while ( act_list != NULL ) {
			fprintf (error_log_file, "Sensor #%i: from %i to %i\n", ((Activity_t *)(act_list->data))->sensor
										, (int) (((Activity_t *)(act_list->data))->beginning_date)
										, (int) (((Activity_t *)(act_list->data))->end_date));
			
			if ( machine_is_all_machines (f->display_machine)) { 
				// If All Machines mode is on, count the number of activities for one machine
				//TODO Count for the selected time period
			//	time_t current_time = time(NULL);
			//	current_time -= data->time_period_offset * correlation_get_period (HOUR);
			//	time_t beginning_time = current_time - correlation_get_period (f->display_period.type);
			//	if (((Activity_t *)(act_list->data))->beginning_date > beginning_time 
			//				&& ((Activity_t *)(act_list->data))->end_date < current_time) 
			//	{
					data->activity_number[machine_index]++;
			//	}
			}

			act_list = g_list_next (act_list);
		}
	}

	g_free (sensor);
	g_free (criticality);
	fclose (report_file);	
	fclose (error_log_file);

	// Init sequences list in f->priv
	data->Number_Of_Sequences = count_configuration_rules (f, SEQUENCES_FILE);
	data->sequences_list = malloc (data->Number_Of_Sequences * sizeof (GList *));
	for (i=0 ; i < data->Number_Of_Sequences ; i++) {
		data->sequences_list[i] = NULL;
	}
	init_sequences_list(f, cr, machine_index);

	// Fill the debugging file with found sequences
	error_log_file = fopen (error_path, "a+");
	if (error_log_file == NULL) { 
		perror("Error openning the error log file\n");
		return -1;
	}
	fprintf(error_log_file, "\nSEQUENCES\n");
	GList * tmp2 = NULL;
	GList * tmp3 = NULL;
	for (i = 0 ; i < data->Number_Of_Sequences ; i++) {
	
		tmp2 = g_list_copy (data->sequences_list[i]);
		while (tmp2 != NULL) {
			tmp3 = g_list_copy (tmp2->data);
			while (tmp3 != NULL) {
				fprintf(error_log_file, "sequence #%i (index %i): type=%i number=%i index%i\n"
								, i+1
								, g_list_index (data->sequences_list[i], tmp2->data)
								, ((Sequence_t*) (tmp3->data) )->sub_sequence_type
								, ((Sequence_t*) (tmp3->data) )->sub_sequence_number
								, ((Sequence_t*) (tmp3->data) )->sub_sequence_index);
				tmp3 = g_list_next (tmp3);
			}
			// If All Machines mode is on, count the number of sequences for one machine
			if ( machine_is_all_machines (f->display_machine)) {
					data->sequence_number[machine_index]++;
			}
			
			tmp2 = g_list_next (tmp2);
		}
	}
	
	fclose(error_log_file);
	g_free (error_path);
	data->refreshing_is_allowed = 1;

	return 0;
}


int init_sequences_list(frame_t *f, cairo_t *cr, int machine_index)
{
	data_t 		* data 			= (data_t *) f->priv;

	char 		* error_path		= frame_get_absolute_data_path (f, "error.log");
	FILE 		* error_log_file	= fopen (error_path, "a+");
	if (error_log_file == NULL) { 
		perror("Error openning the error log file\n");
		return -1;
	}
	g_free (error_path);

	char 		* path 			= g_strdup (SEQUENCES_FILE);
	FILE 		* file 			= fopen (path, "r");
	g_free (path);
	if (file == NULL) { 
		perror("Error openning the activity type file\n");
		return -1;
	}

	size_t 		size 			= 256;
	char 		* line 			= NULL;
	int 		j 			= 0;
	int 		sequence_number 	= 0;
	int 		ret 			= 0; 
	
	// Read each sequence in the sequences file
	while ( (ret = getline (&line, &size, file)) != -1) {
		char 	** sequence_array_tmp 	= g_strsplit (line, "\"", 256);
		char 	** sequence_array 	= g_strsplit (sequence_array_tmp[2], " ", 256);
		
		fprintf (error_log_file, "\nSEQUENCE: %s", sequence_array_tmp[2]);
		j = 1;
		int 	stop 			= 0;
		g_strfreev (sequence_array_tmp);
		
		// For each sequence, look for every occurence
		while (sequence_array[j] != NULL && g_strcmp0 (sequence_array[j], "") != 0 
						 && g_strstr_len (sequence_array[j], -1, "(") == NULL 
						 && stop ==0) {
			detect_sequences (f, error_log_file, sequence_number, sequence_array[j], j); 
			if (data->sequences_list[sequence_number] == NULL)
				stop = 1;
			j++;
		}

		if (machine_is_all_machines (f->display_machine) && stop == 0) {
			// If All Machines mode is on, update the sequence criticality 
			int sequence_criticality = 0;
			sscanf (sequence_array[j], "(%i)", &sequence_criticality);
			data->sequence_criticality[machine_index] += g_list_length(data->sequences_list[sequence_number]) * sequence_criticality;
		}
		
		g_strfreev (sequence_array);
		sequence_number++;
	}

	g_free (line);
	fclose (file);
	fclose (error_log_file);

	return sequence_number;
}


int refresh_activity_list(frame_t *f, cairo_t *cr)
{
	data_t 			* data 		= (data_t *) f->priv;

	char 			* path 		= NULL;
	char 			* sensor	= NULL;
	int 			k 		= 0;
	int 			osiris_file_nb 	= data->osiris_file_nb;
	int 			sensor_nb 	= 0;
	GSList 			* tmp 		= g_slist_copy (data->Sensors_List);

	criticality_struct 	* criticality 	= malloc(sizeof(criticality_struct));
	criticality->criticality_time 		= malloc(sizeof(struct tm));

	path = g_strdup_printf ("%s/%s/%s/correlation/report", MACHINES_DIR, machine_get_folder_name (f->display_machine), REPORTS_DIR);
	FILE * report_file = fopen(path,"a+");
	if (report_file == NULL) {
		perror("error report file\n");
		exit(1);
	}

	// For each sensor
	for (k = 0 ; k < data->Number_Of_Sensors ; k++) {
		// Get the log file path
		sensor = g_strdup(tmp->data);
		sensor_nb = g_slist_index (data->Sensors_List, tmp->data);
		
		if (g_strcmp0 (sensor, OSIRIS) == 0) {
			path = g_strdup_printf ("./%s/%s/%s/%s/%i", MACHINES_DIR
							, machine_get_folder_name (f->display_machine)
							, LOGS_DIR
							, sensor
							, osiris_file_nb);
			osiris_file_nb++;
			k--;
		}
		else if (g_strcmp0 (sensor, SELINUX) == 0) {
			path = g_strdup_printf ("./%s/%s/%s/%s/audit.log", MACHINES_DIR
								, machine_get_folder_name (f->display_machine)
								, LOGS_DIR
								, sensor);
			tmp = g_slist_next(tmp);
		}
		else if (g_strcmp0 (sensor, SNORT) == 0) {
			path = g_strdup_printf ( "./%s/%s/%s/%s/alert", MACHINES_DIR
							, machine_get_folder_name (f->display_machine)
							, LOGS_DIR
							, sensor);
			tmp = g_slist_next(tmp);
		}
		else if (g_strcmp0 (sensor, SYSLOG) == 0) {
			path = g_strdup_printf ("./%s/%s/%s/syslog_ssh/auth.log", MACHINES_DIR
									, machine_get_folder_name (f->display_machine)
									, LOGS_DIR);
			tmp = g_slist_next(tmp);
		}
		else {
			return 0;		
		}


		// Open the log file 
		FILE * log_file = fopen(path,"r");
		if (log_file == NULL) {
			if (g_strcmp0 (sensor, OSIRIS) == 0) {
				k++;
				tmp = g_slist_next(tmp);
			}
			else {
				printf("%s\n", path);
				perror("error log file");
			}
			g_free (path);
		}
		else {
			fseek (log_file, data->file_end_position[sensor_nb], SEEK_SET);
			
			criticality->criticality_time->tm_mday = 1; 
			criticality->criticality_time->tm_mon = 1; 
			criticality->criticality_time->tm_year = 1; 
			criticality->criticality_time->tm_hour = 1; 
			criticality->criticality_time->tm_min = 1; 
			criticality->criticality_time->tm_sec = 1;

			char log_line[256];
			int ret = 0;
			while (read_criticality_get_log(sensor, log_line, log_file) != NULL) {
				criticality->value = -1;
				// If there is a new log, handle it
				ret = find_pattern (sensor, log_line, criticality, report_file, NULL );	

				if (ret == 0) {
					if (criticality->criticality_time->tm_year > 1900) {
						criticality->criticality_time->tm_year = criticality->criticality_time->tm_year - 1900;
						criticality->criticality_time->tm_mon = criticality->criticality_time->tm_mon - 1;
					}
					if (criticality->value_rule_number != 0) {
						time_t log_time = mktime(criticality->criticality_time);
						read_activity (f, cr
								, criticality->value_rule_number
								, log_time
								, sensor);
					}
				}		
			}

			data->file_end_position[sensor_nb] = ftell (log_file);
			fclose(log_file);
		}

	}
	g_free (criticality);
	fclose (report_file);	
	g_free (sensor);

	return 0;
}


int free_activities_list (frame_t * f)
{
	data_t 		*data 		= (data_t *) f->priv;
	
	int 		i 		= 0;
	GList 		* tmp 		= NULL; 


	// Free data->activity_list
	for (i = 0 ; i < data->Number_Of_Activities ; i++) {
		tmp = g_list_copy(data->activity_list[i]);
		int j =1;
		while ( tmp != NULL ) {
			g_free ( (Activity_t *)(tmp->data) );
			tmp = g_list_next(tmp);
			j++;
		}
		g_list_free (data->activity_list[i]);
	}
	g_list_free (tmp);
	g_free (data->activity_list);

	// Free data->colors_array
	for (i = 0 ; i < data->Number_Of_Activities ; i++) {
		g_free (data->colors_array[i]);
	}
	g_free (data->colors_array);

	// Free data->sequences_list
	GList * tmp3 = NULL;
	for (i = 0 ; i < data->Number_Of_Sequences ; i++) {
		tmp = g_list_copy (data->sequences_list[i]);
		while (tmp != NULL) {
			tmp3 = g_list_copy (tmp->data);
			while (tmp3 != NULL) {
				g_free (tmp3->data);
				tmp3 = g_list_next (tmp3);
			}
			g_list_free (tmp3);
			tmp = g_list_next (tmp);
		}
		g_list_free (data->sequences_list[i]);
	}
	g_list_free (tmp);
	g_free (data->sequences_list);	

	g_free (data->file_end_position);
	g_free (data->activity_name_array);

	return 0;
}


int init_all_machines(frame_t *f, cairo_t *cr)
{
	data_t 		*data 		= (data_t *) f->priv;

	int 		i 		= 0;
	GList 		* list 		= machine_get_list_for_plugin (f->type, FALSE);

	data->activity_number = malloc ( (g_list_length (list)) * sizeof (int) );
	data->sequence_number = malloc ( (g_list_length (list)) * sizeof (int) );
	data->sequence_criticality = malloc ( (g_list_length (list)) * sizeof (int) );
	for (i = 0 ; i < g_list_length (list) ; i++) {
		data->activity_number[i] = 0;
		data->sequence_number[i] = 0;
		data->sequence_criticality[i] = 0;
	}

	i = 0;
	if (list) {
		GList *iter = list;
		// Call init_activity_list for every machine
		do {
			if (!machine_is_all_machines (iter->data)) {
				init_activity_list (f, NULL, i, (char*)machine_get_folder_name (iter->data));
			}
			i++;
		} while ( (iter = g_list_next (iter)) != NULL );
	}

	return 0;
}


GList * detect_sequences (frame_t * f, FILE * error_log_file, int sequence_nb, char * sequence, int sub_sequence_number) 
{
	data_t 		* data 		= (data_t *) f->priv;

	GList 		* next 		= NULL;
	int 		previous_act 	= 0;
	int 		next_act	= 0;
	int 		delta 		= 0;
	int 		sequence_number = 0;
	
	char 		** activities_array_tmp = NULL;
	GList 		* next2 		= NULL;
	int i = 0;

	fprintf (error_log_file, "Sub-sequence : %s\n", sequence);

	if (sub_sequence_number == 1) { 
		// If it is the first sub-sequence in this sequence
		if (g_strstr_len (sequence, -1, ":") != NULL) {
			// If the first sub-sequence is an activities pair
			activities_array_tmp = g_strsplit (sequence, ":", 3);
			previous_act = atoi (activities_array_tmp[0]) - 1;
			next_act = atoi (activities_array_tmp[2]) - 1;
			next = g_list_copy (data->activity_list[next_act]);
			next2 = g_list_copy (data->activity_list[next_act]);
			delta = atoi (activities_array_tmp[1]);

			// Init list with the first activity
			GList * tmp = g_list_copy (data->activity_list[previous_act]);
			GList * tmp2 = g_list_copy (data->sequences_list[sequence_nb]);
			
			i = 0;
			while (tmp != NULL) {
				GList * sequence_occurence = NULL;
				Sequence_t * sequence_data = malloc (sizeof (Sequence_t));
				sequence_data->sub_sequence_type = 0;
				sequence_data->sub_sequence_number = atoi (activities_array_tmp[0]) - 1;
				sequence_data->sub_sequence_index = i;
				sequence_occurence = g_list_prepend (sequence_occurence, sequence_data);
			
				tmp2 = g_list_prepend (tmp2, sequence_occurence);
				i++;

				tmp = g_list_next (tmp);
			}
			data->sequences_list[sequence_nb] = g_list_reverse (tmp2);

		}
		else {
			// If the first sub-sequence is another sequence
			sequence_number = atoi (sequence) - 1;
			GList * tmp2 = g_list_copy (data->sequences_list[sequence_nb]);
			i = 0;
			
			// Init list with the said sequence
			GList * tmp = g_list_copy (data->sequences_list[sequence_number]);
			while (tmp != NULL) {
				GList * sequence_occurence = NULL;
				Sequence_t * sequence_data = malloc (sizeof (Sequence_t));
				sequence_data->sub_sequence_type = 1;
				sequence_data->sub_sequence_number = sequence_number;
				sequence_data->sub_sequence_index = i;
				sequence_occurence = g_list_prepend (sequence_occurence, sequence_data);
				tmp2 = g_list_prepend (tmp2, sequence_occurence);
				i++;
				tmp = g_list_next (tmp);
			}
			data->sequences_list[sequence_nb] = g_list_reverse (tmp2);
			
			return NULL;
		}
	}
	else { 
		// If it isn't the first sub-sequence in this sequence
		if (g_strstr_len (sequence, -1, ":") != NULL) {
			// If the sub-sequence is an activities pair
			activities_array_tmp = g_strsplit (sequence, ":", 3);	
			next_act = atoi(activities_array_tmp[2]) - 1;
			next2 = g_list_copy (data->activity_list[next_act]);
			delta = atoi (activities_array_tmp[1]);
		}
		else {
			// If the sub-sequence is another sequence
			sequence_number = atoi (sequence) - 1;
			next2 = g_list_copy (data->sequences_list[sequence_number]);
			delta = extract_first_delta_from_sequence (sequence_number);
		}	
	}
	
	GList 	* previous 	= g_list_copy (data->sequences_list[sequence_nb]);
	int 	continue_flag 	= 1;
	int 	sequence_end 	= 0;

	while (sequence_end == 0 && previous != NULL) {
		continue_flag = 1;
	
		while ( (next2 != NULL) && (continue_flag == 1) ) {
			int diff = 0;
			if (g_strstr_len (sequence, -1, ":") != NULL) {
				// If sub-sequence is an activities pair
				// Extract activities dates to find the time difference
				GList 		* sub_sequence 	= g_list_copy (previous->data);
				Sequence_t 	* sequence_info = ((Sequence_t *)(sub_sequence->data));
				GList 		* tmp 		= g_list_nth (data->activity_list[sequence_info->sub_sequence_number]
											, sequence_info->sub_sequence_index);
				int	 	end_date 	= ((Activity_t *) (tmp->data))->end_date;

				diff = ((Activity_t *)(next2->data))->beginning_date - end_date ;

			}
			else {
				// If sub-sequence is another sequence
				// Extract sequence date to find the time difference

				// Get end date of previous sub-sequence
				GList * sub_sequence = g_list_copy (previous->data);
				Sequence_t * sequence_info = ((Sequence_t *)(sub_sequence->data));
				while (sequence_info->sub_sequence_type == 1) { 
					// While sub-sequence is a sequence, search date in its own sub-sequence
					sub_sequence = g_list_nth (data->sequences_list[sequence_info->sub_sequence_number]
									, sequence_info->sub_sequence_index);
					sub_sequence = g_list_copy (sub_sequence->data);
					sequence_info = ((Sequence_t *)(sub_sequence->data));
				}
				GList * tmp = g_list_nth ( data->activity_list[sequence_info->sub_sequence_number]
								, sequence_info->sub_sequence_index );
				int end_date = ((Activity_t *) (tmp->data))->end_date;

				// Get beginning date of next sub-sequence
				sub_sequence = g_list_last (next2->data);
				sequence_info = ((Sequence_t *)(sub_sequence->data));
				while (sequence_info->sub_sequence_type == 1) { 
					// While sub-sequence is a sequence, search date in its own sub-sequence
					sub_sequence = g_list_nth (data->sequences_list[sequence_info->sub_sequence_number]
									, sequence_info->sub_sequence_index);
					sub_sequence = g_list_last (sub_sequence->data);
					sequence_info = ((Sequence_t *)(sub_sequence->data));
				}
				tmp = g_list_nth ( data->activity_list[sequence_info->sub_sequence_number], sequence_info->sub_sequence_index );
				int beginning_date = ((Activity_t *) (tmp->data))->beginning_date;

				diff = beginning_date - end_date ;
			}

			if ( diff <= 0) {
				next2 = g_list_next (next2);
			}
			else if ( diff <= delta) {
				// If delta is bigger than the time difference, the log is a part of the sequence occurence. 
				// The sequence list in f->priv has to be filled
				if (g_strstr_len (sequence, -1, ":") != NULL) {
					Sequence_t * sequence_data = malloc (sizeof (Sequence_t));
					sequence_data->sub_sequence_type = 0;
					sequence_data->sub_sequence_number = atoi (activities_array_tmp[2]) - 1;
					sequence_data->sub_sequence_index = g_list_index (data->activity_list[next_act], next2->data);
					previous->data = g_list_prepend (previous->data, sequence_data);
				}
				else {
					Sequence_t * sequence_data = malloc (sizeof (Sequence_t));
					sequence_data->sub_sequence_type = 1;
					sequence_data->sub_sequence_number = sequence_number;
					sequence_data->sub_sequence_index = g_list_index (data->sequences_list[sequence_number], next2->data);
					previous->data = g_list_prepend (previous->data, sequence_data);
				}	
					
				next2 = g_list_next (next2);
				
				if (g_list_next (previous) == NULL)
					// If all sequences beginning for this sequence has been tested, step out of the loop
					sequence_end = 1;
				else {
					// Else, look for the next item in this sequence list
					previous = g_list_next (previous);
				}
				continue_flag = 0;
			}
			else {
				// If the time difference is bigger than delta, next occurence in the sequence list must be tested
				// and the actual one must be erase from the sequence list
				if (g_list_next (previous) == NULL)
					sequence_end = 1;
				else {
					previous = g_list_remove (previous, previous->data);
				}
				continue_flag = 0;
			}
		}

		if (next2 == NULL) {
			// If there is no more item in the next sub-sequence list
			// Delete elements from the actual sequence list who were not tested, since they cannot be part of a sequence occurence
			GList * list_to_delete = g_list_copy (previous);
			previous = g_list_first (previous);
			while (list_to_delete != NULL) {
				previous = g_list_remove (previous, list_to_delete->data);
				list_to_delete = g_list_next (list_to_delete);
			}
			sequence_end = 1;
		}
		else if (sequence_end == 1) {
			// If sequence end was reached
			// Delete elements from the actual sequence list who were not tested, since they cannot be part of a sequence occurence
			GList * list_to_delete = g_list_previous (previous);
			previous = g_list_first (previous);
			while (list_to_delete != NULL) {
				list_to_delete = g_list_remove (previous, list_to_delete->data);
				list_to_delete = g_list_next (list_to_delete);
			}
			sequence_end = 1;
		}
		else {
			if (previous == NULL || g_list_next (previous) == NULL) {
				sequence_end = 1;
			}
		}
	}

	previous = g_list_first (previous);	
	data->sequences_list[sequence_nb] = g_list_copy (previous);
	
	g_strfreev(activities_array_tmp);

	return NULL;
}



int init_color_array (frame_t * f) 
{
	data_t 		* data 			= (data_t *) f->priv;
	
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;
	char 		pattern[256];
	char 		string[256];
	
	char		activity_number[256];
	char		activity_name[256];
	char		color[256];
	int 		activity 		= 0;
	int 		i 			= 0;	
	char		* path			= g_strdup (ACTIVITY_FILE_PATH);
	FILE 		* activity_type_file 	= fopen (path, "r");
	free (path);

	// Init colors array
	data->colors_array = malloc ( (data->Number_Of_Activities + 1) * sizeof(char *));
	data->activity_name_array = malloc(data->Number_Of_Activities * sizeof(char *));
	for (i=0 ; i < data->Number_Of_Activities ; i++) {
		data->colors_array[i] = g_strdup("black");
	}
	data->colors_array[data->Number_Of_Activities] = NULL;


	if (activity_type_file == NULL) { 
		perror("Error openning the activity type file\n");
		return -1;
	}

	sprintf (pattern, "([0-9]*) \"([^\"]*)\" ([^ ]*).* ");

	while(fgets (string, 256, activity_type_file) != NULL) {
		// For each activity
		re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
		if (re == NULL) {
			  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
			  return -1;
		}
		rc = pcre_exec(re, NULL, string, (int)strlen(string), 0, 0, ovector, OVECCOUNT);    
		if (rc > 0) {
			// Fill colors array with color given in activity file
			sprintf(activity_number, "%.*s", ovector[3] - ovector[2], string + ovector[2]);
			sprintf(activity_name, "%.*s", ovector[5] - ovector[4], string + ovector[4]);
			sprintf(color, "%.*s", ovector[7] - ovector[6], string + ovector[6]);
			
			activity = atoi(activity_number);
			data->colors_array[activity-1] = g_strdup (color);
			data->activity_name_array[activity-1] = g_strdup (activity_name);
		}
	}
	pcre_free (re);
	fclose (activity_type_file);

	return 0;
}



int init_sensors_list (frame_t * f) 
{
	data_t 		* data 			= (data_t *) f->priv;

	char 		* line 			= NULL;
	char 		sensor2[256];
	size_t 		size 			= 256;
	int 		regex 			= 0;
	int 		found_flag 		= 0;
	int 		j 			= 1;
	GSList 		* tmp 			= NULL;


	char 		* path			= frame_get_absolute_data_path (f, "error.log");
	FILE 		* error_log_file 	= fopen (path, "w+");
	if (error_log_file == NULL) { 
		perror("Error openning the error log file\n");
		return -1;
	}

	path 				= g_strdup (REGEX_FILE);
	FILE 		* file 		= fopen (path, "r");
	if (file == NULL) { 
		perror("Error openning the activity type file\n");
		return -1;
	}

	g_free (path);
	
	data->Sensors_List = NULL;
	data->Number_Of_Activities = 0;
	
	fprintf (error_log_file, "ACTIVATED SENSORS: \n");

	while (getline (&line, &size, file) != -1) {
		sscanf (line, "%i %s ::", &regex, sensor2);

		char 	* sensor_tmp 		= g_strdup (sensor2);
		tmp = g_slist_copy (data->Sensors_List);
		found_flag = 0;
		while (tmp != NULL && found_flag == 0) {
			if (g_strcmp0 (tmp->data, sensor2) == 0) 
				found_flag = 1;
			tmp = g_slist_next(tmp);
		}
		
		if (found_flag == 0) {
			data->Sensors_List = g_slist_prepend (data->Sensors_List, sensor_tmp);
			j++;
		}
	}
	data->Number_Of_Sensors = g_slist_length (data->Sensors_List);
	fclose (file);

	tmp = g_slist_copy (data->Sensors_List);
	for (j = 0; j < data->Number_Of_Sensors; j++) {
		fprintf (error_log_file, "#%i: %s \n", get_sensor_number(f, (char*)(tmp->data)), (char*)(tmp->data));
		tmp = g_slist_next (tmp);
	}
	
	data->file_end_position = malloc (data->Number_Of_Sensors * sizeof (long));

	fclose (error_log_file);
	
	return 0;
}


int count_configuration_rules (frame_t * f, char * path) 
{
	char 		string[256];
	int		number_of_lines 	= 0;
	FILE 		* file 			= fopen (path, "r");

	if (file == NULL) { 
		perror("Error openning the file\n");
		return -1;
	}
	while (fgets(string, 256, file) != NULL) {
		number_of_lines++;
	}
	fclose (file);

	return number_of_lines;
}



	
int read_activity (frame_t * f, cairo_t *cr, int value_rule_number, time_t log_time, char * sensor) 
{
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;

	char 		pattern[256];
	char 		string[256];
	
	char		activity_number[256];
	char		activity_name[256];
	char		color[256];
	char		delta[256];
	int 		activity = 0;
	int		delta_t = 0;
	
	char 		* path 					= g_strdup (ACTIVITY_FILE_PATH);
	FILE 		* activity_type_file 			= fopen (path, "r");
	g_free (path);
	if (activity_type_file == NULL) { 
		perror("Error openning the activity type file\n");
		return -1;
	}

	sprintf(color, "black");
	sprintf (pattern, "([0-9]*) \"([^\"]*)\" ([^ ]*).* %i:([0-9]*)", value_rule_number);

	// Look for an activity with rule #value_rule_number
	while (fgets (string, 256, activity_type_file) != NULL) {
		re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
		if (re == NULL) {
			  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
			  return -1;
		}
		rc = pcre_exec(re, NULL, string, (int)strlen(string), 0, 0, ovector, OVECCOUNT);    
		// If an activity is found, store the log in data->activity_list
		if (rc > 0) {
			sprintf(activity_number, "%.*s", ovector[3] - ovector[2], string + ovector[2]);
			sprintf(activity_name, "%.*s", ovector[5] - ovector[4], string + ovector[4]);
			sprintf(color, "%.*s", ovector[7] - ovector[6], string + ovector[6]);
			sprintf(delta, "%.*s", ovector[9] - ovector[8], string + ovector[8]);
			
			activity = atoi(activity_number);
			delta_t = atoi (delta);

			if (activity != 0) { 
				fill_activity_struct (f, log_time, delta_t, activity, sensor); 
			}
		}
	}

	pcre_free (re);
	fclose (activity_type_file);
	
	return 0;
}


int fill_activity_struct (frame_t * f, time_t log_date, int delta_t, int activity, char * sensor)
{
	data_t 			*data 			= (data_t *) f->priv; 

	int 			found_flag		= 0;
	int 			i 			= 0;
	int 			stop_flag 		= 0;
	int 			sensor_nb 		= get_sensor_number (f, sensor);

	// Look for an occurence of the activity in which the log can be added
	GList 			* tmp 			= g_list_last(data->activity_list[activity-1]); 
	while ( (tmp != NULL) && (found_flag == 0) && (stop_flag == 0)) {
		if (((Activity_t *)(tmp->data))->sensor == sensor_nb) {
			// If found, modify the occurence
			if ((log_date - ((Activity_t *)(tmp->data))->end_date < delta_t) ) {
				((Activity_t *)(tmp->data))->end_date = log_date;
				found_flag = 1;
			}
			stop_flag = 1;
		}
		i++;
		tmp = g_list_previous(tmp);
	}

	// If not found, a new occurence is created
	if (found_flag == 0) {
		Activity_t * activity_info = malloc (sizeof(Activity_t));
		activity_info->sensor = sensor_nb;
		activity_info->beginning_date = log_date;
		activity_info->end_date = log_date;
		activity_info->activity_links_number = 0;
		activity_info->sequence_links_number = 0;
		data->activity_list[activity-1] = g_list_append (data->activity_list[activity-1], activity_info); 
	}
	
	return 0;
}

