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
 * \file editor.c
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

#include <time.h>

#include "constants.h"
#include "frame-common.h"
#include "frame-utilities.h"
#include "machine.h"
#include "plugin.h"
#include "time-period.h"
#include "correlation.h"
#include "criticality.h"
#include "menu_functions.h"
#include "alerts_map.h"
#include "editor.h"
#include "text_mode.h"


int rules_editor (frame_t * f, int quit)
{
	data_t 				*data 		= (data_t *) f->priv;
	log_struct 			** logs 	= NULL;
	logs = malloc (512 * sizeof(log_struct*));
	int 				log_index 	= 0;
	long 				* log_pos 	= NULL;
	log_pos = malloc (data->Number_Of_Sensors * sizeof(long));

	int 				i 		= 0;
	
	for (i=0 ; i < data->Number_Of_Sensors; i++) 
		log_pos[i] = 0;

	if (quit == 0) {
		printf("\n\n*** Rules Editor ***\n\n");
		int file_to_edit = 0;
		file_to_edit = select_file();
		while (file_to_edit != 7) {
			file_edit (f, file_to_edit, logs, log_index);
			file_to_edit = select_file();
		}
	}

	printf("\n\n*** Quitting rules editor ***\n\n");

	g_free (log_pos);
	
	for (i = 0 ; i < log_index ; i++) {
		if (logs[i] != NULL) {
			g_free(logs[i]);
		}
	}
	g_free (logs);

	data->graph_type = g_strdup ("bars");
	refresh_content(f);

	return 0;
}

int select_file ()
{
	int file = 0;
	char * answer = NULL;
	size_t size = 256;
	// Print menu
	printf("\n\n*** Main Menu ***\n\n");
	printf("1: Create or modify regex matching logs\n");
	printf("2: Create or modify activity criticity\n");
	printf("3: Create or modify an activity\n");
	printf("4: Create or modify intra-activity links\n");
	printf("5: Create or modify inter-activity links\n");
	printf("6: Create or modify activities sequences\n");
	printf("7: Exit editor\n\n");
	
	do {
		printf("Enter your choice: ");
		getline (&answer, &size, stdin);
		file = strtol(answer, NULL, 10);
	} while (file <= 0 || file > 7);

	g_free (answer);
	
	return file;
}

int file_edit (frame_t * f, int file_to_edit, log_struct ** logs, int log_index) 
{
	switch (file_to_edit) {
		case 1:
			edit_regex_file(f, logs, log_index);
			break;
		case 2:
			edit_value_file(f, NULL, 0);
			break;
		case 3:
			edit_activity_file(f, logs, log_index);
			break;
		case 4:
			edit_intra_activity_file(f, logs, log_index);
			break;
		case 5:
			edit_inter_activity_file(f, logs, log_index);
			break;
		case 6:
			edit_sequences_file(f, logs, log_index);
			break;
		default:
			edit_regex_file(f, logs, log_index);
			break;
	}
	return 0;
}

int select_time_period()
{
	printf("Enter the period used to display logs (in seconds): ");

	char * answer = NULL;
	size_t size = 256;
	int delta = 0;

	do {
		printf("Enter delta to display logs (in seconds): ");
		getline (&answer, &size, stdin);
		delta = strtol(answer, NULL, 10);
	} while (delta == 0);
	
	g_free (answer);
		
	return delta;
}



int edit_regex_file(frame_t * f, log_struct ** logs, int log_index)
{
	size_t 		size 		= 256;
	size_t 		length 		= 0;
	char * 		answer 		= NULL;
	char * 		sensor 		= NULL;
	int 		regex_number 	= 0;
	
	display_file(REGEX_FILE, sensor, 0);
	
	// Select sensor for new regex
	do {
		printf("Enter the sensor for the regex you want to create: \n");
		length = getline (&sensor, &size, stdin);
	} while (length == -1);
	sensor = g_strndup (sensor, length-1);
	
	// Show existing regex
	do {
		printf ("Display regex file (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');

	if (answer[0] == 'y') { 
		display_file(REGEX_FILE, sensor, 0);
	}

	do {
		printf ("Create a new regex to match (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');
	
	// Create a new regex
	if (answer[0] == 'y') { 
		regex_number = create_new_regex (f, sensor);
	}
	else { //TODO modify a regex

	}

	// Create value(s) for the new regex
	do {		
		do {
			printf ("Create or modify a criticality value for this regex (y/n)? ");	
			getline (&answer, &size, stdin);
		} while (answer[0] != 'y' && answer[0] != 'n');
		
		if (answer[0] == 'y') { 
			edit_value_file(f, sensor, regex_number);
		}
	} while (answer[0] != 'n');
	
	g_free (answer);
	g_free (sensor);

	return 0;
}

int edit_value_file(frame_t * f, char * sensor, int regex_number)
{
	size_t 		size 	= 32;
	size_t 		length 	= 0;
	char * 		answer 	= NULL;

	if (sensor == NULL)
		getchar();

	// Select sensor
	while (sensor == NULL) {
		printf("Enter the sensor for the criticality value you want to create: ");
		length = getline (&sensor, &size, stdin);
		sensor = g_strndup (sensor, length-1);
	} 

	// If the regex wasn't given as a parameter, select a regex
	if (regex_number == 0) {
		int regex_max_number = 0;
		regex_max_number = display_file(REGEX_FILE, sensor, 0); 
		do {
			printf("Enter the number of the regex for which the value will be added: ");
			scanf ("%i", &regex_number);
		} while (regex_number <= 0 || regex_number > regex_max_number);
		getchar();
	}

	// Show values file
	do {
		printf ("Display values concerning this regex (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');

	if (answer[0] == 'y') { 
		display_file(VALUE_FILE, NULL, regex_number);
	}

	// Create the new value
	do {
		printf ("Create a new value for the regex (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');

	if (answer[0] == 'y') { 
		create_new_value (f, sensor, regex_number);
	}
	else { //TODO modify a value
	
	}
	
	g_free (answer);

	return 0;
}


int edit_activity_file(frame_t * f, log_struct ** logs, int log_index)
{
	int 		activity_number 	= 0;
	size_t		size 			= 256;
	char 		* answer 		= NULL;

	display_file (ACTIVITY_FILE_PATH, NULL, 0);

	do {
		printf ("Create a new activity (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');

	// Create a new activity
	if (answer[0] == 'y') { 
		char 	* sensor 		= NULL;
		do {
			printf("Enter the sensor for the activity you want to create: \n");
			getline (&sensor, &size, stdin);
			sensor = is_sensor_activated (f, sensor);
		} while (sensor == NULL);		
	
		activity_number = create_new_activity (f, sensor);
		g_free (sensor);
	}
	// Modify an old activity
	else {
		do {
			printf ("Modify a existing activity (y/n)? ");	
			getline (&answer, &size, stdin);
		} while (answer[0] != 'y' && answer[0] != 'n');

		if (answer[0] == 'y') { 
			activity_number = modify_activity (f);
		}
	}
	
	g_free (answer);
	
	return activity_number;
}


int edit_intra_activity_file(frame_t * f, log_struct ** logs, int log_index)
{
	int 		activity_max_number 	= 0;
	int 		activity_number 	= 0;
	int 		delta 			= 0;
	size_t 		size 			= 32;
	size_t 		length 			= 0;
	char 		* answer 		= NULL;
	char 		* pattern 		= NULL;
	char 		* sensor1 		= NULL;
	char 		* sensor2 		= NULL;

	// Create a new activity for the new rule or use an existing one ?
	do {
		printf ("Use an existing activity (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');

	if (answer[0] == 'y') { 
		// Select an activity
		activity_max_number = display_file(ACTIVITY_FILE_PATH, NULL, 0);
		do {
			printf("Enter the number of the activity you want to use: ");
			scanf ("%i", &activity_number);
		} while (activity_number <= 0 || activity_number > activity_max_number);
		getchar();
	}
	else {
		// Create an activity
		activity_number = edit_activity_file(f, logs, log_index);
	}

	display_file (INTRA_ACTIVITY_RULES_FILE, NULL, activity_number);

	// Select first sensor for the intra-activity link
	do {
		printf("Enter the first sensor for the intra-activity link you want to create: \n");
		length = getline (&sensor1, &size, stdin);
		printf("sen #%s#\n", sensor1);
		sensor1= is_sensor_activated (f, sensor1);
	} while (sensor1 == NULL);		

	// Select second sensor
	do {
		printf("Enter the second sensor for the intra-activity link you want to create (* to select all sensors): \n");
		length = getline (&sensor2, &size, stdin);
		if (g_strstr_len (sensor2, 1, "*") != NULL) {
			sensor2 = g_strndup (sensor2, length - 1);
		}
		else {
			sensor2= is_sensor_activated (f, sensor2);
		}
	} while (sensor2 == NULL);		

	// Select delta
	printf ("Delta intra-activity?\n");
	scanf ("%i", &delta);
	getchar(); 

	// Write new rule to file
	pattern = g_strdup_printf ("%i %s %s ", activity_number, sensor1, sensor2);	
	write_intra_activity_rules_file (f, INTRA_ACTIVITY_RULES_FILE, 0, pattern, abs(delta));

	g_free (answer);
	g_free (pattern);
	g_free (sensor1);
	g_free (sensor2);
	
	return 0;
}



int edit_inter_activity_file(frame_t * f, log_struct ** logs, int log_index)
{
	data_t *data = (data_t *) f->priv;

	int 		activity_max_number 	= 0;
	int 		first_activity_number 	= 0;
	int 		second_activity_number 	= 0;
	int 		delta 			= 0;
	int 		* act_nb 		= NULL;
	size_t 		size 			= 32;
	char 		* answer 		= NULL;

	do {
		printf ("Use the inter activities auto-creation tool (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');

	// Auto-creation tool for new inter-activities rules
	if (answer[0] == 'y') { 
		int 		log_number 	= 0;
		int 		i 		= 0;
		int 		delta 		= 0;
		long 		* log_pos = malloc (data->Number_Of_Sensors * sizeof(long));
		time_t 		time1		= 0;
		time_t 		time2		= 0;
		time_t 		first_log_date 	= 0;


		// Select the period to display logs (in seconds)
		for (i=0 ; i < data->Number_Of_Sensors; i++) 
			log_pos[i] = 0;
		delta = select_time_period();

		// Select the first date to read: either first log date, or the one chosen by the user
		first_log_date = find_first_date(f);
		do {
			printf ("Read logs from the beginning (y/n)? ");	
			getline (&answer, &size, stdin);
		} while (answer[0] != 'y' && answer[0] != 'n');
	
		if (answer[0] == 'n') { 
			int day = 0;
			int month = 0;
			int year = 0;
			printf("Enter a date to begin log display (jj/mm/yyyy): ");
			scanf ("%i/%i/%i", &day, &month, &year);
			first_log_date = get_user_date (day, month, year);
			getchar();
		}
		
		// Display logs	
		log_index = display_logs (f, 0, first_log_date, delta, logs, log_pos, 1);
		g_free (log_pos);

		if (log_index == 0) {
			printf ("There is no logs for this time period.\n");
			return 0;
		} 

		// Select the first log to use and find its activity
		do {
			printf("Select the first log to detect its activity: ");
			scanf ("%i", &log_number);
		} while (log_number >= log_index);
		act_nb = find_activity (logs[log_number]->sensor, logs[log_number]->log_line, 1);	
		first_activity_number = act_nb[0];
		time1 = get_date_from_log (logs[log_number]->sensor, logs[log_number]->log_line);
		
		// Select the second log to use and find its activity
		log_number = 0;
		do {
			printf("Select the second log to detect its activity: ");
			scanf ("%i", &log_number);
		} while (log_number >= log_index);
		act_nb = find_activity (logs[log_number]->sensor, logs[log_number]->log_line, 1);	
		second_activity_number = act_nb[0];
		time2 = get_date_from_log (logs[log_number]->sensor, logs[log_number]->log_line);

		g_free(act_nb);

		// If the two logs have the same activity, return to main menu
		if (first_activity_number == second_activity_number) {
			printf("The two logs have the same activity. No inter activities rule possible.\n");
			getchar();
			return 0;
		}

		// Set delta to the time difference between selected logs 
		delta = time2 - time1;
		if (delta < 0) {
			delta = -delta;
			int tmp = first_activity_number;
			first_activity_number = second_activity_number;
			second_activity_number = tmp;
		}
		printf("Actual delta between selected logs is %i seconds\n", delta);
	
		// Allows the user to change delta
		getchar();	
		do {
			printf ("Change delta (y/n)? ");	
			getline (&answer, &size, stdin);
		} while (answer[0] != 'y' && answer[0] != 'n');

		if (answer[0] == 'y') { 
			printf("Enter new delta: ");
			scanf ("%i", &delta);		
			getchar();
		}

	}
	// The user creates the new inter-activities rule without using the auto-creation tool
	else {
		// First activity handler
		printf("First Activity:\n");
		do {
			printf ("Use an existing activity (y/n)? ");	
			getline (&answer, &size, stdin);
		} while (answer[0] != 'y' && answer[0] != 'n');

		if (answer[0] == 'y') { 
			// Select an activity
			activity_max_number = display_file (ACTIVITY_FILE_PATH, NULL, 0);
			do {
				printf("Enter the number of the activity you want to use: ");
				scanf ("%i", &first_activity_number);
			} while (first_activity_number <= 0 || first_activity_number > activity_max_number);
			getchar();
		}
		else {
			// Create a new activity
			first_activity_number = edit_activity_file(f, logs, log_index);
		}

		// Second activity handler
		printf("Second Activity:\n");
		do {
			printf ("Use an existing activity (y/n)? ");	
			getline (&answer, &size, stdin);
		} while (answer[0] != 'y' && answer[0] != 'n');

		if (answer[0] == 'y') { 
			// Select an activity
			activity_max_number = display_file(ACTIVITY_FILE_PATH, NULL, 0);
			do {
				printf("Enter the number of the activity you want to use: ");
				scanf ("%i", &second_activity_number);
			} while (second_activity_number <= 0 || second_activity_number > activity_max_number);
			getchar();
		}
		else {
			// Create a new activity
			second_activity_number = edit_activity_file(f, logs, log_index);
		}

		// Enter delta
		delta = 0;
		printf ("Delta inter-activity?\n");
		scanf ("%i", &delta);
		getchar();
	}

	// Get the index of the new rule
	char * path = g_strdup (INTER_ACTIVITY_RULES_FILE);
	FILE * file = fopen (path, "a+");
	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning the inter-activities file");
		return -1;
	}
	g_free (path);
	
	char string[256];
	int inter_act_number = 0;
	while (fgets(string, 256, file) != NULL)
		inter_act_number++;

	// Write the new inter-activities rules to file
	char * pattern = g_strdup_printf (" %i %i ", first_activity_number, second_activity_number);	
	write_intra_activity_rules_file (f, INTER_ACTIVITY_RULES_FILE, ++inter_act_number, pattern, abs(delta));

	fclose (file);
	g_free (answer);
	g_free (pattern);
	
	return 0;
}


int edit_sequences_file(frame_t * f, log_struct ** logs, int log_index)
{
	int 		sequence_number		= 0;
	size_t 		size 			= 256;
	char 		* answer		= NULL;

	display_file (SEQUENCES_FILE, NULL, 0);
	do {
		printf ("Create or modify a sequence (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');

	// Create a new sequence
	if (answer[0] == 'y') { 
		sequence_number = create_new_sequence (f, logs, log_index);
	}

	g_free (answer);

	return 0;
}



time_t find_first_date(frame_t * f)
{
	data_t 		*data 		= (data_t *) f->priv;

	int 		k		= 0;
	char 		* path 		= NULL;
	char 		* sensor 	= NULL;
	FILE 		* log_file 	= NULL;
	GSList 		* tmp 		= NULL;
	time_t 		beginning_date 	= 0;

	tmp = g_slist_copy (data->Sensors_List);
	
	// For each sensor
	for (k = 0 ; k < data->Number_Of_Sensors ; k++) {
		// Select the log file
		sensor = g_strdup(tmp->data);
		if (g_strcmp0 (sensor, OSIRIS) == 0) {
			path = g_strdup_printf ("./%s/%s/%s/%s/%i", MACHINES_DIR
							, machine_get_folder_name (f->display_machine)
							, LOGS_DIR
							, sensor
							, 2);
		}
		else if (g_strcmp0 (sensor, SELINUX) == 0) {
			path = g_strdup_printf ("./%s/%s/%s/%s/audit.log", MACHINES_DIR
								, machine_get_folder_name (f->display_machine)
								, LOGS_DIR
								, sensor);
		}
		else if (g_strcmp0 (sensor, SNORT) == 0) {
			path = g_strdup_printf ( "./%s/%s/%s/%s/alert", MACHINES_DIR
							, machine_get_folder_name (f->display_machine)
							, LOGS_DIR
							, sensor);
		}
		else if (g_strcmp0 (sensor, SYSLOG) == 0) {
			path = g_strdup_printf ("./%s/%s/%s/syslog_ssh/auth.log", MACHINES_DIR
									, machine_get_folder_name (f->display_machine)
									, LOGS_DIR);
		}
		else {
			return 0;		
		}

		
		log_file = fopen(path,"r");
		g_free (path);

		char * log_line = NULL;
		if (log_file == NULL) {
			perror("error log file");
		}		
		else {
			// Get first log date and compare it to the current beginning date. Save the earlier date. 
			log_line = get_log(sensor, log_line, log_file);
			beginning_date = get_older_date (sensor, log_line, beginning_date);
		}
		g_free (log_line);
		
		fclose (log_file);
		tmp = g_slist_next (tmp);
	}
	
	g_free (sensor);
	return beginning_date;
}


time_t get_older_date (char * sensor, char * log_line, time_t beginning_date)
{
	time_t log_date = get_date_from_log (sensor, log_line);
	
	if (beginning_date == 0 || (log_date > 0 && log_date < beginning_date)) 
		return log_date;
	else
		return beginning_date;

}


time_t get_date_from_log (char * sensor, char * log_line)
{
	time_t 		log_date = 0;
	struct tm 	* time_struct = malloc (sizeof (struct tm));
	date_struct 	date;
	
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;
	char 		pattern[256];
	
	// Regex to use to extract date from log
	if (g_strcmp0(sensor, SELINUX) == 0 ) {
		sprintf (pattern, "msg=audit[^0-9]([0-9]*)");	
	}
	else if (g_strcmp0(sensor, OSIRIS) == 0 ) {
		sprintf (pattern, "compare time: ([a-zA-Z]{3}) ([a-zA-Z]{3}) ([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2}) ([0-9]{4})");
	}
	else if (g_strcmp0(sensor, SNORT) == 0 ) { 
		sprintf (pattern, "([0-9]{2})/([0-9]{2})-([0-9]{2}):([0-9]{2}):([0-9]{2})"); 
	}
	else if (g_strcmp0(sensor, SYSLOG) == 0 ) {
		sprintf (pattern, "([a-zA-Z]{3}) *([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2})");
	}

	re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
	if (re == NULL) {
		  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
		  return -1;
	}
	rc = pcre_exec(re, NULL, log_line, (int)strlen(log_line), 0, 0, ovector, OVECCOUNT);    

	if (rc > 0) {
		// Fill a struct tm to save the log date
		if ( (strcmp (sensor, OSIRIS) == 0)) {
			sprintf(date.Year, "%.*s", ovector[15] - ovector[14], log_line + ovector[14]);				
			sprintf(date.Month, "%.*s", ovector[5] - ovector[4], log_line + ovector[4]);
			sprintf(date.Day, "%.*s", ovector[7] - ovector[6], log_line + ovector[6]);
			sprintf(date.Hour, "%.*s", ovector[9] - ovector[8], log_line + ovector[8]);
			sprintf(date.Min, "%.*s", ovector[11] - ovector[10], log_line + ovector[10]);
			sprintf(date.Sec, "%.*s", ovector[13] - ovector[12], log_line + ovector[12]);

			time_struct->tm_year = atoi (date.Year);
			time_struct->tm_mon = get_month (date.Month);
			time_struct->tm_mday = atoi (date.Day);
			time_struct->tm_hour = atoi (date.Hour);
			time_struct->tm_min = atoi (date.Min);
			time_struct->tm_sec = atoi (date.Sec);
		}
		else if (strcmp (sensor, SELINUX) == 0) {
			char time_sec[256];
			sprintf(time_sec, "%.*s", ovector[3] - ovector[2], log_line + ovector[2]);
			int time_sec_value = atoi (time_sec);
			time_struct = gmtime((time_t*)&time_sec_value);
			time_struct->tm_year += 1900;
			time_struct->tm_mon += 1;
		}
		else if (strcmp (sensor, SNORT) == 0) {
			sprintf(date.Month, "%.*s", ovector[3] - ovector[2], log_line + ovector[2]);
			sprintf(date.Day, "%.*s", ovector[5] - ovector[4], log_line + ovector[4]);
			sprintf(date.Hour, "%.*s", ovector[7] - ovector[6], log_line + ovector[6]);
			sprintf(date.Min, "%.*s", ovector[9] - ovector[8], log_line + ovector[8]);
			sprintf(date.Sec, "%.*s", ovector[11] - ovector[10], log_line + ovector[10]);

			time_t current_time = time(NULL);
			time_struct = gmtime(&current_time);
			time_struct->tm_year += 1900;

			time_struct->tm_mon = atoi (date.Month);
			time_struct->tm_mday = atoi (date.Day);
			time_struct->tm_hour = atoi (date.Hour);
			time_struct->tm_min = atoi (date.Min);
			time_struct->tm_sec = atoi (date.Sec);
		}
		else if (strcmp (sensor, SYSLOG) == 0) {
			sprintf(date.Month, "%.*s", ovector[3] - ovector[2], log_line + ovector[2]);
			sprintf(date.Day, "%.*s", ovector[5] - ovector[4], log_line + ovector[4]);
			sprintf(date.Hour, "%.*s", ovector[7] - ovector[6], log_line + ovector[6]);
			sprintf(date.Min, "%.*s", ovector[9] - ovector[8], log_line + ovector[8]);
			sprintf(date.Sec, "%.*s", ovector[11] - ovector[10], log_line + ovector[10]);

			time_t current_time = time(NULL);
			time_struct = gmtime(&current_time);
			time_struct->tm_year += 1900;

			time_struct->tm_mon = get_month (date.Month);
			time_struct->tm_mday = atoi (date.Day);
			time_struct->tm_hour = atoi (date.Hour);
			time_struct->tm_min = atoi (date.Min);
			time_struct->tm_sec = atoi (date.Sec);
		}

		if (time_struct->tm_year > 1900) {
			time_struct->tm_year = time_struct->tm_year - 1900;
			time_struct->tm_mon = time_struct->tm_mon - 1;
		}


		log_date = mktime(time_struct);
	}
		
	pcre_free (re);
	
	return log_date;
}


time_t get_user_date (int day, int month, int year)
{
	time_t 		log_date = 0;
	struct tm 	* time_struct = malloc (sizeof (struct tm));

	time_struct->tm_year = year - 1900;
	time_struct->tm_mon = month - 1;
	time_struct->tm_mday = day;
	time_struct->tm_hour = 0;
	time_struct->tm_min = 0;
	time_struct->tm_sec = 0;

	log_date = mktime(time_struct);

	return log_date;
}



int display_logs (frame_t * f, int file_to_edit, time_t first_log_date, int delta, log_struct ** logs, long * log_pos, int display)
{
	data_t 		*data 			= (data_t *) f->priv;

	int 		end_flag 		= 0;
	int 		k			= 0;
	int 		log_index	 	= 0;
	int 		log_date 		= 0;
	int 		log_flag 		= 0;
	int 		osiris_file 		= 2;
	GSList 		* tmp 			= NULL;
	char 		* log_line		= NULL;
	char 		* path 			= NULL;
	char 		* sensor		= NULL;
	FILE 		* log_file		= NULL;
	time_t 		end_display_date 	= first_log_date + delta;

	tmp = g_slist_copy (data->Sensors_List);

	// For each sensor
	for (k = 0 ; k < data->Number_Of_Sensors ; k++) {
		// Select the log file
		sensor = g_strdup(tmp->data);
		if (g_strcmp0 (sensor, OSIRIS) == 0) {
			path = g_strdup_printf ("./%s/%s/%s/%s/%i", MACHINES_DIR
							, machine_get_folder_name (f->display_machine)
							, LOGS_DIR
							, sensor
							, osiris_file);
			osiris_file++;
			k--;
		}
		else if (g_strcmp0 (sensor, SELINUX) == 0) {
			path = g_strdup_printf ("./%s/%s/%s/%s/audit.log", MACHINES_DIR
								, machine_get_folder_name (f->display_machine)
								, LOGS_DIR
								, sensor);
		}
		else if (g_strcmp0 (sensor, SNORT) == 0) {
			path = g_strdup_printf ( "./%s/%s/%s/%s/alert", MACHINES_DIR
							, machine_get_folder_name (f->display_machine)
							, LOGS_DIR
							, sensor);
		}
		else if (g_strcmp0 (sensor, SYSLOG) == 0) {
			path = g_strdup_printf ("./%s/%s/%s/syslog_ssh/auth.log", MACHINES_DIR
									, machine_get_folder_name (f->display_machine)
									, LOGS_DIR);
		}
		else {
			return 0;		
		}

		// Find the first log to read, according to the beginning date
		log_file = find_date_position_in_file(sensor, path, first_log_date, log_pos[k]);

		g_free (path);
		log_line = NULL;

		if (log_file != NULL) {
			log_pos[k] = ftell (log_file);

			char * log_tmp = NULL;
			end_flag = 0;
			int first_log = 1;
			
			while (!feof(log_file) && end_flag == 0) {
				log_line = get_log(sensor, log_line, log_file);
				log_date = get_date_from_log (sensor, log_line);

				// Display logs 
				if (g_strcmp0 (sensor, OSIRIS) != 0 && log_date > 0 && log_date < end_display_date) {
					logs[log_index] = malloc (sizeof (log_struct));
					log_tmp = g_strdup_printf("%s", log_line);
					logs[log_index]->sensor = g_strdup_printf ("%s", sensor);
					logs[log_index]->log_line = g_strdup_printf ("%s", log_tmp);
					if (display == 1) {
						if (first_log == 1) {
							printf("\n\n*** %s ***\n\n", sensor);
							first_log = 0;
						}
						printf("%i: %s\n", log_index, logs[log_index]->log_line);
					}
					log_index++;
					log_flag = 1;
				}
				else if (log_date > 0 && log_date < end_display_date && log_date >= first_log_date) {
					// For Osiris, display the entire log file
					while (!feof(log_file) && end_flag == 0) {
						if (g_strstr_len (log_line, -1, "[") != NULL) {
							
							logs[log_index] = malloc (sizeof (log_struct));
							log_tmp = g_strdup_printf("%s", log_line);
							logs[log_index]->sensor = g_strdup_printf ("%s", sensor);
							logs[log_index]->log_line = g_strdup_printf ("%s", log_tmp);
							if (display == 1) {
								if (first_log == 1) {
									printf("\n\n*** %s ***\n\n", sensor);
									first_log = 0;
								}
								printf("%i: %s\n", log_index, logs[log_index]->log_line);
							}
							log_index++;
							log_flag = 1;
						}
							log_line = get_log(sensor, log_line, log_file);
					}
				}
				else {
					end_flag = 1;
				}
			
				if (log_index > 255) {
					printf ("Too many logs in this time period: enter a smaller delta\n");
					end_flag = 1;
				}
			}
		}
		
		if (g_strcmp0 (sensor, OSIRIS) != 0) {
			tmp = g_slist_next(tmp);
		}
		else {
			if (log_file == NULL) {
				k++;
				tmp = g_slist_next(tmp);
			}
		}
		if (log_file != NULL)
			fclose (log_file);
	}
	g_free (sensor);
	
	return log_index;
}




int create_new_activity (frame_t * f, char * sensor)
{
	size_t 		size 		= 256;
	size_t 		length 		= 0;
	char 		* answer 	= NULL;
	int 		regex_number 	= 0;
	int 		regex_max_number = 0;
	int 		value_number 	= 0;

	// Regex handler
	do {
		printf ("Create a regex for the new activity (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');

	if (answer[0] == 'y') { 
		// Create a new regex for the new activity
		do {
			printf ("Display regex already used for this sensor (y/n)? ");	
			getline (&answer, &size, stdin);
		} while (answer[0] != 'y' && answer[0] != 'n');
		
		if (answer[0] == 'y') { 
			display_file (REGEX_FILE, sensor, 0);
		}
		regex_number = create_new_regex(f, sensor);
	}
	else {
		// Select an existing regex for the new activity
		regex_max_number = display_file(REGEX_FILE, sensor, 0);
		do {
			printf("Enter the number of the regex for which the value will be added: ");
			scanf ("%i", &regex_number);
		} while (regex_number <= 0 || regex_number > regex_max_number);
		getchar();
	}

	// Value handler
	do {
		printf ("Create a value for the new activity regex (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');

	if (answer[0] == 'y') { 
		// Create a new value for the new activity
		do {
			printf ("Display values already used for this regex(y/n)? ");	
			getline (&answer, &size, stdin);
		} while (answer[0] != 'y' && answer[0] != 'n');
		
		if (answer[0] == 'y') { 
			display_file (VALUE_FILE, NULL, regex_number);
		}
		value_number = create_new_value(f, sensor, regex_number);
	}
	else {
		// Select an existing value for the new activity
		int value_max_number = 0;
		value_max_number = display_file(VALUE_FILE, NULL, regex_number);
		do {
			printf("Enter the number of the value to be added to the activity: ");
			scanf ("%i", &value_number);
		} while (value_number <= 0 || value_number > value_max_number);
		getchar();
	}


	// Get the index for the new activity
	char 	string[256]; 
	char 	* path 		= g_strdup (ACTIVITY_FILE_PATH);
	FILE 	* file 		= fopen (path, "a+");
	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning the activity file");
		return -1;
	}
	g_free (path);
	int activity_number = 0;
	while (fgets(string, 256, file) != NULL)
		activity_number++;

	// Select new activity's name	
	char * act_name = NULL;
	printf ("Name for new activity?\n");
	length = getline (&act_name, &size, stdin);
	act_name = g_strndup (act_name, length-1);

	// Select new activity's delta
	int delta = 0;
	printf ("Delta for new activity (in seconds)?\n");
	scanf ("%i", &delta);
	getchar();
	
	// Select new activity's color
	char * color = NULL;
	printf ("Activity color?\n");
	length = getline (&color, &size, stdin);
	color = g_strndup (color, length-1);

	// Write new activity to file
	fprintf (file, "%i \"%s\" %s %i:%i\n", ++activity_number, act_name, color, value_number, abs(delta));

	fclose (file);
	g_free (answer);
	g_free (act_name);	
	g_free (color);

	printf ("The activity has been created\n\n");

	return activity_number;
}

int create_new_regex (frame_t * f, char * sensor)
{
	char 		string[256];
	char 		* regex 		= NULL;
	char 		* path 			= g_strdup (REGEX_FILE);
	int 		regex_number 		= 0;
	size_t 		size 			= 256;
	FILE 		* file 			= fopen (path, "a+");

	if (file == NULL) { 
		perror("Error openning the regex file");
		return -1;
	}
	g_free (path);

	// Get index for the new regex	
	while (fgets(string, 256, file) != NULL)
		regex_number++;

	// Enter new regex
	printf ("Regex to match?\n");
	getline (&regex, &size, stdin);

	// Write regex rule to file
	fprintf (file, "%i %s ::regex::%s", ++regex_number, sensor, regex);
	
	g_free (regex);
	fclose (file);
	printf("The regex has been created\n\n");

	return regex_number;
}

int display_file (char * path, char * sensor, int rule) 
{
	int 		number_of_lines	= 0;
	size_t 		size 		= 256;
	char 		* line 		= NULL;
	
	FILE 		* file 		= fopen (path, "r");
	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning file");
		return -1;
	}

	// Print file name
	if (g_strcmp0 (path, REGEX_FILE) == 0)
		printf("\n\n*** Criticality Regex *** \n\n");
	else if (g_strcmp0 (path, VALUE_FILE) == 0)
		printf("\n\n*** Criticality Values *** \n\n");
	else if (g_strcmp0 (path, ACTIVITY_FILE_PATH) == 0)
		printf("\n\n*** Activities *** \n\n");
	else if (g_strcmp0 (path, SEQUENCES_FILE) == 0)
		printf("\n\n*** Sequences *** \n\n");
	else if (g_strcmp0 (path, INTRA_ACTIVITY_RULES_FILE) == 0)
		printf("\n\n*** Intra-Activity Rules *** \n\n");
			
	if (sensor != NULL) {
		// If a sensor name was given, display only rules about this sensor
		while (getline (&line, &size, file) != -1) {
			if (g_strstr_len (line, -1, sensor) != NULL) {
				printf("%s", line);
			}
			number_of_lines++;
		}
	}
	else if (rule != 0) {
		// If a rule number was given, display only rules about it
		char * string = NULL;
		if (g_strcmp0 (path, VALUE_FILE) == 0)
			string = g_strdup_printf (" %i ", rule);
		else if (g_strcmp0 (path, INTRA_ACTIVITY_RULES_FILE) == 0)
			string = g_strdup_printf ("%i ", rule);
	
		while (getline (&line, &size, file) != -1) {
			if (g_strstr_len (line, -1, string) != NULL) {
				printf("%s", line);
			}
			number_of_lines++;
		}
		g_free (string);
	}
	else {
		// Display all the file
		while (getline (&line, &size, file) != -1) {
			printf("%s", line);
			number_of_lines++;
		}
	}
	printf("\n\n");

	g_free (line);	
	fclose (file);
	
	return number_of_lines;
}


int create_new_value (frame_t * f, char * sensor, int regex_number)
{
	char 		string[256];
	char 		* value	 	= NULL;
	int 		value_number 	= 0;
	int 		criticality 	= 0;
	size_t 		length 		= 0;
	size_t 		size 		= 256;

	char 		* path 		= g_strdup (VALUE_FILE);
	FILE 		* file 		= fopen (path, "a+");
	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning the value file");
		return -1;
	}
	g_free (path);

	// If no regex is selected, select one for the new value
	if (regex_number == 0) {
		int regex_max_number = display_file(REGEX_FILE, sensor, 0);
		do {
			printf("Enter the number of the regex for which the value will be added: ");
			scanf ("%i", &regex_number);
		} while (regex_number <= 0 || regex_number > regex_max_number);
		getchar();
	}

	// Get index for the new value
	while (fgets(string, 256, file) != NULL)
		value_number++;

	// Enter new value
	printf ("Possible value matched by the regex? \n");
	length = getline (&value, &size, stdin);
	// Enter criticality for the new value
	printf ("Criticality for this value?\n");
	scanf ("%i", &criticality);
	value = g_strndup (value, length-1);
	getchar();
	// Write new value rule to file
	fprintf (file, "%i %i %s %i\n", ++value_number, regex_number, value, criticality);

	g_free (value);
	fclose (file);
	printf ("The value has been created\n\n");

	return value_number;
}


int * find_activity (char * sensor, char * log_line, int display_activity)
{
	criticality_struct * criticality;
	criticality = malloc(sizeof(criticality_struct));
	criticality->criticality_time = malloc(sizeof(struct tm));
	criticality->value = -1;
	// Look for the index of the value rule for log_line (and its criticality)
	find_pattern (sensor, log_line, criticality, NULL, NULL);

	int * returned_value = malloc (2 * sizeof(int));
	returned_value[0] = -1;
	returned_value[1] = -1;

	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;
	char 		pattern[256];
	char 		string[256];
	
	char		activity_number[256];
	char		activity_name[256];
	char 		* path 				= g_strdup (ACTIVITY_FILE_PATH);
	FILE 		* activity_type_file 		= fopen (path, "r");
	g_free (path);

	if (activity_type_file == NULL) { 
		perror("Error openning the activity type file\n");
		return returned_value;
	}

	// Look for an activity with the value rule find by find_pattern()
	sprintf (pattern, "([0-9]*) \"([^\"]*)\" ([^ ]*).* %i:([0-9]*)", criticality->value_rule_number);
	while(fgets (string, 256, activity_type_file) != NULL) {
		re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
		if (re == NULL) {
			  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
			  return returned_value;
		}
		rc = pcre_exec(re, NULL, string, (int)strlen(string), 0, 0, ovector, OVECCOUNT);    
		if (rc > 0) {
			sprintf(activity_number, "%.*s", ovector[3] - ovector[2], string + ovector[2]);
			sprintf(activity_name, "%.*s", ovector[5] - ovector[4], string + ovector[4]);
			if (display_activity == 1)
				printf("Log activity : %s %s\n", activity_number, activity_name);		
		}
	}

	pcre_free (re);
	fclose (activity_type_file);
	
	// Return activity number and log criticality
	returned_value[0] = atoi(activity_number);
	returned_value[1] = criticality->value;

	g_free (criticality);

	return returned_value;	
}


FILE * find_date_position_in_file(char * sensor, char * path, time_t first_date_to_read, long current_offset)
{
	char 		* log_line 	= NULL;
	long 		new_offset 	= 0;
	FILE 		* log_file 	= fopen(path,"r");

	if (log_file == NULL) {
		return log_file;
	}
	
	// Go to last position read in this file
	fseek (log_file, current_offset, SEEK_SET);
	log_line = get_log(sensor, log_line, log_file);
		
	// If the log to read is before the current position, go to the beginning of the file
	if (get_date_from_log (sensor, log_line) > first_date_to_read) {
		rewind(log_file);
		log_line = get_log(sensor, log_line, log_file);
	}

	// Look for the first line to read
	while (!feof(log_file) && get_date_from_log (sensor, log_line) < first_date_to_read) {
		new_offset = ftell (log_file);	
		log_line = get_log(sensor, log_line, log_file);
	}

	fseek(log_file, new_offset, SEEK_SET);
	g_free (log_line);	

	return log_file;
}


int modify_activity (frame_t * f)
{
	char 		* answer 		= NULL;
	char 		* sensor 		= NULL;
	int 		activity_number 	= 0;
	int 		activity_max_number 	= 0;
	int 		delta 			= 0;
	int 		regex_number 		= 0;
	size_t 		size 			= 256;
	size_t 		length 			= 0;

	activity_max_number = display_file (ACTIVITY_FILE_PATH, NULL, 0);
	
	// Select the activity to modify
	do {
		printf("Select the activity to be modify: ");
		scanf ("%i", &activity_number);
	} while (activity_number > activity_max_number || activity_number < 0);
	getchar();

	// Regex handler
	do {
		printf ("Create a regex for this activity (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');

	if (answer[0] == 'y') { 
		// Create a new regex
		do {
			printf("Enter the sensor for the regex you want to create: \n");
			length = getline (&sensor, &size, stdin);
		} while (length == -1);
		sensor = g_strndup (sensor, length-1);
	
		do {
			printf ("Display regex already used for this sensor (y/n)? ");	
			getline (&answer, &size, stdin);
		} while (answer[0] != 'y' && answer[0] != 'n');
		
		if (answer[0] == 'y') { 
			display_file (REGEX_FILE, sensor, 0);
		}
		regex_number = create_new_regex(f, sensor);
	}
	else {
		// Select an existing regex
		do {
			printf("Enter the sensor for the regex you want to use: \n");
			length = getline (&sensor, &size, stdin);
		} while (length == -1);
		sensor = g_strndup (sensor, length-1);
	

		int regex_max_number = 0;
		regex_max_number = display_file(REGEX_FILE, sensor, 0);
		do {
			printf("Enter the number of the regex for which the value will be added: ");
			scanf ("%i", &regex_number);
		} while (regex_number <= 0 || regex_number > regex_max_number);
		getchar();
	}

	// Value handler
	int value_number = 0;
	do {
		printf ("Create a value for this regex (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');

	if (answer[0] == 'y') { 
		// Create a new value
		do {
			printf ("Display values already used for this regex(y/n)? ");	
			getline (&answer, &size, stdin);
		} while (answer[0] != 'y' && answer[0] != 'n');
		
		if (answer[0] == 'y') { 
			display_file (VALUE_FILE, NULL, regex_number);
		}
		value_number = create_new_value(f, sensor, regex_number);
	}
	else {
		// Select an existing value
		int value_max_number = 0;
		value_max_number = display_file(VALUE_FILE, NULL, regex_number);
		do {
			printf("Enter the number of the value to be added to the activity: ");
			scanf ("%i", &value_number);
		} while (value_number <= 0 || value_number > value_max_number);
		getchar();
	}

	// Enter delta
	printf ("Delta for this activity value (in seconds)?\n");
	scanf ("%i", &delta);
	getchar();

	// Write changes to file
	rewrite_file (f, ACTIVITY_FILE_PATH, activity_number, value_number, delta);

	g_free (answer);
	g_free (sensor);

	return activity_number;
}


int rewrite_file (frame_t * f, char * path, int activity_number, int value_number, int delta)
{
	char 		* line 		= NULL;
	int 		number_of_lines = 0;
	size_t 		size 		= 256;
	size_t 		length 		= 0;

	FILE 		* file 		= fopen (path, "r+");
	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning file");
		return -1;
	}
	else {
		char 	* path_tmp 	= frame_get_absolute_tmp_path (f, "tmp");
		FILE 	* file_tmp 	= fopen (path_tmp, "w+");
		if (file_tmp == NULL) { 
			fclose (file);
			perror("Error openning file");
			return -1;
		}
		else {
			g_free (path_tmp);
	
			// Write lines previous to the one to modify in a temporary file			
			while ( number_of_lines < activity_number -1 && getline (&line, &size, file) != -1 ) {
				fprintf(file_tmp, "%s", line);
				number_of_lines++;
			}	
			// Modify the line and write it in tmp file
			length = getline (&line, &size, file);
			line = g_strndup (line, strlen(line) - 1);
			fprintf(file_tmp, "%s %i:%i\n", line, value_number, delta);
			number_of_lines++;
			// Copy all other lines in tmp file
			while (getline (&line, &size, file) != -1) {
				fprintf(file_tmp, "%s", line);
				number_of_lines++;
			}	
			
			fprintf(file_tmp, "\n");

			g_free (line);
			fflush (file_tmp);
			fseek(file, 0, SEEK_SET);
			fseek(file_tmp, 0, SEEK_SET);
			int	j 	= 0;
			char 	* line2 = NULL;

			// Rewrite file from tmp file	
			for (j=0 ; j<number_of_lines ; j++) {
				getline (&line2, &size, file_tmp);
				fprintf(file, "%s", line2);
			}
			
			g_free (line2);

			if (file_tmp != NULL) {
				if (fclose (file_tmp) != 0)
					perror("file_tmp");
			}
			if (file != NULL) {
				if (fclose (file) != 0)
					perror ("file");
			}
		}
	}
	
	return 0;
}



int create_new_sequence (frame_t * f, log_struct ** logs, int log_index)
{
	char 		string[256];
	char 		* sequence 		= NULL;
	char 		* path 			= g_strdup (SEQUENCES_FILE);
	char		* sensor		= NULL;
	char		* answer		= NULL;
	char 		* new_sequence 		= NULL;
	int 		first_activity_number 	= 0;
	int 		second_activity_number 	= 0;
	int 		sequence_number 	= 0;
	int 		sequence_max_number 	= 0;
	int 		flag 			= 0;
	size_t 		size 			= 256;
	size_t		length			= 0;
	FILE 		* file 			= fopen (path, "a+");

	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning the sequences file");
		return -1;
	}
	g_free (path);
	
	// Get index for new sequence 
	while (fgets(string, 256, file) != NULL) {
		sequence_number++;
	}

	// Enter new sequence name
	do {
		printf("Enter the name of the sequence: \n");
		length = getline (&sequence, &size, stdin);
	} while (length == -1);
	sequence = g_strndup (sequence, length-1);
		
	new_sequence = 	g_strdup_printf ("%i \"%s\"", ++sequence_number, sequence);
	// Describe sequence
	do {
		do {
			printf ("Use an existing sequence as sub-sequence (y/n)? ");	
			getline (&answer, &size, stdin);
		} while (answer[0] != 'y' && answer[0] != 'n');

		if (answer[0] == 'y') { 
			// Sub-sequence is an existing sequence
			if (flag == 0) { // If it is the first sub-sequence 
				int sub_sequence_number = 0;
				sequence_max_number = display_file(SEQUENCES_FILE, NULL, 0);
				// Select sequence to be used as sub-sequence
				do {
					printf("Enter the number of the sequence to be used as sub-sequence: ");
					scanf ("%i", &sub_sequence_number);
				} while (sub_sequence_number <= 0 || sub_sequence_number > sequence_max_number);
				getchar();
				
				new_sequence = 	g_strdup_printf  ("%s %i", new_sequence, sub_sequence_number);
				second_activity_number = extract_last_activity (sub_sequence_number);
			} 
			else { // If it isn't the first sub-sequence 
				int sub_sequence_number = 0;
				sequence_max_number = display_file(SEQUENCES_FILE, NULL, 0);
				// Select sequence to be used as sub-sequence
				do {
					printf("Enter the number of the sequence to be used as sub-sequence: ");
					scanf ("%i", &sub_sequence_number);
				} while (sub_sequence_number <= 0 || sub_sequence_number > sequence_max_number);
				getchar();

				// Get first activity in this sub-sequence. 
				// If it isn't the same that the last one in new sequence, add a transition
				first_activity_number = extract_first_activity (sub_sequence_number);
				if (first_activity_number == second_activity_number) {
					new_sequence = 	g_strdup_printf  ("%s %i", new_sequence, sub_sequence_number);
				}
				else {
					new_sequence = 	g_strdup_printf  ("%s %i::%i %i", new_sequence
											, second_activity_number
											, first_activity_number
											, sub_sequence_number);
				}
				second_activity_number = extract_last_activity (sub_sequence_number);
			}
		}
		else {
			// Sub-sequence is an activities pair
			if (flag == 0) { // If it is the first sub-sequence 
				// First activity handler
				printf("\nFirst Activity: \n");
				do {
					printf ("Create an activity for the new sequence (y/n)? ");	
					getline (&answer, &size, stdin);
				} while (answer[0] != 'y' && answer[0] != 'n');

				if (answer[0] == 'y') { 
					// Create a new activity
					do {
						printf("Enter the sensor for the regex you want to create: \n");
						length = getline (&sensor, &size, stdin);
					} while (length == -1);
					sensor = g_strndup (sensor, length-1);
				
					do {
						printf ("Display activities already used (y/n)? ");	
						getline (&answer, &size, stdin);
					} while (answer[0] != 'y' && answer[0] != 'n');
					
					if (answer[0] == 'y') { 
						display_file (ACTIVITY_FILE_PATH, sensor, 0);
					}
					first_activity_number = create_new_activity(f, sensor);
				}
				else {
					// Select an existing activity
					int activity_max_number = 0;
					activity_max_number = display_file(ACTIVITY_FILE_PATH, sensor, 0);
					do {
						printf("Enter the number of the activity you want to add to the new sequence: ");
						scanf ("%i", &first_activity_number);
					} while (first_activity_number <= 0 || first_activity_number > activity_max_number);
					getchar();
				}
				
				// Second activity handler
				printf("\nSecond activity: \n");
				do {
					printf ("Create an activity for the new sequence (y/n)? ");	
					getline (&answer, &size, stdin);
				} while (answer[0] != 'y' && answer[0] != 'n');

				if (answer[0] == 'y') { 
					// Create a new activity
					do {
						printf("Enter the sensor for the regex you want to create: \n");
						length = getline (&sensor, &size, stdin);
					} while (length == -1);
					sensor = g_strndup (sensor, length-1);
				
					do {
						printf ("Display activities already used (y/n)? ");	
						getline (&answer, &size, stdin);
					} while (answer[0] != 'y' && answer[0] != 'n');
					
					if (answer[0] == 'y') { 
						display_file (ACTIVITY_FILE_PATH, sensor, 0);
					}
					second_activity_number = create_new_activity(f, sensor);
				}
				else {
					// Select an existing activity
					int activity_max_number = 0;
					activity_max_number = display_file(ACTIVITY_FILE_PATH, sensor, 0);
					do {
						printf("Enter the number of the activity you want to add to the new sequence: ");
						scanf ("%i", &second_activity_number);
					} while (second_activity_number <= 0 || second_activity_number > activity_max_number);
					getchar();
				}
			}
			else { // If it isn't the first sub-sequence 
				printf("\nNext Activity: \n");
				first_activity_number = second_activity_number;
				
				do {
					printf ("Create an activity for the new sequence (y/n)? ");	
					getline (&answer, &size, stdin);
				} while (answer[0] != 'y' && answer[0] != 'n');

				if (answer[0] == 'y') { 
					// Create a new activity
					do {
						printf("Enter the sensor for the regex you want to create: \n");
						length = getline (&sensor, &size, stdin);
					} while (length == -1);
					sensor = g_strndup (sensor, length-1);
				
					do {
						printf ("Display activities already used (y/n)? ");	
						getline (&answer, &size, stdin);
					} while (answer[0] != 'y' && answer[0] != 'n');
					
					if (answer[0] == 'y') { 
						display_file (ACTIVITY_FILE_PATH, sensor, 0);
					}
					second_activity_number = create_new_activity(f, sensor);
				}
				else {
					// Select an existing activity
					int activity_max_number = 0;
					activity_max_number = display_file(ACTIVITY_FILE_PATH, sensor, 0);
					do {
						printf("Enter the number of the activity you want to add to the new sequence: ");
						scanf ("%i", &second_activity_number);
					} while (second_activity_number <= 0 || second_activity_number > activity_max_number);
					getchar();
				}
			}
			new_sequence = 	g_strdup_printf  ("%s %i::%i", new_sequence, first_activity_number, second_activity_number);
		}
		
		flag = 1;
		
		do {
			printf ("\nAdd another activity or sub-sequence to the sequence (y/n)? ");	
			getline (&answer, &size, stdin);
		} while (answer[0] != 'y' && answer[0] != 'n');

	} while ( answer[0] == 'y');
	
	// Write new sequence to file
	new_sequence = g_strdup (replace_by_subsequence (f, new_sequence));
	fprintf (file, "%s ", new_sequence);
	
	printf("\n");
	g_free (answer); 
	g_free (sensor);
	g_free (sequence);
	fclose (file);

	// Create deltas for new sequence
	create_delta_for_sequence (f, logs, log_index, sequence_number);
	printf("The activities sequence has been created\n\n");

	return sequence_number;

}


int extract_last_activity (int sequence_number) 
{
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;
	char 		pattern[256];

	char 		activity[256];
	char 		* line			= NULL;
	char 		* path 			= g_strdup (SEQUENCES_FILE);
	int 		last_activity 		= 0;
	int 		number_of_lines 	= 0;
	size_t 		size 			= 256;
	FILE 		* file 			= fopen (path, "a+");
	
	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning the sequences file");
		return -1;
	}
	g_free (path);

	// Get the sequence from which activity will be extracted		
	while (getline (&line, &size, file) != -1 && number_of_lines < sequence_number - 1) {
		number_of_lines++;
	}	

	// Extract last activity
	sprintf (pattern, "[a-zA-Z0-9 :]*((:[0-9]*)|( [0-9]*)) ");	
	re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
	if (re == NULL) {
		  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
		  return -1;
	}
	rc = pcre_exec(re, NULL, line, (int)strlen(line), 0, 0, ovector, OVECCOUNT);    

	if (rc > 0) {
		sprintf(activity, "%.*s", ovector[3] - ovector[2], line + ovector[2]);
		if (g_strstr_len (activity, -1, ":") == NULL) {
			last_activity =  atoi(activity+1);
			last_activity = extract_last_activity (last_activity);
		}
		else {
			last_activity =  atoi(activity+1);
		}
	}

	pcre_free (re);
	g_free (line);
	fclose (file);

	return last_activity;
}

int extract_first_activity (int sequence_number)
{
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;
	char 		pattern[256];

	char 		* line			= NULL;
	char 		* path 			= g_strdup (SEQUENCES_FILE);
	char 		activity[256];
	int 		first_activity 		= 0;
	int 		number_of_lines 	= 0;
	size_t 		size 			= 256;
	FILE 		* file 			= fopen (path, "a+");

	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning the sequences file");
		return -1;
	}
	g_free (path);
		
	// Get the sequence from which activity will be extracted		
	while (getline (&line, &size, file) != -1 && number_of_lines < sequence_number - 1) {
		number_of_lines++;
	}	

	// Extract first activity
	sprintf (pattern, "[0-9]* \"[a-zA-Z0-9 ]*\" (([0-9]*:)|([0-9]* ))");	
	re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
	if (re == NULL) {
		  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
		  return -1;
	}

	rc = pcre_exec(re, NULL, line, (int)strlen(line), 0, 0, ovector, OVECCOUNT);    

	if (rc > 0) {
		sprintf(activity, "%.*s", ovector[3] - ovector[2], line + ovector[2]);
		if (g_strstr_len (activity, -1, ":") == NULL) {
			first_activity =  atoi(g_strndup (activity, strlen(activity)-1));
			first_activity = extract_first_activity (first_activity);
		}
		else {
			first_activity =  atoi(g_strndup (activity, strlen(activity)-1));
		}
	}

	pcre_free (re);
	g_free (line);
	fclose (file);

	return first_activity;
}



int extract_first_delta_from_sequence (int sequence_number)
{
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;
	char 		pattern[256];

	int 		first_delta 		= 0;
	int 		number_of_lines 	= 0;
	size_t 		size 			= 256;
	char 		* line			= NULL;
	char 		* path 			= g_strdup (SEQUENCES_FILE);
	char 		* string 		= malloc(256 * sizeof(char));
	FILE 		* file 			= fopen (path, "a+");
	
	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning the sequences file");
		return -1;
	}
	g_free (path);

	// Get the sequence from which delta will be extracted		
	while (getline (&line, &size, file) != -1 && number_of_lines < sequence_number ) {
		number_of_lines++;
	}	

	// Extract delta
	sprintf (pattern, "[0-9]* \"[a-zA-Z0-9 ]*\" (([0-9]*:[0-9]*)|([0-9]* ))");	
	re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
	if (re == NULL) {
		  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
		  return -1;
	}

	rc = pcre_exec(re, NULL, line, (int)strlen(line), 0, 0, ovector, OVECCOUNT);    

	if (rc > 0) {
		sprintf(string, "%.*s", ovector[3] - ovector[2], line + ovector[2]);
		if (g_strstr_len (string, -1, ":") == NULL) {
			first_delta =  atoi(g_strndup (string, strlen(string)-1));
			first_delta = extract_first_delta_from_sequence (first_delta);
		}
		else {
			string = g_strstr_len (string, -1, ":");
			first_delta =  atoi(g_strndup (string+1, strlen(string)));
		}
	}

	pcre_free (re);
	g_free (line);
	fclose (file);
	return first_delta;
}


int create_delta_for_sequence (frame_t * f, log_struct ** logs, int log_index, int sequence_nb)
{
	char		string[256];
	char 		* path 			= g_strdup (SEQUENCES_FILE);
	char 		* line			= NULL;
	char		* answer		= NULL;
	int 		number_of_lines 	= 0;
	int 		sequence_number 	= 0;
	int 		sequence_max_number 	= 0;
	int		sequence_criticality	= 1;
	size_t		size			= 256;
	size_t		length			= 0;
	FILE 		* file 			= fopen (path, "r+");
	
	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning file");
		return -1;
	}
	else {
		char 	* path_tmp 		= frame_get_absolute_tmp_path (f, "tmp");
		FILE 	* file_tmp 		= fopen (path_tmp, "w+");
		g_free (path);
		
		if (file_tmp == NULL) { 
			fclose (file);
			perror("Error openning file");
			return -1;
		}
		else {
			g_free (path_tmp);
			// Generate delta for each sub-sequence in a sequence
			printf ("\n\n*** Generate delta for sequence *** \n\n");
			
			// If the sequence index wasn't given as a parameter, enter it 
			if (sequence_nb == 0) {
				sequence_max_number = display_file(SEQUENCES_FILE, NULL, 0);			
				do {
					printf("Enter the number of the sequence you want to use: ");
					scanf ("%i", &sequence_number);
				} while (sequence_number <= 0 || sequence_number > sequence_max_number);
				getchar();
			}
			else {
				sequence_max_number = sequence_nb;
				sequence_number = sequence_nb;
			}

			// Get the sequence line in which delta will be added
			while (number_of_lines < sequence_number -1 && getline (&line, &size, file) != -1 ) {
				fprintf(file_tmp, "%s", line);
				number_of_lines++;
			}	
			length = getline (&line, &size, file);
			line = g_strndup (line, strlen(line)-1);
			number_of_lines++;

			// Handle sequence line
			char ** sequence_array_tmp = g_strsplit (line, "\"", 256);
			fprintf(file_tmp, "%s\"%s\" ", sequence_array_tmp[0], sequence_array_tmp[1]);
			char ** sequence_array = g_strsplit (sequence_array_tmp[2], " ", 256);

			int j = 1;
			int sub_sequence_nb = 0;
			while (sequence_array[j] != NULL && g_strcmp0 (sequence_array[j], "") != 0) {
				strcpy (string, sequence_array[j]);
				if (g_strstr_len (string, -1, "::") != NULL) {
					// If sub-sequence is an activities pair, find delta
					sequence_find_delta (f, file_tmp, string, logs, log_index);
				}
				else {
					// If sub-sequence is a sequence, update default sequence criticality
					fprintf(file_tmp, "%s ",string);	
					sub_sequence_nb = atoi (sequence_array[j]);
					if (sub_sequence_nb <= sequence_max_number)
						sequence_criticality *= get_sequence_criticality (sub_sequence_nb);
				}
				j++;
			}

			// Suggest a default criticality for the selected sequence
			printf ("\n\n*** Generate criticality for sequence *** \n\n");
			printf("The suggested criticality for this sequence is %i.\n",  sequence_criticality);
			do {
				printf ("Do you want to change it (y/n)? ");	
				getline (&answer, &size, stdin);
			} while (answer[0] != 'y' && answer[0] != 'n');

			if (answer[0] == 'y') { 
				// Change sequence criticality
				printf("Enter new criticality: ");
				scanf ("%i", &sequence_criticality);		
				getchar();
			}			

			fprintf(file_tmp, "(%i) \n", sequence_criticality);
			
			g_free (answer); 
			g_strfreev (sequence_array);
			g_strfreev (sequence_array_tmp);
						
			while (getline (&line, &size, file) != -1) {
				fprintf(file_tmp, "%s", line);
				number_of_lines++;
			}	
			
			fprintf(file_tmp, "\n");
			g_free (line);
			fflush (file_tmp);
			fseek(file, 0, SEEK_SET);
			fseek(file_tmp, 0, SEEK_SET);
			j = 0;		

			// Rewrite file from tmp file		
			char * line2 = NULL;
			for (j=0 ; j<number_of_lines ; j++) {
				getline (&line2, &size, file_tmp);
				fprintf(file, "%s", line2);
			}

			g_free (line2);
			printf("\nTime intervals have been added\n\n");
			if (file_tmp != NULL) {
				if (fclose (file_tmp) != 0)
					perror("file_tmp");
			}
			if (file != NULL) {
				if (fclose (file) != 0)
					perror ("file");
			}
		}
	}

	return 0;
}


int sequence_find_delta (frame_t * f, FILE * file_tmp, char * string, log_struct ** logs, int log_index) 
{
	data_t 		* data 				= (data_t *) f->priv;

	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;
	char 		pattern[256];

	char 		first_activity_string[256];
	char 		second_activity_string[256];
	int 		first_activity 			= 0;
	int 		second_activity 		= 0;
	int 		delta 				= 0;
	char		* answer 			= NULL;
	size_t 		size				= 256;

	// Get first and second activity from an activities pair
	sprintf (pattern, "([0-9]*)::([0-9]*)");	
	re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
	if (re == NULL) {
		  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
		  return -1;
	}
	rc = pcre_exec(re, NULL, string, (int)strlen(string), 0, 0, ovector, OVECCOUNT);    
	if (rc > 0) {
		sprintf(first_activity_string, "%.*s", ovector[3] - ovector[2], string + ovector[2]);
		sprintf(second_activity_string, "%.*s", ovector[5] - ovector[4], string + ovector[4]);
		first_activity = atoi (first_activity_string);
		second_activity = atoi (second_activity_string);
	}
	
	time_t 	first_act_min 		= 0;
	time_t 	first_act_medium 	= 0;
	time_t 	first_act_max 		= 0;
	int 	first_act_nb 		= 0;
	time_t 	second_act_min 		= 0;
	time_t 	second_act_medium 	= 0;
	time_t 	second_act_max 		= 0;
	int 	second_act_nb 		= 0;
	time_t 	log_time 		= 0;

	// Select logs to find delta between first and sexond activity
	long * log_pos = malloc (data->Number_Of_Sensors * sizeof(long));
	int i = 0;
	for (i=0 ; i < data->Number_Of_Sensors; i++) 
		log_pos[i] = 0;
	int delta_display = 0;
	delta_display = select_time_period();
	time_t first_log_date = 0;
	first_log_date = find_first_date(f);
	
	// Select first date to read
	do {
		printf ("Read logs from the beginning (y/n)? ");	
		getline (&answer, &size, stdin);
	} while (answer[0] != 'y' && answer[0] != 'n');
	
	if (answer[0] == 'n') { 
		int day = 0;
		int month = 0;
		int year = 0;
		printf("Enter a date to begin log display (jj/mm/yyyy): ");
		scanf ("%i/%i/%i", &day, &month, &year);
		first_log_date = get_user_date (day, month, year);
		getchar();
	}

	// Display logs	
	log_index = display_logs (f, 0, first_log_date, delta_display, logs, log_pos, 0);
	g_free (log_pos);
	
	i = 0;
	int * log_act = NULL;
	while (i < log_index) {
		log_act = find_activity (logs[i]->sensor, logs[i]->log_line, 0);
		if (log_act[0] == first_activity) {
			// If the log activity is the first one, update first activity infos
			log_time = get_date_from_log (logs[i]->sensor, logs[i]->log_line);

			first_act_nb++;
			first_act_medium += log_time;
			if (first_act_min == 0) {
				first_act_min = log_time;
				first_act_max = log_time;
			}
			else {
				if (log_time < first_act_min) 
					first_act_min = log_time;
				if (log_time > first_act_max) 
					first_act_max = log_time;
			}
		}
		else if (log_act[0] == second_activity) {
			// If the log activity is the second one, update second activity infos
			log_time = get_date_from_log (logs[i]->sensor, logs[i]->log_line);
		
			second_act_nb++;
			second_act_medium += log_time;
			if (second_act_min == 0) {
				second_act_min = log_time;
				second_act_max = log_time;
			}
			else {
				if (log_time < second_act_min) 
					second_act_min = log_time;
				if (log_time > second_act_max) 
					second_act_max = log_time;		
			}
		}
		i++;
	}
	
	if (first_act_nb != 0 && second_act_nb != 0) { 
		// If both activity have logs on the read time period, generate three default delta (min, max and medium)
		first_act_medium = first_act_medium / first_act_nb;
		second_act_medium = second_act_medium / second_act_nb;

		int delta_min = second_act_min - first_act_max;
		int delta_max = second_act_max - first_act_min;
		int delta_medium = second_act_medium - first_act_medium;
		delta = delta_medium;
		
		if (delta_min < 0 ){
			printf ("Warning: one value is negative. If it is selected, its absolute value will be used.\n");
		}
		
		// Select a default delta or ente a new one
		printf("What time interval do you want to use between the first activity (#%i) and the second one (#%i)?\n" 
					,first_activity, second_activity);
		printf("  1 - Minimum: %i seconds\n", delta_min);
		printf("  2 - Medium: %i seconds\n", delta_medium);
		printf("  3 - Maximum: %i seconds\n", delta_max);
		printf("  4 - Other\n");
		int delta_nb = 2;
		do {
			printf("Choose an interval: ");
			scanf ("%i", &delta_nb);
		} while (delta_nb < 1 || delta_nb > 4);
		getchar();
		
		if (delta_nb == 1)
			delta = abs(delta_min);
		else if (delta_nb == 2)
			delta = abs(delta_medium);
		else if (delta_nb == 3)
			delta = abs(delta_max);
		else if (delta_nb == 4) {
			printf("Enter an interval (in seconds): ");
			scanf ("%i", &delta);		
			getchar();
		}		
		fprintf(file_tmp, "%i:%i:%i ", first_activity, delta, second_activity);
	}
	else {
		// If one activity have no logs on this time period, enter a delta
		printf("\nOne activity has no log for this period.\n");	
		printf("Enter a delta (in seconds): ");
		scanf ("%i", &delta);		
		getchar();

		fprintf(file_tmp, "%i:%i:%i ", first_activity, delta, second_activity);
		
		g_free (answer); 
	}
	if (log_act != NULL)
		g_free (log_act);

	pcre_free (re);

	return 0;
}


int get_sequence_criticality (int sub_sequence_nb)
{
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;
	char 		pattern[256];

	int 		number_of_lines 	= 0;
	size_t 		size 			= 256;
	char 		* line			= NULL;
	char 		* path 			= g_strdup (SEQUENCES_FILE);
	FILE 		* file 			= fopen (path, "a+");
	char 		criticality[256];
	int 		sub_sequence_criticality = 1;

	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning the sequences file");
		return -1;
	}
	g_free (path);

	// Get	the subsequence
	while (getline (&line, &size, file) != -1 && number_of_lines < sub_sequence_nb - 1) {
		number_of_lines++;
	}	

	// Extract the sub-sequence criticality
	sprintf (pattern, "[a-zA-Z0-9 :]*[(]([0-9]*)[)]");	
	re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
	if (re == NULL) {
		  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
		  return -1;
	}
	rc = pcre_exec(re, NULL, line, (int)strlen(line), 0, 0, ovector, OVECCOUNT);    

	if (rc > 0) {
		sprintf(criticality, "%.*s", ovector[3] - ovector[2], line + ovector[2]);
		sub_sequence_criticality =  atoi(criticality);
	}

	pcre_free (re);
	g_free (line);
	fclose (file);

	return sub_sequence_criticality;
}



char * replace_by_subsequence (frame_t * f, char * new_sequence)
{
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;
	char 		pattern[256];

	char 		* path 			= g_strdup (SEQUENCES_FILE);
	FILE 		* file 			= fopen (path, "r");

	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning file");
	}
	else {
		size_t 	size 			= 256;
		char 	* line 			= NULL;
		char 	sub_sequence[256];

		sprintf (pattern, "\" ([0-9 :]*) [(]");	
		re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
		if (re == NULL) {
			  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
		}
		else {

			char * ptr = NULL;
			char * sequence_tmp = NULL;
			char * cleaned_sub_sequence = NULL;

			int sequence_nb = 1;
			size_t length = 0;
			while (getline (&line, &size, file) != -1) { // For each existing sequence
				// Extract sequence definition from sequence line
				rc = pcre_exec(re, NULL, line, (int)strlen(line), 0, 0, ovector, OVECCOUNT);    

				// Clean sequence definition from all delta
				if (rc > 0) {
					sprintf(sub_sequence, "%.*s", ovector[3] - ovector[2], line + ovector[2]);
					cleaned_sub_sequence = remove_deltas_from_sequence (sub_sequence);
				}
				
				// If it is a part of the new sequence, remplace it
				if ( (ptr = g_strstr_len (new_sequence, -1, cleaned_sub_sequence)) != NULL) {
					sequence_tmp = g_strndup (new_sequence, ptr - new_sequence);
					length = strlen (sequence_tmp);
					sequence_tmp = g_strdup_printf ("%s%i%s", sequence_tmp
										, sequence_nb
										, new_sequence + length + strlen (cleaned_sub_sequence));
					new_sequence = g_strdup (sequence_tmp);
				}
				sequence_nb++;
			}
			
			g_free (line);	
		}
		fclose (file);
	}

	return new_sequence;
}


char * remove_deltas_from_sequence (char * sequence)
{
	char ** sequence_array = g_strsplit (sequence, " ", 256);
	char ** activities_array_tmp = NULL;
	char * new_sequence = NULL;
	int j = 0;

	while (sequence_array[j] != NULL && g_strcmp0 (sequence_array[j], "") != 0) {
		if (g_strstr_len (sequence_array[j], -1, ":") != NULL) {
			activities_array_tmp = g_strsplit (sequence_array[j], ":", 3);
			if (new_sequence == NULL)
				new_sequence = g_strdup_printf("%s::%s", activities_array_tmp[0], activities_array_tmp[2]);
			else
				new_sequence = g_strdup_printf("%s %s::%s", new_sequence, activities_array_tmp[0], activities_array_tmp[2]);
		}
		else {
			if (new_sequence == NULL)
				new_sequence = g_strdup_printf("%s", sequence_array[j]);
			else
				new_sequence = g_strdup_printf("%s %s", new_sequence, sequence_array[j]);
		}
		j++;
	}

	g_strfreev(sequence_array);
	g_strfreev(activities_array_tmp);

	return new_sequence;
}


int write_intra_activity_rules_file (frame_t * f, char *  path, int index, char * pattern, int delta)
{
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;
	char 		* answer 	= NULL;

	FILE 		* file 		= fopen (path, "r+");
	if (file == NULL) { 
		printf("%s\n", path);
		perror("Error openning file");
		return -1;
	}
	else {
		char 	* path_tmp 	= frame_get_absolute_tmp_path (f, "tmp");
		FILE 	* file_tmp 	= NULL;
		file_tmp = fopen (path_tmp, "w+");
		if (file_tmp == NULL) { 
			fclose (file);
			perror("Error openning file");
			return -1;
		}
		g_free (path_tmp);
	
		size_t 	size 		= 256;
		char 	* line 		= NULL;

		re = pcre_compile (pattern, 0, &error, &erroffset, NULL);  
		if (re == NULL) {
			  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
			  return -1;
		}
		else {
			int loop_nb 	= 1;
			int found_flag 	= 0;
		
			while (found_flag == 0 && getline (&line, &size, file) != -1) {
				rc = pcre_exec(re, NULL, line, (int)strlen(line), 0, 0, ovector, OVECCOUNT);    

				if (rc > 0) {
					// If the new rule will erase a old one, choose one of them
					found_flag = 1;
					printf ("A rule already exists for the activity and sensors you selected.\n");
					do {
						printf ("Replace old rule by new one (y/n)? ");	
						getline (&answer, &size, stdin);
					} while (answer[0] != 'y' && answer[0] != 'n');

					if (answer[0] == 'y') { 
						if (index != 0) {
							fprintf (file_tmp, "%i%s%i\n", loop_nb, pattern, delta);
						}
						else {
							fprintf (file_tmp, "%s%i\n", pattern, delta);
						}
					}
					else {
						fprintf (file_tmp, "%s", line);
					}
				}
				else {
					fprintf (file_tmp, "%s", line);
				}

				loop_nb++;
			}
			
			if (found_flag != 0) {
				// If the new rule erased an old one, write the remaining lines
				while (getline (&line, &size, file) != -1) {
					fprintf (file_tmp, "%s", line);
				}		
				
				if (line != NULL)
					g_free (line);
				fflush (file_tmp);
				fclose (file);
				file = fopen (path, "w+");
				fseek(file_tmp, 0, SEEK_SET);
				char * line2 = NULL;
		
				while (getline (&line2, &size, file_tmp) != -1) {
					fprintf(file, "%s", line2);
				}

				g_free (line2);
			}
			else {
				// If the new rule didn't erase an old one, write it at the end of file
				if (index != 0) {
					fprintf(file, "%i%s%i\n", index, pattern, delta);
				}
				else {
					fprintf(file, "%s%i\n", pattern, delta);
				}
			}
			pcre_free (re);

		}
		fclose (file_tmp);
		fclose (file);
	}

	g_free (answer);

	return 0;
}


char * is_sensor_activated (frame_t * f, char * sensor)
{
	data_t 		* data 		= (data_t *) f->priv;
	GSList 		* tmp 		= g_slist_copy (data->Sensors_List);
	// Go through th sensor list to find sensor
	while (tmp != NULL) {
		if (g_strstr_len (sensor, -1, tmp->data) != NULL) {
			sensor = g_strdup (tmp->data);
			return sensor;
		}
		else {
			tmp = g_slist_next (tmp);
		}
	}

	return NULL;
}
