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
#include "text_mode.h"

int time_correlation_display_alerts_map(frame_t *f, cairo_t *cr)
{
	data_t 		*data 		= (data_t *) f->priv;
	
	// Draw title and legend 
	cairo_set_source_rgb (cr, 255, 255, 255); 
	cairo_paint_with_alpha (cr, 1);
	print_title (f, cr, "Activities");
	legend_handler (f, cr, ACTIVITY_FILE_PATH);
	// Draw axis 
	time_t current_time = time(NULL);
	current_time -= data->time_period_offset * correlation_get_period (HOUR);
	correlation_draw_axis(f, cr, current_time, f->display_period.type);
	// Draw arrows to go backward and forward
	if (f->display_period.type != LIVE)
		draw_arrows_for_map (f, cr);

	// Draw activities, intra-acticities links, and inter-acticities links
	intra_activity_handler (f, cr, current_time);
	// Draw confidence plot
	draw_activities_confidence_plot (f, cr, current_time);
	
	return 0;
}


int get_sensor_number (frame_t * f, char * sensor) 
{
	data_t 			* data 		= (data_t *) f->priv;
	
	GSList 			* tmp 		= g_slist_copy (data->Sensors_List);
	int 			found_flag 	= 0;
	
	// Go through the sensors list
	while (tmp != NULL && found_flag == 0) {
		if (g_strcmp0 (sensor, tmp->data) == 0)
			found_flag = 1;
		else 
			tmp = g_slist_next (tmp);
	}
	
	// Return the sensor index
	int index = g_slist_index (data->Sensors_List, tmp->data);

	return index+1;
}


int select_activity_color (cairo_t *cr, char * color)
{
	if (g_strcmp0 (color, "red") == 0)
		cairo_set_source_rgb (cr, 1, 0, 0); 
	else if (g_strcmp0 (color, "green") == 0)
		cairo_set_source_rgb (cr, 0, 1, 0); 	
	else if (g_strcmp0 (color, "blue") == 0)
		cairo_set_source_rgb (cr, 0, 0, 1); 
	else if (g_strcmp0 (color, "grey") == 0)
		cairo_set_source_rgb (cr, (double)84/255, (double)84/255, (double)84/255); 
	else if (g_strcmp0 (color, "cyan") == 0)
		cairo_set_source_rgb (cr, 0, 1, 1); 
	else if (g_strcmp0 (color, "brown") == 0)
		cairo_set_source_rgb (cr, (double)165/255, (double)42/255, (double)42/255); 
	else if (g_strcmp0 (color, "orange") == 0) 
		cairo_set_source_rgb (cr, 1, (double)140/255, 0); 
	else if (g_strcmp0 (color, "pink") == 0)
		cairo_set_source_rgb (cr, 1, (double)20/255, (double)147/255); 
	else if (g_strcmp0 (color, "purple") == 0)
		cairo_set_source_rgb (cr, (double)160/255, (double)32/255, (double)240/255); 
	else if (g_strcmp0 (color, "yellow") == 0)
		cairo_set_source_rgb (cr, 1, 1, 0); 
	else if (g_strcmp0 (color, "black") == 0)
		cairo_set_source_rgb (cr, 0, 0, 0); 
	else
		cairo_set_source_rgb (cr, 0, 0, 0);

	return 0;
}

int intra_activity_handler (frame_t * f, cairo_t * cr, time_t current_time) 
{
	data_t 		*data 				= (data_t *) f->priv;

	Activity_info_t			* intra_act_info[256];
	int 				i = 0, j = 0, m = 1, k 	= 0;
	int				delta_t 		= 0;
	int				delta_tmp 		= 0;
	char				* color 		= NULL;
	GList 				* tmp 			= NULL;
	GList 				* tmp2 			= NULL;

	// Init deltas per activity and per sensors couple
	int 				***intra_act_delta_array = NULL;
	intra_act_delta_array = init_intra_activity_delta (f, intra_act_delta_array);
	
	for (i = 0 ; i < data->Number_Of_Activities ; i++) { 
		// For each activity
		color = g_strdup (data->colors_array[i]);
		tmp = g_list_copy(data->activity_list[i]);
		
		while ( tmp != NULL ) { 
			// For each activity occurrence, reset number of links pointing to the activity occurrence
			((Activity_t *)(tmp->data))->activity_links_number = 0;
			intra_act_info[0] = malloc (sizeof(Activity_info_t));
			intra_act_info[0]->sensor = ((Activity_t *)(tmp->data))->sensor;
			intra_act_info[0]->beginning_date = ((Activity_t *)(tmp->data))->beginning_date;
			m = 1;

			// Draw the activity occurrence
			draw_link(f, cr, current_time
					, ((Activity_t *)(tmp->data))->sensor
					, ((Activity_t *)(tmp->data))->beginning_date
					, ((Activity_t *)(tmp->data))->sensor
					, ((Activity_t *)(tmp->data))->end_date
					, i, i
					, color, NULL);
			
			
			tmp2 = g_list_next(tmp);
			// Store info about every activity instance who occurred in a time interval inferior to delta
			while ( tmp2 != NULL ) {
				delta_t = intra_act_delta_array[i][((Activity_t *)(tmp->data))->sensor-1][((Activity_t *)(tmp2->data))->sensor-1];
				delta_tmp = ((Activity_t *)(tmp2->data))->beginning_date - ((Activity_t *)(tmp->data))->beginning_date;

				if (delta_tmp < delta_t && delta_tmp > 0){
					// If the two activity occurrences are close enough (time difference < delta)
					// Store them in a structure with infos about the activity related occurences
					intra_act_info[m] = malloc (sizeof(Activity_info_t));
					intra_act_info[m]->sensor = ((Activity_t *)(tmp2->data))->sensor;
					intra_act_info[m]->beginning_date = ((Activity_t *)(tmp2->data))->beginning_date;
					// Increase number of links pointing to the second activity occurrence
					((Activity_t *)(tmp2->data))->activity_links_number++; 
					m++;
				}	
				tmp2 = g_list_next(tmp2);
			}
			m--;

			// Draw links between related occurences of a same activity
			k = 0;
			while (k < m ) {
				if (intra_act_info[k+1]->beginning_date - intra_act_info[k]->beginning_date 
						< intra_act_delta_array[0][intra_act_info[k]->sensor-1][intra_act_info[k+1]->sensor-1]) 
				{
					draw_link(f, cr, current_time
							, intra_act_info[k]->sensor
							, intra_act_info[k]->beginning_date
							, intra_act_info[k+1]->sensor
							, intra_act_info[k+1]->beginning_date
							, i, i
							, color, color);
					g_free (intra_act_info[k]);
						
				}
				k++;
			}
			g_free (intra_act_info[k]);
		
			j++;
			tmp = g_list_next(tmp);
		}
	}

	for (j=0 ; j < data->Number_Of_Activities ; j++) {
		for (i = 0 ; i < data->Number_Of_Sensors; i++) {
			free (intra_act_delta_array[j][i]);
		}
		free (intra_act_delta_array[j]);
	}	
	free (intra_act_delta_array);


	free (color);
	inter_activities_handler(f, cr, current_time); 

	return 0;
}


int draw_link (frame_t * f, cairo_t * cr, time_t current_time, int sensor_1, int time_1
								, int sensor_2, int time_2
								, int act_1, int act_2
								, char * color_1, char * color_2)
{
	data_t 		*data 			= (data_t *) f->priv;
	float 		y_pos_1 		= 0;
	float		y_pos_2 		= 0;

	y_pos_1 = ((0.1+(sensor_1*0.8)/(data->Number_Of_Sensors+1))*frame_get_height(f)); 
	y_pos_1 += ((double)act_1/(double)(data->Number_Of_Activities+1)) * (frame_get_height(f) / (2*data->Number_Of_Sensors + 1));
	y_pos_2 = ((0.1+(sensor_2*0.8)/(data->Number_Of_Sensors+1))*frame_get_height(f));
	y_pos_2 += ((double)act_2/(double)(data->Number_Of_Activities+1)) * (frame_get_height(f) / (2*data->Number_Of_Sensors + 1));

	// Get the time to use for the end of xaxis
	struct tm * end_time;
	end_time = localtime ((time_t*)&current_time);
	time_t end_time_t = 0;
	end_time = fill_time_struct_for_drawing (f, end_time);
	end_time_t = mktime (end_time);


	int period = correlation_get_period( f->display_period.type );
	float x_pos_1 = frame_get_width (f) - ( frame_get_width (f)* (0.04 + 0.80*(end_time_t - time_1)/period) );
	float x_pos_2 = frame_get_width (f) - ( frame_get_width (f)* (0.04 + 0.80*(end_time_t - time_2)/period) );

	// Draw activities
	if (color_2 == NULL) {
		cairo_set_line_width (cr, 5);
		if (x_pos_1 > 0.16*frame_get_width (f) && x_pos_1 < 0.96*frame_get_width (f) ){
			cairo_move_to (cr, x_pos_1, y_pos_1);	 
			select_activity_color (cr, color_1);
			if (time_2 == -1) {
				cairo_line_to (cr, x_pos_1+1, y_pos_1);
			}
			else
				cairo_line_to (cr, x_pos_2+1, y_pos_1);
			cairo_stroke (cr);
		}
		// If the activity begins before the beginning date
		else if (x_pos_2 > 0.16*frame_get_width (f) && x_pos_2 < 0.96*frame_get_width (f) ){
			cairo_move_to (cr, 0.16*frame_get_width (f), y_pos_1);	 
			select_activity_color (cr, color_1);
			cairo_line_to (cr, x_pos_2+1, y_pos_1);
			cairo_stroke (cr);
		}
	}

	// Draw link between two activities
	else if (x_pos_1 > 0.16*frame_get_width (f) && x_pos_2 > 0.16*frame_get_width (f) 
						&& x_pos_1 < 0.96*frame_get_width (f) 
						&& x_pos_2 < 0.96*frame_get_width (f)
						) {
		cairo_move_to (cr, x_pos_1, y_pos_1);	 		
 		cairo_set_line_width (cr, 1);
		if (g_strcmp0 (color_1, color_2) == 0) {
			select_activity_color (cr, color_1);
			cairo_line_to (cr, x_pos_2, y_pos_2);
			cairo_stroke (cr);
		}
		else {
			select_activity_color (cr, color_1);
			cairo_line_to (cr, (x_pos_1 + x_pos_2) / 2, (y_pos_1 + y_pos_2) / 2);
			cairo_stroke (cr);
			cairo_move_to (cr, (x_pos_1 + x_pos_2) / 2, (y_pos_1 + y_pos_2) / 2);	 	
			select_activity_color (cr, color_2);
			cairo_line_to (cr, x_pos_2, y_pos_2);
			cairo_stroke (cr);
		}
	}

	return 0;
}

int inter_activities_handler(frame_t * f, cairo_t * cr, time_t current_time)
{
	data_t 		*data 				= (data_t *) f->priv;
	char		* path			 	= g_strdup (INTER_ACTIVITY_RULES_FILE);
	FILE 		* inter_activities_file		= fopen (path, "r");
	free (path);

	int 		k 				= 0;
	int 		found_flag 			= 0;
	int 		inter_activities_rule_nb 	= 0;
	int 		first_activity 			= 0; 
	int 		second_activity 		= 0;
	int 		delta_t 			= 0;
	int 		delta_tmp 			= 0;

	char		* first_color 			= NULL;
	char		* second_color 			= NULL;
	char 		string[256];

	GList 		* tmp 				= NULL;
	GList 		* tmp2 				= NULL;

	while (fgets(string, 256, inter_activities_file)) {	
		// For each inter-activities rule, extract activities and delta
		inter_activities_rule_nb = 0;
		first_activity = 0; 
		second_activity = 0;
		delta_t = 0;
		sscanf(string, "%i %i %i %i", &inter_activities_rule_nb, &first_activity, &second_activity, &delta_t);

		tmp = g_list_copy(data->activity_list[first_activity-1]); 
		while ( tmp != NULL ) {
			// For each occurrence of the first activity
			k = 1;
			found_flag = 0;
			tmp2 = g_list_copy(data->activity_list[second_activity-1]); 
			first_color = g_strdup (data->colors_array[first_activity-1]);

			while ( (tmp2 != NULL) && (found_flag == 0) ) {
				// For each occurrence of the second activity
				second_color = g_strdup (data->colors_array[second_activity-1]);
				delta_tmp =  ((Activity_t *)(tmp2->data))->beginning_date - ((Activity_t *)(tmp->data))->end_date;
				if ((delta_tmp < delta_t && delta_tmp > 0)) {
					// If the second activity occurrence isn't more than delta seconds after the first activity occurrence
					// Draw a link between them
					draw_link (f, cr, current_time
								, ((Activity_t *)(tmp->data))->sensor
								, ((Activity_t *)(tmp->data))->end_date
								, ((Activity_t *)(tmp2->data))->sensor
								, ((Activity_t *)(tmp2->data))->beginning_date
								, first_activity-1, second_activity-1
								, first_color, second_color);
					// Increase number of links pointing to the second activity occurrence
					((Activity_t *)(tmp2->data))->activity_links_number++;

					found_flag = 1;
				}
				tmp2 = g_list_next(tmp2);
			}
			tmp = g_list_next(tmp);
		}
	}

	fclose (inter_activities_file);
	free (first_color);
	free (second_color);

	return 0;
}


gint sort_by_beginning_date (gconstpointer item1, gconstpointer item2) 
{
	int nb_1 = ((Activity_t *)item1)->beginning_date;
	int nb_2 = ((Activity_t *)item2)->beginning_date;
	return (nb_1 - nb_2);
}


int draw_legend (frame_t * f, cairo_t * cr, int activity, char * activity_name)
{
	double y_pos = frame_get_height (f)*0.05 * activity;

	cairo_set_font_size (cr, frame_get_height (f)*0.04);
	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

	cairo_move_to(cr, frame_get_width(f) * 0.6 , y_pos );
	cairo_show_text (cr, activity_name);

	return 0;
}


int legend_handler (frame_t * f, cairo_t * cr, char * path)
{
	data_t *data = (data_t *) f->priv;
	
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;

	char 		pattern[256];
	char 		string[256];
	
	char		activity_number[256];
	char		activity_name[256];
	int 		activity = 0;
	
	FILE 		* activity_type_file 			= fopen (path, "r");

	if (activity_type_file == NULL) { 
		perror("Error openning the activity type file\n");
		return -1;
	}

	sprintf (pattern, "([0-9]*) \"([^\"]*)\" ([^ ]*).* ");

	// For each activity
	while (fgets (string, 256, activity_type_file) != NULL) {
		re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
		if (re == NULL) {
			  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
			  return -1;
		}
		rc = pcre_exec(re, NULL, string, (int)strlen(string), 0, 0, ovector, OVECCOUNT);    
		if (rc > 0) {
			sprintf(activity_number, "%.*s", ovector[3] - ovector[2], string + ovector[2]);
			sprintf(activity_name, "%.*s", ovector[5] - ovector[4], string + ovector[4]);
			activity = atoi(activity_number);

			// If a new activity has been added since last initialisation of activities and sequences
			// re-parse the logs
			if (activity > data->Number_Of_Activities ){
				free_activities_list (f);
				init_activity_list(f, NULL, -1, NULL);
			}

			// Draw a legend item
			select_activity_color (cr, data->colors_array[activity-1]);
			draw_legend (f, cr, activity, activity_name);
		}
	}
	pcre_free (re);
	fclose (activity_type_file);

	return 0;
}




int display_error_log_file(frame_t *f, cairo_t *cr)
{
	int 		i		= 0;
	int 		j		= 0;
	char 		text[256];
	size_t 		length 		= 0;
	
	char 		* path				= frame_get_absolute_data_path (f, "error.log");
	FILE 		* error_log_file 		= fopen (path, "r");
	if (error_log_file == NULL) { 
		perror("Error openning the error log file\n");
		return -1;
	}

	// Print baxkground and title 
	cairo_set_source_rgb (cr, 255, 255, 255); 
	cairo_paint_with_alpha (cr, 1);
	print_title (f, cr, "Debug mode");

	// Read log file 
	if (error_log_file != NULL) {
		// Set the cursor at the correct position in the file
		j = set_file_cursor (f, path, NULL, error_log_file, 0);

		while ( (j > 0) && (i < NUMBER_OF_LINES) ) { // Read the specified number of lines, and display them
			fgets (text, 256, error_log_file);
			length = strlen (text);
			text[length-1] = '\0';			
			select_activity_color (cr, "black");

			move_to_next_line (f, cr, 2);
			show_text (f, cr, text);
			i++;
			j--;

		}
		fclose (error_log_file); 
	}
	else {
		printf("Error opening the log file %s\n", path);
	}

	g_free(path);
	draw_scroll(f, cr);

	return 0;
}


int draw_activities_confidence_plot (frame_t * f, cairo_t * cr, time_t current_time)
{
	data_t 			*data 		= (data_t *) f->priv;

	int 			i		= 0;
	GList 			* tmp 		= NULL;

	// For each activity occurence, draw a bar for confidence plot
	for (i = 0 ; i < data->Number_Of_Activities ; i++) {
		tmp = g_list_copy(data->activity_list[i]);
		while ( tmp != NULL ) {
			draw_curve (f, cr, current_time, ((Activity_t *)(tmp->data))->beginning_date
							, ((Activity_t *)(tmp->data))->activity_links_number);
			tmp = g_list_next (tmp);
		}
	}

	return 0;
}

int draw_curve (frame_t * f, cairo_t * cr, time_t current_time, time_t activity_time, int links_number)
{
	float 		y_pos_1 	= 0;
	
	y_pos_1 = 0.9*frame_get_height(f) - 3*links_number; 

	// Get the time to use for the end of xaxis
	struct tm * end_time;
	end_time = localtime ((time_t*)&current_time);
	time_t end_time_t = 0;
	end_time = fill_time_struct_for_drawing (f, end_time);
	end_time_t = mktime (end_time);

	int 	period 	= correlation_get_period(f->display_period.type );
	float 	x_pos 	= frame_get_width (f) - ( frame_get_width (f)* (0.04 + 0.80*(end_time_t - activity_time)/period) );

	cairo_set_line_width (cr, 1);

	// Draw a bar for confidence plot
	if (x_pos > 0.16*frame_get_width (f) && x_pos < 0.96*frame_get_width (f) ) {
		select_activity_color (cr, "black");
		cairo_move_to (cr, x_pos, 0.9*frame_get_height(f));
		cairo_line_to (cr, x_pos, y_pos_1);
	}	

	cairo_stroke (cr);

	return 0;
}




int *** init_intra_activity_delta (frame_t * f, int *** intra_act_delta_array)
{
	data_t 		*data 		= (data_t *) f->priv;

	char		* path				= frame_get_absolute_data_path (f, "error.log");
	FILE 		* error_log_file 		= fopen (path, "a+");
	if (error_log_file == NULL) { 
		perror("Error openning the error log file\n");
		return intra_act_delta_array;
	}

	pcre 				* re;
	const char 			* error;
	int 				erroffset;
	int 				ovector[OVECCOUNT];
	int 				rc;
	char 				pattern[256];
	char 				activity[256];

	int 				sensor1 		= 0;
	int 				sensor2 		= 0;
	int				activity_nb		= 0;
	int				i, j, k;
	char 				* line 			= NULL;
	char 				string[256];	
	char 				sensor_1[256];
	char 				sensor_2[256];
	char 				time_interval[256];

	char				* path2			= g_strdup (INTRA_ACTIVITY_RULES_FILE);
	FILE 				* intra_activity_file 	= fopen (path2, "r");
	free (path2);

	// Alloc the array	
	intra_act_delta_array = malloc (data->Number_Of_Sensors * sizeof(int*));
	for (j=0 ; j < data->Number_Of_Activities ; j++) {
		intra_act_delta_array[j] = malloc (data->Number_Of_Sensors * sizeof(int*));
		for (i = 0 ; i < data->Number_Of_Sensors; i++) {
			intra_act_delta_array[j][i] = malloc (data->Number_Of_Sensors * sizeof(int*));
			for (k = 0; k < data->Number_Of_Sensors; k++) {
				intra_act_delta_array[j][i][k] = 0;
			}
		}
	}
	sprintf (pattern, "([0-9]*) ([a-zA-Z*]*) ([a-zA-Z*]*) ([0-9]*)");
	re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
	if (re == NULL) {
		  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
		  return intra_act_delta_array;
	}

	fprintf(error_log_file, "\nIntra activity: \n");
	while(fgets (string, 256, intra_activity_file) != NULL) {
		// Extract delta for each intra-activity rule
		rc = pcre_exec(re, NULL, string, (int)strlen(string), 0, 0, ovector, OVECCOUNT);    
		if (rc > 0) {
			sprintf(activity, "%.*s", ovector[3] - ovector[2], string + ovector[2]);
			sprintf(sensor_1, "%.*s", ovector[5] - ovector[4], string + ovector[4]);
			sprintf(sensor_2, "%.*s", ovector[7] - ovector[6], string + ovector[6]);
			sprintf(time_interval, "%.*s", ovector[9] - ovector[8], string + ovector[8]);

			activity_nb = atoi (activity);
			// If second sensor is *, fill delta array for every sensor
			if (g_strcmp0 (sensor_2, "*") == 0) {
				sensor1 = get_sensor_number (f, sensor_1);			
				for (j = 0 ; j < data->Number_Of_Sensors; j++) {
					if (intra_act_delta_array[activity_nb - 1][sensor1-1][j] == 0) 
						intra_act_delta_array[activity_nb - 1][sensor1-1][j] = atoi (time_interval);
				}
				fprintf (error_log_file, "%i: sensor #%i -> sensor #%s: delta = %i\n", activity_nb
													, sensor1
													, sensor_2
													, atoi (time_interval));
			}
			// If second sensor isn't *, fill the corresponding array item
			else {
				sensor1 = get_sensor_number (f, sensor_1);
				sensor2 = get_sensor_number (f, sensor_2);
				intra_act_delta_array[activity_nb - 1][sensor1-1][sensor2-1] = atoi (time_interval);
				fprintf (error_log_file, "%i: sensor #%i -> sensor #%i: delta = %i\n", activity_nb
													, sensor1
													, sensor2
													, atoi (time_interval));
			}
		}
	}

	g_free (line);	
	fclose (intra_activity_file);
	fclose (error_log_file);

	return intra_act_delta_array;
}


struct tm * fill_time_struct_for_drawing (frame_t * f, struct tm * end_time)
{
	switch (f->display_period.type ) {
		case LIVE:
			end_time->tm_hour += 1;
			end_time->tm_min = 0;
			end_time->tm_sec = 0;
			break;
		case HOUR:
			end_time->tm_hour += 1;
			end_time->tm_min = 0;
			end_time->tm_sec = 0;
			break;
		case DAY:
			end_time->tm_mday += 1;
			end_time->tm_hour = 1;
			end_time->tm_min = 0;
			end_time->tm_sec = 0;
			break;
		case WEEK:
			end_time->tm_mday += 1;
			end_time->tm_hour = 0;
			end_time->tm_min = 0;
			end_time->tm_sec = 0;
			break;
		case MONTH:
			end_time->tm_mday += 1;
			end_time->tm_hour = 0;
			end_time->tm_min = 0;
			end_time->tm_sec = 0;
			break;
		default:
			end_time->tm_hour = 0;
			end_time->tm_min = 0;
			end_time->tm_sec = 0;
			break;
	}

	return end_time;
}



