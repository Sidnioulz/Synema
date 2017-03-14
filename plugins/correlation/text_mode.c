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
 * \file correlation.c
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
#include "editor.h"
#include "text_mode.h"

int display_latest_text_alerts(frame_t *f, cairo_t *cr, char * sensor)
{
	data_t 		*data 		= (data_t *) f->priv;
	double 		alert_level 	= 0.02;
	char		*path		= NULL; 
	FILE 		* file 		= NULL;
	char 		text2[256]; 
	char		*log_line 	= NULL;
	int 		i		= 0;
	int 		j		= 0;


	if ( g_strcmp0(sensor, "syslog_ssh") == 0 )
		sensor = g_strdup("syslog");
	
	// Get log file for sensor
	if (g_strcmp0(sensor, "selinux") == 0 ) {
		path = g_strdup_printf ("%s/%s/%s/%s/audit.log", MACHINES_DIR
								, machine_get_folder_name (f->display_machine)
								, LOGS_DIR
								, sensor);
	}
	else {
		if (g_strcmp0(sensor, "syslog") == 0 ) {
		path = g_strdup_printf ("%s/%s/%s/%s/auth.log", MACHINES_DIR
								, machine_get_folder_name (f->display_machine)
								, LOGS_DIR
								, "syslog_ssh");
		}
		else if (g_strcmp0(sensor, "osiris") == 0 ) {
			path = g_strdup_printf ("%s/%s/%s/%s/%i", MACHINES_DIR
								, machine_get_folder_name (f->display_machine)
								, LOGS_DIR
								, sensor
								, data->osiris_file_nb);
		}
		else if (g_strcmp0(sensor, "snort") == 0 ) {
			path = g_strdup_printf ("%s/%s/%s/%s/alert", MACHINES_DIR
								, machine_get_folder_name (f->display_machine)
								, LOGS_DIR
								, sensor);
		}
	}

	// Print title
	cairo_set_source_rgb (cr, 255, 255, 255); 
	cairo_paint_with_alpha (cr, 1);
	print_title (f, cr, sensor);	
	
	// Read log file 
	file = fopen(path, "r");
	if (file != NULL) {
	
		// If the sensor is osiris, look for the date at the beginning of the file
		char * osiris_log_date = NULL;
		if (g_strcmp0 (sensor, "osiris") == 0 ) {
			size_t size = 256;
			getline (&osiris_log_date, &size, file);
			fseek ( file, 0, SEEK_SET);
		}
	
	
		//Set the cursor at the position to read
		j = set_file_cursor (f, path, sensor, file, 0);

		pcre 		* re;
		const char 	* error;
		int 		erroffset;
		int 		ovector[OVECCOUNT];
		int 		rc;
		char 		pattern[256];
		char		field1[256], field2[256], field3[256], field4[256];
		while (j>0 && i< NUMBER_OF_LINES) { // Read the specified number of lines
			log_line = get_log(sensor, log_line, file);	
			// Look for parts of logs that will be displayed
			if (g_strcmp0(sensor, "selinux") == 0 ) {
				sprintf (pattern, "type=([A-Z_]*) msg=audit[(]([0-9]*)(?:(?:.*subj=([a-z_:]*))|(?:.*))");	
			}
			else if (g_strcmp0(sensor, "osiris") == 0 ) {
				sprintf (pattern, "compare time: ([a-zA-Z0-9 :]*)|[[][0-9]*[]][[][a-zA-Z0-9]*[]](.*)");
			}
			else if (g_strcmp0(sensor, "snort") == 0 ) {
				sprintf (pattern, "[]] ([^[]*) [[].*\n.*\n([0-9]*)/([0-9]*)-([0-9:]*)"); 
			}
			else if (g_strcmp0(sensor, "syslog") == 0 ) {
				sprintf (pattern, "([a-zA-Z]{3}[ ]*[0-9]*) ([0-9:]*) [^ ]* ([a-zA-Z0-9]*).*: (.*)");
			}

			re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
			if (re == NULL) {
				  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
				  return -1;
			}
		
			rc = pcre_exec(re, NULL, log_line, (int)strlen(log_line), 0, 0, ovector, OVECCOUNT);    

			if (rc > 0) {
				sprintf(field1, "%.*s", ovector[3] - ovector[2], log_line + ovector[2]);
				sprintf(field2, "%.*s", ovector[5] - ovector[4], log_line + ovector[4]);
				if (g_strcmp0 (sensor, "osiris") != 0) {
					sprintf(field3, "%.*s", ovector[7] - ovector[6], log_line + ovector[6]);
					if (g_strcmp0 (sensor, "syslog") == 0 || g_strcmp0 (sensor, "snort") == 0)
						sprintf(field4, "%.*s", ovector[9] - ovector[8], log_line + ovector[8]);
				}	


				if (g_strcmp0(sensor, "selinux") == 0 ) {
					struct tm * time_struct = malloc (sizeof (struct tm));
					int time_sec_value = atoi (field2);
					time_struct = gmtime((time_t*)&time_sec_value);
					time_struct->tm_year += 1900;
					time_struct->tm_mon += 1;

					sprintf(text2, "%i/%i/%i %i:%i:%i %s %s", time_struct->tm_mday
										, time_struct->tm_mon
										, time_struct->tm_year
										, time_struct->tm_hour
										, time_struct->tm_min
										, time_struct->tm_sec
										, field1, field3);
				}
				else if (g_strcmp0(sensor, "osiris") == 0 ) {
					sprintf(text2, "%s %s", field1, field2);
				}
				else if (g_strcmp0(sensor, "snort") == 0 ) {
					sprintf(text2, "%s/%s %s %s", field3, field2, field4, field1);
				}
				else if (g_strcmp0(sensor, "syslog") == 0 ) {
					sprintf(text2, "%s %s %s %s", field1, field2, field3, field4);
				}

				int * act_nb = NULL;
				act_nb = find_activity (sensor, log_line, 0);
				
				// Select activity color to display text
				if (act_nb[0] > data->Number_Of_Activities || act_nb[0] <= 0) 
					select_activity_color (cr, "black");
				else
					select_activity_color (cr, data->colors_array[act_nb[0] - 1]);

				// Log criticality and alert level
				if (act_nb[1] != -1) {
					alert_level += ((double)(act_nb[1]))/100;
				}

				g_free (act_nb);

				move_to_next_line (f, cr, 2);
				show_text (f, cr, text2);
				i++;
			}
				j--;
			g_free(log_line);
			pcre_free (re);
			
		}
		fclose(file); 
	
		// If sensor is osiris, print the log date 
		if (g_strcmp0 (sensor, "osiris") == 0 ) {
			cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
			cairo_move_to(cr, frame_get_width (f) * 0.25, frame_get_height (f)*0.042);
			char * osiris_date = g_strstr_len (osiris_log_date, -1, ":") + 2;
			osiris_date = g_strndup (osiris_date, strlen (osiris_date) - 1);
			cairo_show_text (cr, osiris_date);
			g_free (osiris_log_date);
			g_free (osiris_date);
		}
	
	}
	else {
		printf("Error opening the log file %s\n", path);
	}

	g_free(path);
	draw_alert_level_bar(f, cr, alert_level);
	draw_scroll(f, cr);

	return 0;
}




int count_alerts (char * path, char * sensor)
{
	FILE 		*file 		= NULL;
	int 		num_alerts 	= 0;
	char 		*line 		= NULL;

	file = fopen (path, "r");

	if (file != NULL) { 
		fseek(file, 0, SEEK_SET);
		char * buffer = NULL;
		while ( (line = get_log (sensor, line, file)) != NULL && (feof (file) == 0) ) { 
			buffer = g_strdup (line);
			if (g_strcmp0 (buffer, "\n") != 0) {
	        	       	num_alerts++;
        		}
        	}
        	g_free (buffer);
		g_free (line);
		fclose (file);
	}
	else {
		fprintf (stderr, "Error opening the log file %s\n", path);
	}

	return num_alerts;
}

 

void draw_alert_level_bar (frame_t *f, cairo_t *cr, double alert_level) 
{
	cairo_surface_t			*image			= NULL;
	char				*path_warning		= NULL;
	double				tmp;
	int				w,h;
	cairo_pattern_t 		*gradient		= NULL;

	if (alert_level > 1.0) {
		alert_level = 1.0;
	}
		
	// Setting the gradient overlay for the alert level bar
	gradient = cairo_pattern_create_linear(0,frame_get_height (f),frame_get_width (f),frame_get_height (f) );
	cairo_pattern_add_color_stop_rgb ( gradient, 0.0, 0.0, 1.0, 0.0); // green
	cairo_pattern_add_color_stop_rgb ( gradient, 0.5, 1.0, 1.0, 0.0); // yellow 
	cairo_pattern_add_color_stop_rgb ( gradient, 1.0, 1.0, 0.0, 0.0); // red
	cairo_rectangle (cr, 6, frame_get_height (f)*0.95 , alert_level*frame_get_width (f) - 12, frame_get_height(f)*0.05);
	cairo_set_source(cr,gradient);
	cairo_fill (cr);
	cairo_set_source_rgb (cr, 0, 0, 0); 
	cairo_stroke (cr);
	
	// When the alert level is very high, display a warning message
	if (alert_level >= 0.9) {
		path_warning = frame_get_absolute_data_path (f, "dialog-warning.png");
		image = cairo_image_surface_create_from_png (path_warning);
		w = cairo_image_surface_get_width (image);
		h = cairo_image_surface_get_height (image);
		tmp = frame_get_height (f) / h;
	
		cairo_save (cr);
		cairo_scale (cr, tmp, tmp);
		cairo_set_source_surface (cr, image, 0, 0);
		cairo_paint_with_alpha(cr, 0.3);
		
		cairo_surface_destroy (image);
		cairo_restore (cr);	
	}
	g_free(path_warning);
	cairo_pattern_destroy (gradient);
}


void draw_scroll (frame_t *f, cairo_t *cr) 
{
	char				*path_scroll		= NULL;
	cairo_surface_t			*image			= NULL;
	double				tmp = 0;	
	int				w,h;

	path_scroll = frame_get_absolute_data_path (f, "scroll.png");
	image = cairo_image_surface_create_from_png (path_scroll);
	w = cairo_image_surface_get_width (image);
	h = cairo_image_surface_get_height (image);
	tmp = frame_get_height (f) / h;
	cairo_save (cr);
	cairo_scale (cr, tmp, tmp);
	cairo_set_source_surface (cr, image, 0, 0);
	cairo_paint (cr);
	cairo_surface_destroy (image);
	g_free(path_scroll);
	cairo_restore (cr);
}


char * get_log (char * sensor, char * log_line, FILE * log_file)
{
	log_line = g_strdup(NULL);
	if (sensor != NULL && strcmp(sensor, "snort") == 0) { 
		char 	tmp[512];
		fgets(tmp, 512, log_file);
		if ( g_strcmp0(tmp, "\n") == 0)  
			fgets(tmp, 512, log_file);
			
		log_line = g_strdup(tmp);
			
		while ( (fgets(tmp, 512, log_file) != NULL) && (g_strcmp0(tmp, "\n") != 0) ) {
			log_line = g_strconcat (log_line, tmp, NULL);
		}
	}
	else {
		char tmp[512];
		fgets (tmp, 512, log_file);
		log_line = g_strdup (tmp);
	}

	if ((log_line != NULL) && strcmp(log_line, "\0") != 0)
		return log_line; 
	else 
		return NULL;
}

int move_to_next_line (frame_t *f, cairo_t * cr, double x) 
{
	double current_x;
	double current_y;
	cairo_get_current_point (cr, &current_x, &current_y);
	cairo_rel_move_to (cr, -(current_x - x), 0.052 * frame_get_height (f));

	return 0;
}

int show_text (frame_t *f, cairo_t * cr, char * text) 
{
	char * text_tmp = NULL;
	char * text_ptr = &text[0];
	double current_x;
	double current_y;
	int i = 0;
	
	text_tmp = g_strndup (text_ptr, 50);
	cairo_get_current_point (cr, &current_x, &current_y);
	if (current_y > 0.95 * frame_get_height (f)) {
		return 0;
	}
	cairo_show_text (cr, text_tmp);

	// If text needs more than one line to be displayed, go to next line when the end of the frame is reached
	while (50 + 45*i < strlen(text)) {
		move_to_next_line (f, cr, 8);
		cairo_get_current_point (cr, &current_x, &current_y);
		if (current_y > 0.95 * frame_get_height (f)) {
			return 0;
		}
		char * text_ptr = &text[50 + 45*i];
		text_tmp = g_strndup (text_ptr, 45);		
		cairo_show_text (cr, text_tmp);
		i++;
	}
				
	return 0;
}


int set_file_cursor (frame_t * f, char * path, char * sensor, FILE * file, int lines_already_read) 
{
	data_t 		*data 		= (data_t *) f->priv;
	char		*log_line 	= NULL;
	int 		alerts_number 	= count_alerts(path, sensor);
	int 		lines_to_read 	= alerts_number - (NUMBER_OF_LINES + data->line_number_offset - lines_already_read);
	int 		j;
	
	if ( lines_to_read > 0) {
		j = 0;
		while (j < lines_to_read) {
			log_line = get_log(sensor, log_line, file);
			j++;
		}
		j= NUMBER_OF_LINES + data->line_number_offset;
	}		
	else {
		rewind(file);
		j = alerts_number;
	}

	g_free(log_line);
	
	return j;
}




