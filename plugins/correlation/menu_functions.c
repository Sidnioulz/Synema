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
 * \file menu_functions.c
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
#include "menu_functions.h"
#include "correlation.h"
#include "alerts_map.h"
#include "editor.h"
#include "init_functions.h"

GList *build_func_list ()
{

	GList *list = NULL;
	GList *children_list = NULL;

	func_list_add_new_func (&list, "Reload", reload_activities);

	func_list_add_new_separator (&list);

	func_list_add_new_func (&children_list, "Debug", debug_menu);	
	func_list_add_new_func (&children_list, "Editor", rules_editor_menu);
	func_list_add_new_submenu (&list, "Rules editor", children_list);


	func_list_add_new_separator (&list);

	children_list = NULL;
	func_list_add_new_func (&children_list, "Sequences criticality", sequences_criticality);
	func_list_add_new_func (&children_list, "Sequences number", sequences_number);
	func_list_add_new_func (&children_list, "Activities number", activities_number);
	func_list_add_new_func (&children_list, "Criticality", criticality_matrix);
	func_list_add_new_func (&children_list, "All", all_matrix);
	func_list_add_new_submenu (&list, "Matrix", children_list);

	func_list_add_new_separator (&list);

	func_list_add_new_func (&list, "Time correlation", time_correlation);
	func_list_add_new_func (&list, "Sequences", sequences);
	func_list_add_new_func (&list, "Bars", correlation_sensor_bars);
	
	func_list_add_new_separator (&list);


	children_list = NULL;
	func_list_add_new_func (&children_list, "Syslog", text_syslog);
	func_list_add_new_func (&children_list, "Snort", text_snort);
	func_list_add_new_func (&children_list, "SELinux", text_selinux);
	func_list_add_new_func (&children_list, "Osiris", text_osiris);
	func_list_add_new_submenu (&list, "Text Logs", children_list);

	return list;
}


GList *build_time_periods ()
{
	GList *list = NULL;
	time_period_t *p1, *p2, *p3, *p4, *p5;

	p1 = time_period_new (MONTH, 1);
	list = g_list_prepend (list, p1);

	p2 = time_period_new (WEEK, 1);
	list = g_list_prepend (list, p2);

	p3 = time_period_new (DAY, 1);
	list = g_list_prepend (list, p3);

	p5 = time_period_new (HOUR, 1);
	list = g_list_prepend (list, p5);

	p4 = time_period_new (LIVE, 0);
	list = g_list_prepend (list, p4);

	return list;
}





void sequences (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;

	data->graph_type = g_strdup ("sequences_mapping");
	
	frame_request_content_refresh (f);
	frame_request_redraw (f);
}



void time_correlation (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;

	data->graph_type = g_strdup ("time_correlation");
	
	frame_request_content_refresh (f);
	frame_request_redraw (f);
}

void correlation_sensor_bars (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;

	data->graph_type = g_strdup ("bars");

	name = g_strdup_printf ("bar_%s", correlation_get_period_name(f->display_period.type));
	data->ploticus_script = strdup (name);

	g_free(name);
//	frame_request_content_refresh (f);
	frame_request_redraw (f);
}


void text_osiris (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;

	data->graph_type = g_strdup ("text_osiris");	
	data->line_number_offset = 0;
	data->osiris_file_nb = 1;
	
	 // Get the latest file in osiris logs directory	
	char * path = NULL;
	path = g_strdup_printf ("%s/%s/%s/%s", MACHINES_DIR
						, machine_get_folder_name (f->display_machine)
						, LOGS_DIR
						, "osiris");
	DIR * directory = opendir (path);
	struct dirent * dir_struct = NULL;
	dir_struct = readdir (directory);
	while ( (dir_struct != NULL) ) {
		if ( (g_strcmp0 (dir_struct->d_name, "flagLog.txt") != 0) && atoi(dir_struct->d_name) > data->osiris_file_nb) {
			data->osiris_file_nb = atoi(dir_struct->d_name);
		}
		dir_struct = readdir (directory);
	}
	closedir(directory);
	g_free(path);
	
	frame_request_redraw (f);
}


void text_selinux (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;

	data->graph_type = g_strdup ("text_selinux");	
	data->line_number_offset = 0;
	data->osiris_file_nb = 1;
		
	frame_request_redraw (f);
}


void text_snort (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;

	data->graph_type = g_strdup ("text_snort");	
	data->line_number_offset = 0;
	data->osiris_file_nb = 1;
		
	frame_request_redraw (f);
}


void text_syslog (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;

	data->graph_type = g_strdup ("text_syslog");	
	data->line_number_offset = 0;
	data->osiris_file_nb = 1;
		
	frame_request_redraw (f);
}



void rules_editor_menu (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;

	data->graph_type = g_strdup ("editor");	
	data->line_number_offset = 0;
	
	frame_request_redraw (f);
}


void reload_activities (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;

	free_activities_list (f);


	data->line_number_offset = 0;
	data->osiris_file_nb = 1;
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

	frame_request_content_refresh (f);
	frame_request_redraw (f);
}


void debug_menu (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;


	data->graph_type = g_strdup ("text_debug");	
	
	frame_request_redraw (f);
}

void criticality_matrix (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;


	data->graph_type = g_strdup ("matrix_criticality");	
	
	frame_request_redraw (f);
}


void sequences_number (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;

	data->graph_type = g_strdup ("matrix_sequences");	

	free_activities_list (f);

	if ( machine_is_all_machines (f->display_machine)) {
		init_all_machines(f, NULL);
	}
	
	
	frame_request_redraw (f);
}

void activities_number (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;

	data->graph_type = g_strdup ("matrix_activities");	

	free_activities_list (f);

	if ( machine_is_all_machines (f->display_machine)) {
		init_all_machines(f, NULL);
	}
	
	
	frame_request_redraw (f);
}





void sequences_criticality (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;

	data->graph_type = g_strdup ("matrix_sequences_criticality");	

	free_activities_list (f);

	if ( machine_is_all_machines (f->display_machine)) {
		init_all_machines(f, NULL);
	}
	
	
	frame_request_redraw (f);
}


void all_matrix (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;


	data->graph_type = g_strdup ("matrix_all");	
	
	frame_request_redraw (f);
}


