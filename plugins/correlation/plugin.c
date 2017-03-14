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
 * \file plugin.c
 * \brief Correlation plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 *
 * This file contains the source code for the Correlation plugin.
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
#include "init_functions.h"
#include "sequences_map.h"
#include "matrix.h"
#include "text_mode.h"

char *get_display_name (frame_t *f)
{
	return "Correlation";
}



int is_refreshing_needed (frame_t *f)
{
	int refreshing_needed = !(rand() % 2);
	return refreshing_needed; 
	//return 1;
}



int refresh_content (frame_t *f)
{
	data_t 		*data 			= (data_t *) f->priv;	
	
	char 		*tmppath 		= frame_get_absolute_tmp_path (f, "correlation_plot.svg");
	char		*file_name		= g_strdup_printf ("report");
	char		*path 			= NULL;
	char		*command		= NULL;
	char 		*script		 	= NULL;
	char 		*reportsdir 		= NULL;
	
	if (f->display_period.type == LIVE)
		data->time_period_offset = 0;		
			
	if(g_strcmp0(data->graph_type,"bars") == 0 && (!machine_is_all_machines (f->display_machine)) && data->refreshing_is_allowed == 1) {
		path 			= frame_get_absolute_reports_path (f, file_name, NULL);
		script		 	= frame_get_absolute_data_path (f, data->ploticus_script);
		reportsdir 		= frame_get_absolute_reports_path (f, NULL, NULL);
	
		time_t current_time = time(NULL);
		current_time -= data->time_period_offset * correlation_get_period (HOUR);
		struct tm * end_time;
		end_time = localtime ((time_t*)&current_time);
	
		
		char * day = NULL, * month = NULL, * hour = NULL, * min = NULL;
		
		if (end_time->tm_mday < 10)
			day = g_strdup_printf("0%i", end_time->tm_mday);
		else 
			day = g_strdup_printf("%i", end_time->tm_mday);

		if (end_time->tm_mon < 9)
			month = g_strdup_printf("0%i", end_time->tm_mon + 1);
		else 
			month = g_strdup_printf("%i", end_time->tm_mon + 1);

		if (end_time->tm_hour < 10)
			hour = g_strdup_printf("0%i", end_time->tm_hour);
		else 
			hour = g_strdup_printf("%i", end_time->tm_hour);

		if (end_time->tm_min < 10)
			min = g_strdup_printf("0%i", end_time->tm_min);
		else 
			min = g_strdup_printf("%i", end_time->tm_min);

		command = g_strdup_printf ("ploticus '%s' -dir '%s' -svg TODAY=%s/%s/%i CURRENT_TIME=%s:00 -o '%s'"
						, script
						, reportsdir
						, day
						, month
						, end_time->tm_year + 1900
						, hour
						, tmppath);

		system (command);
		
		g_free (day);
		g_free (month);
		g_free (hour);
		g_free (min);
	}

	g_free (file_name);
	g_free (tmppath);
	g_free (command);
	g_free (script);
	g_free (reportsdir);
	g_free (path);

	frame_request_redraw (f);
	return 0;
}

int draw_area (frame_t *f, cairo_t *cr)
{
	char 				*path		= NULL;
	data_t 				*data 		= (data_t *) f->priv;
	GError				*err		= NULL;
	RsvgHandle			*myhandle	= NULL;	

	if (g_strcmp0(data->graph_type,"editor") == 0 ) {
		cairo_set_source_rgb(cr, 1, 1, 1);
		cairo_paint(cr);		
		cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);	
		cairo_set_font_size (cr, frame_get_height (f)*0.07);
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		cairo_move_to(cr, frame_get_width (f)*0.30, frame_get_height (f)*0.35);
		cairo_show_text (cr, "Rules Editor");
		cairo_stroke (cr);
		rules_editor(f, 0);	
	} 
	else if (!machine_is_all_machines (f->display_machine)) {
		refresh_activity_list (f, cr);
		if (g_strcmp0(data->graph_type,"sequences_mapping") == 0 ) {
				display_sequences_map(f, cr);	
		} 
		else if (g_strcmp0(data->graph_type,"time_correlation") == 0 ) {
				time_correlation_display_alerts_map(f, cr);
		} 
		else if (g_strstr_len(data->graph_type, 10, "text") != NULL) {
			if (g_strstr_len(data->graph_type, -1, "selinux") != NULL)
				display_latest_text_alerts(f, cr, "selinux");
			else if (g_strstr_len(data->graph_type, -1, "osiris") != NULL)
				display_latest_text_alerts(f, cr, "osiris");
			else if (g_strstr_len(data->graph_type, -1, "syslog") != NULL)
				display_latest_text_alerts(f, cr, "syslog_ssh");
			else if (g_strstr_len(data->graph_type, -1, "snort") != NULL)
				display_latest_text_alerts(f, cr, "snort");
			else if(g_strcmp0(data->graph_type,"text_debug") == 0 ) {
				display_error_log_file(f, cr);
			}
		}
		else if (g_strstr_len(data->graph_type, 10, "bar") != NULL) {
			cairo_set_source_rgb(cr, 1, 1, 1);
			cairo_paint(cr);		
			
			path = frame_get_absolute_tmp_path (f, "correlation_plot.svg");
			myhandle = rsvg_handle_new_from_file (path, &err);

			g_free (path);
			if (err) { 
				g_clear_error (&err);
			}
			else {
				cairo_save (cr); 
				cairo_scale (cr, frame_get_width (f)*0.5, frame_get_height (f)*0.85);			
				cairo_translate (cr, 0.45, 0.13);
				cairo_rotate (cr, M_PI);
				rsvg_handle_render_cairo (myhandle, cr);
				cairo_restore (cr);
				g_object_unref (myhandle);
			}
			
			print_title (f, cr, "Alerts Criticality");
			if (f->display_period.type != LIVE)
				draw_arrows_for_map (f, cr);

		}	
		else {
			cairo_set_source_rgb(cr, 1, 1, 1);
			cairo_paint(cr);		
			cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);	
			cairo_set_font_size (cr, frame_get_height (f)*0.07);
			cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
			cairo_set_font_size (cr, frame_get_height (f)*0.045);
			cairo_move_to(cr, frame_get_width (f)*0.15, frame_get_height (f)*0.40);
			cairo_show_text (cr, "This mode is only available for \"All Machines\"");
			cairo_stroke (cr);
		}

	}
	else {
		if (g_strstr_len(data->graph_type, 10, "matrix") != NULL) {
			draw_matrix (f, cr);	
		}
		else {
			cairo_set_source_rgb(cr, 1, 1, 1);
			cairo_paint(cr);		
			cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);	
			cairo_set_font_size (cr, frame_get_height (f)*0.07);
			cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
			cairo_set_font_size (cr, frame_get_height (f)*0.045);
			cairo_move_to(cr, frame_get_width (f)*0.15, frame_get_height (f)*0.40);
			cairo_show_text (cr, "This mode is not available for \"All Machines\"");
			cairo_stroke (cr);
		}
	}
				
	return 0;
}


int button_press_handler (frame_t *f, GdkEventButton *event, cairo_t *cr)
{
	data_t 	*data		= 	(data_t *) f->priv;

	if ( g_strstr_len(data->graph_type, 10, "text") != NULL && !(machine_is_all_machines (f->display_machine)) ) {	
		char		*path		= NULL; 
		char 		* sensor	= NULL;
				
		if (g_strstr_len(data->graph_type, -1, "selinux") != NULL) {
			sensor = g_strdup ("selinux");
			path = g_strdup_printf ("%s/%s/%s/%s/audit.log", MACHINES_DIR
								, machine_get_folder_name (f->display_machine)
								, LOGS_DIR
								, sensor);
		}
		else {
			if (g_strstr_len(data->graph_type, -1, "syslog") != NULL) {
				sensor = g_strdup("syslog_ssh");
				path = g_strdup_printf ("%s/%s/%s/%s/auth.log", MACHINES_DIR
									, machine_get_folder_name (f->display_machine)
									, LOGS_DIR
									, sensor);
			}
			else if (g_strstr_len(data->graph_type, -1, "osiris") != NULL) {
				sensor = g_strdup("osiris");
				path = g_strdup_printf ("%s/%s/%s/%s/%i", MACHINES_DIR
									, machine_get_folder_name (f->display_machine)
									, LOGS_DIR
									, sensor
									, data->osiris_file_nb);
			}
			else if (g_strstr_len(data->graph_type, -1, "snort") != NULL) {
				sensor = g_strdup("snort");
				path = g_strdup_printf ("%s/%s/%s/%s/alert", MACHINES_DIR
									, machine_get_folder_name (f->display_machine)
									, LOGS_DIR
									, sensor);
			}
			else if (g_strstr_len(data->graph_type, -1, "debug") != NULL) {
				path = frame_get_absolute_data_path (f, "error.log");
			}
		}
		
		int alerts_number = count_alerts(path, sensor);
		


		if (event->x > frame_get_width (f)*0.9 && event->y > frame_get_height (f)*0.9) { 
			//scrolling down page by page
			if (data->line_number_offset >= 0) {
				data->line_number_offset -= NUMBER_OF_LINES;
			}
			else if (g_strcmp0 (sensor, "osiris") == 0) {

				data->osiris_file_nb++;
				struct stat file;
				path = g_strdup_printf ("%s/%s/%s/%s/%i", MACHINES_DIR
								, machine_get_folder_name (f->display_machine)
								, LOGS_DIR
								, sensor
								, data->osiris_file_nb);
				int ret = stat(path, &file);
				if (ret == 0) {	
					data->line_number_offset = alerts_number;
				}
				else {
					data->osiris_file_nb--;
				}
			}
			
		}
		else if (event->x > frame_get_width (f)*0.9 && event->y < frame_get_height (f)*0.1) {
			//scrolling up page by page
			if (data->line_number_offset < alerts_number - NUMBER_OF_LINES ){ 
				data->line_number_offset += NUMBER_OF_LINES;
			}
			else if (g_strcmp0 (sensor, "osiris") == 0  && data->osiris_file_nb > 1) {
				data->line_number_offset = 0;
				data->osiris_file_nb--;
			}
		}
		else if (event->x > frame_get_width (f)*0.9 && event->y < frame_get_height (f)*0.87 && event->y > frame_get_height (f)*0.82) {
			//scrolling down line by line
			if (data->line_number_offset > -NUMBER_OF_LINES + 1 ) {
				data->line_number_offset -= 1;
			}
		}
		else if (event->x > frame_get_width (f)*0.9 && event->y > frame_get_height (f)*0.13 && event->y < frame_get_height (f)*0.18) {
			if (data->line_number_offset < alerts_number - NUMBER_OF_LINES ){ 
				data->line_number_offset += 1;
			}
		}

		g_free (path);
		g_free (sensor);
	}
	
	else if ( (g_strcmp0(data->graph_type,"time_correlation") == 0 	|| g_strcmp0(data->graph_type,"bars") == 0
									|| g_strcmp0(data->graph_type,"sequences_mapping") == 0) 
									&& !(machine_is_all_machines (f->display_machine))
									&& (f->display_period.type != LIVE) ) {	
		if (event->x < frame_get_width (f)*0.1 && event->y > frame_get_height (f)*0.9) { 
			//Go backward in time
			if (f->display_period.type == HOUR)
				data->time_period_offset += 1;
			else if (f->display_period.type == DAY)
				data->time_period_offset += 24;
			else if (f->display_period.type == WEEK)
				data->time_period_offset += 7*24;
			else if (f->display_period.type == MONTH)
				data->time_period_offset += 30*24;
		}
		else if (event->x > frame_get_width (f)*0.9 && event->y > frame_get_height (f)*0.9) { 
			//Go forward in time
			if (data->time_period_offset > 0) {
				if (f->display_period.type == HOUR)
					data->time_period_offset -= 1;
				else if (f->display_period.type == DAY)
					data->time_period_offset -= 24;
				else if (f->display_period.type == WEEK)
					data->time_period_offset -= 7*24;
				else if (f->display_period.type == MONTH)
					data->time_period_offset -= 30*24;
				if (data->time_period_offset < 0)
					data->time_period_offset = 0;
			}
		}
	}

	frame_request_content_refresh (f);
	frame_request_redraw (f);

	return 0;
}


int button_release_handler (frame_t *f, GdkEventButton *event, cairo_t *cr)
{
	data_t 	*data		= 	(data_t *) f->priv;

	if ( (g_strcmp0(data->graph_type,"sequences_mapping") == 0 ) && !(machine_is_all_machines (f->display_machine)) ) {
		if (event->x < frame_get_width (f)*0.2 && event->y < frame_get_height (f)*0.9 && event->y > frame_get_height (f)*0.1) { 
			if (data->display_axis_legend == 0)
				data->display_axis_legend = 1;
			else
				data->display_axis_legend = 0;
		}
	}
	else if ( (machine_is_all_machines (f->display_machine)) ) {
		if (event->x < frame_get_width (f)*0.1 && event->y < frame_get_height (f)*0.9 && event->y > frame_get_height (f)*0.1) { 
			if (data->display_axis_legend == 0)
				data->display_axis_legend = 1;
			else
				data->display_axis_legend = 0;
		}
	}

	frame_request_content_refresh (f);
	frame_request_redraw (f);
		
	return 0;
}


int init_private (frame_t *f)
{
	f->priv = malloc (sizeof (data_t));
	data_t *data = (data_t *) f->priv;
	char 		*name 		= NULL;

	data->line_number_offset = 0;
	data->osiris_file_nb = 1;
	data->refreshing_is_allowed = 1;

	data->time_period_offset = 0;
	data->display_axis_legend = 0;

	name = g_strdup_printf ("bar_%s", correlation_get_period_name(f->display_period.type));
	data->ploticus_script = strdup (name);


	if ( !machine_is_all_machines (f->display_machine)) {
		data->graph_type = g_strdup ("bars");
		init_activity_list(f, NULL, -1, NULL);
	}
	else {
		data->graph_type = g_strdup ("matrix_all");
		init_all_machines(f, NULL);
	}
	g_free(name);
	frame_request_redraw (f);	
	
	return 0;
}


int free_private (frame_t *f)
{
	data_t 		*data 		= (data_t *) f->priv;
	
	g_free(data->graph_type);
	g_free(data->ploticus_script);

	free_activities_list (f);

	free (data);

	return 0;
}

int restore_private(frame_t *f, GKeyFile *file)
{
	data_t 	*data = (data_t *) f->priv;
	GError 	*err = NULL;

	data->ploticus_script = g_key_file_get_string (file, f->get_display_name (f), "Ploticus Script", &err);
	if (err) {
		fprintf (stderr, "Could not load the ploticus script name (%s)\n", err->message);
		g_clear_error (&err);
	}

	data->graph_type = g_key_file_get_string (file, f->get_display_name (f), "Graph type", &err);
	if (err) {
		fprintf (stderr, "Could not load the graphic type (%s)\n", err->message);
		g_clear_error (&err);
	}


	return 0;
}

int save_private(frame_t *f, GKeyFile *file)
{
	data_t 		*data 		= (data_t *) f->priv;

	g_key_file_set_string (file, f->get_display_name (f), "Ploticus Script" ,data->ploticus_script);
	g_key_file_set_string (file, f->get_display_name (f), "Graph type" ,data->graph_type );

	return 0;
}


int display_machine_changed (frame_t *f)
{	
	data_t 		*data 		= (data_t *) f->priv;

	free_activities_list (f);
	if ( !machine_is_all_machines (f->display_machine)) {
		data->graph_type = g_strdup ("bars");
		init_activity_list(f, NULL, -1, NULL);
	}
	else {
		data->graph_type = g_strdup ("matrix_all");
		init_all_machines(f, NULL);
	}
	frame_request_content_refresh (f);

	frame_request_redraw (f);

	return 0;
}


int time_period_changed (frame_t *f)
{	
	data_t 		*data 		= (data_t *) f->priv;
	char 		*name 		= NULL;

	name = g_strdup_printf ("bar_%s", correlation_get_period_name(f->display_period.type));
	data->ploticus_script = strdup (name);

	g_free(name);	

	frame_request_content_refresh (f);
	frame_request_redraw (f);
	return 0;
}


int frame_size_changed (frame_t *f)
{
	frame_request_redraw (f);

	return 0;
}

