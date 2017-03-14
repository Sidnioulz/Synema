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
 * \file matrix.c
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
#include "matrix.h"



int draw_matrix (frame_t * f, cairo_t * cr)
{
	data_t 			* data 		= (data_t *) f->priv;
	int 			machine_number 	= 0;

	GList 			* first_part_ip_list = NULL;

	// Get machines list
	GList 	*list = machine_get_list_for_plugin (f->type, FALSE);
	if (list == NULL) {
		return -1;
	}
	machine_number = g_list_length (list) - 1;

	int * number_of_fields = malloc (2*sizeof (int));
	number_of_fields[0] = 0;
	number_of_fields[1] = 0;

	// Print background
	cairo_set_source_rgb (cr, 255, 255, 255); 
	cairo_paint_with_alpha (cr, 1);
	cairo_stroke(cr);
	cairo_set_source_rgb (cr, 0, 0, 0); 

	// Get matrix size (number of colums (number_of_fields[0]) and of rows (number_of_fields[1]))
	if (g_strcmp0 (data->graph_type, "matrix_all") == 0) {
		number_of_fields[0] = machine_number;
		number_of_fields[1] = NUMBER_OF_CRITERIONS;
	}
	else {
		number_of_fields[0] = 256;
		char * ip_end = NULL;
		
		// Find number of ip beginnings to know number of rows. 
		GList * iter = list;
		GList * tmp = NULL;
		int found_flag = 0;
		do {
			if (!machine_is_all_machines (iter->data)) {
				ip_end = g_strrstr (machine_get_folder_name (iter->data), ".");
				char * ip_first_part = g_strndup (machine_get_folder_name (iter->data), ip_end - machine_get_folder_name (iter->data));
			
				tmp = g_list_copy (first_part_ip_list);
				found_flag = 0;
				while (tmp != NULL && found_flag == 0) {
					if (g_strcmp0 (tmp->data, ip_first_part) == 0) 
						found_flag = 1;
					tmp = g_list_next(tmp);
				}
				
				if (found_flag == 0) {
					first_part_ip_list = g_list_prepend (first_part_ip_list, ip_first_part);
				}
				else {
					g_free (ip_first_part);
				}
			}
		} while ((iter = g_list_next (iter)) != NULL);

		number_of_fields[1] = g_list_length (first_part_ip_list); 
	}

	// Draw matrix axis and stubs
	draw_matrix_axis (f, cr, list, first_part_ip_list, number_of_fields);


	// Draw matrix graph
	if (g_strcmp0 (data->graph_type, "matrix_all") == 0) {
		matrix_all_handler (f, cr, number_of_fields, list);
	}
	else {
		matrix_one_criterion_handler (f, cr, number_of_fields, list, first_part_ip_list);
	}


	g_free (number_of_fields);
	
	return 0;
}



int draw_matrix_axis (frame_t * f, cairo_t * cr, GList * list, GList * first_part_ip_list, int * number_of_fields) 
{
	data_t 		* data 			= (data_t *) f->priv;
 	cairo_set_line_width (cr, 1);
	cairo_set_font_size (cr, frame_get_height (f)*0.04);

	// Draw xaxis
	cairo_move_to (cr, frame_get_width (f)*0.1, frame_get_height (f)*0.1);
	cairo_line_to (cr, frame_get_width (f)*0.96, frame_get_height (f)*0.1);
	cairo_stroke (cr);
	// Draw yaxis
	cairo_move_to (cr, frame_get_width (f)*0.1, frame_get_height (f)*0.1);
	cairo_line_to (cr, frame_get_width (f)*0.1, frame_get_height (f)*0.9);
	cairo_stroke (cr);

	// Draw xstubs
	int 		i 		= 0;
	GList 		* iter 		= list;
	char 		* stub_name 	= NULL;

	if (g_strcmp0 (data->graph_type, "matrix_all") == 0) {
		// If all criterions matrix, xstubs are the machines names in machines list
		for (i = 1 ; i < number_of_fields[0] + 1; i++) {
		 	cairo_move_to (cr, ((0.1+(i*0.86)/(number_of_fields[0] + 1))*frame_get_width(f)), frame_get_height (f)*0.08 );
			cairo_line_to (cr, ((0.1+(i*0.86)/(number_of_fields[0] + 1))*frame_get_width(f)), frame_get_height (f)*0.12 );

		 	cairo_move_to (cr, ((0.1+(i*0.86)/(number_of_fields[0] + 1))*frame_get_width(f)), frame_get_height (f)*0.08 );
			if (machine_is_all_machines (iter->data)) {
			 	iter = g_list_next (iter);
			}
		 	cairo_show_text (cr, machine_get_folder_name (iter->data));
		 	iter = g_list_next (iter);
		 }
	}
	else {
		// If one criterion matrix, xstubs are all possibles end for an ip address (from 0 to 255)
		i = 0;
		while (i < number_of_fields[0] + 1) {
		 	cairo_move_to (cr, ((0.1+(i*0.86)/(number_of_fields[0] + 1))*frame_get_width(f)), frame_get_height (f)*0.08 );
			cairo_line_to (cr, ((0.1+(i*0.86)/(number_of_fields[0] + 1))*frame_get_width(f)), frame_get_height (f)*0.12 );

		 	cairo_move_to (cr, ((0.1+(i*0.86)/(number_of_fields[0] + 1))*frame_get_width(f)), frame_get_height (f)*0.08 );
		 	stub_name = g_strdup_printf ("%i", i);
		 	cairo_show_text (cr, stub_name);
			i+=25;
		}
	}
	cairo_stroke (cr);
	
	
	//Draw ystubs
	iter = first_part_ip_list;
	for (i = 1 ; i < number_of_fields[1] + 1; i++) {
	 	cairo_move_to (cr, frame_get_width (f)*0.08, ((0.1+(i*0.8)/(number_of_fields[1] + 1))*frame_get_height(f)) );
		cairo_line_to (cr, frame_get_width (f)*0.12, ((0.1+(i*0.8)/(number_of_fields[1] + 1))*frame_get_height(f)) );

		cairo_move_to (cr, 0, ((0.1+(i*0.8)/(number_of_fields[1] + 1))*frame_get_height(f)) );

		if (g_strcmp0 (data->graph_type, "matrix_all") == 0) {
			// If all criterions matrix, ystubs are the criterions names
			stub_name = get_criterion (i);
			if (data->display_axis_legend == 1) {
			 	cairo_show_text (cr, stub_name);
			}
			else {
			 	stub_name = g_strndup (stub_name, 7);
			 	cairo_show_text (cr, stub_name);
			}
		}
		else {
			// If one criterion matrix, ystubs are the beginning of the machines names 
		 	cairo_show_text (cr, iter->data);
		 	iter = g_list_next (iter);
		}
		cairo_stroke (cr);
	}

	g_free (stub_name);

	return 0;
}

char * get_criterion (int i)
{
	char 		* criterion_name 	= NULL;

	switch (i) {
		case 1:
			criterion_name = g_strdup("Criticality");
			break;
		case 2:
			criterion_name = g_strdup("Number of Sequences");
			break;
		case 3:
			criterion_name = g_strdup("Sequences Criticality");
			break;
		case 4:
			criterion_name = g_strdup("Number of Activities");
			break;
		default:
			criterion_name = g_strdup("No criterion");
			break;
	}

	return criterion_name;
}

int matrix_all_handler (frame_t * f, cairo_t * cr, int * number_of_fields, GList * list)
{
	int 		criterion_value	= 0;
	int 		machine_nb 	= 1;
	int 		i 		= 0;

	if (list) {
		GList 	* iter 		= list;
		// For each machine
		do { 
			if (!machine_is_all_machines (iter->data)) {
				// For each criterion
				for (i = 1 ; i < number_of_fields[1] + 1; i++) { 
					// Draw a matrix item
				/*	if (machine_nb == 1)
						criterion_value = 100;
					else if (machine_nb == 2)
						criterion_value = 300;
					else if (machine_nb == 3)
						criterion_value = 499;
				*/
					criterion_value = find_criterion_value (f, (char*)machine_get_folder_name (iter->data), list, i);
					draw_matrix_item (f, cr, number_of_fields, machine_nb, i, criterion_value, 0);
				}
				machine_nb++;
			}
		} while ((iter = g_list_next (iter)) != NULL);
	}


	return 0;
}



int draw_matrix_item (frame_t *f, cairo_t * cr, int * number_of_fields, int x_pos, int y_pos, int criterion_value, int criterion_index)
{
	data_t *data = (data_t *) f->priv;

	int square_size = 10;
	cairo_rectangle (cr, ((0.1+(x_pos*0.86)/(number_of_fields[0] + 1))*frame_get_width(f)) - square_size/2
				, (0.1+(y_pos*0.8)/(number_of_fields[1] + 1))*frame_get_height(f) - square_size/2
				, square_size
				, square_size);
				


	if (g_strcmp0 (data->graph_type, "matrix_all") == 0) {
		select_criterion_color (cr, y_pos, criterion_value);
	}
	else {
		select_criterion_color (cr, criterion_index, criterion_value);
	}
	
	cairo_fill (cr);
	cairo_stroke (cr);

	return 0;
}


int select_criterion_color (cairo_t * cr, int criterion_nb, int criterion_value)
{
	//TODO Find max for each criterion
	double criterion_max_value = 100;
	double red = 0;
	double green = 0;
	double blue = 0;
	
	switch (criterion_nb) {
		case 1:
			red  = 1.0;
			green = 0.0;
			blue = 0.0;
			break;
		case 2:
			red  = 0.0;
			green = 1.0;
			blue = 0.0;
			break;
		case 3:
			red  = 0.0;
			green = 0.0;
			blue = 1.0;
			break;
		case 4:
			red  = 1.0;
			green = 1.0;
			blue = 0.0;
			break;
		case 5:
			red  = 0.0;
			green = 1.0;
			blue = 1.0;
			break;
		default:
			red  = 0.0;
			green = 0.0;
			blue = 0.0;
			break;
	}	

	if (criterion_value > criterion_max_value/2) {
		red -= ((double)criterion_value-(criterion_max_value/2)) / (criterion_max_value/2) ;
		green -= ((double)criterion_value-(criterion_max_value/2)) / (criterion_max_value/2) ;
		blue -= ((double)criterion_value-(criterion_max_value/2)) / (criterion_max_value/2) ;
	}
	else if (criterion_value < criterion_max_value/2) {
		red += ((criterion_max_value/2)-(double)criterion_value) / (criterion_max_value/2) ;
		green += ((criterion_max_value/2)-(double)criterion_value) / (criterion_max_value/2) ;
		blue += ((criterion_max_value/2)-(double)criterion_value) / (criterion_max_value/2) ;
	}
	cairo_set_source_rgb (cr, red, green, blue); 

	return 0;
}



int matrix_one_criterion_handler (frame_t * f, cairo_t * cr, int * number_of_fields, GList * list, GList * first_part_ip_list)
{
	data_t 		* data 			= (data_t *) f->priv;

	char 		* ip_end 		= NULL;		
	char 		* ip_first_part 	= NULL;
	int	 	y_pos 			= 0;
	int 		criterion_value 	= 0;
	int 		criterion_type 		= 0;
	int 		machine_nb 		= 1;
	int 		found_flag 		= 0;
	
	if (list) {
		GList 	* iter 			= list;
		GList 	* tmp 			= first_part_ip_list;
		// For each machine
		do {
			if (!machine_is_all_machines (iter->data)) {
				// Get x position for matrix item
				ip_end = g_strrstr (machine_get_folder_name (iter->data), ".");
				ip_first_part = g_strndup (machine_get_folder_name (iter->data), ip_end - machine_get_folder_name (iter->data));
				machine_nb = atoi (ip_end + 1);
				
				// Get y position for matrix item
				tmp = g_list_copy (first_part_ip_list);
				found_flag = 0;
				do {
					if (g_strcmp0 (tmp->data, ip_first_part) == 0) 
						found_flag = 1;
				} while (found_flag == 0 && (tmp = g_list_next(tmp)) != NULL);
				y_pos = g_list_index (first_part_ip_list, tmp->data) + 1;
				
				// Get criterion name and value
				if ( g_strcmp0 (data->graph_type, "matrix_criticality") == 0) {
					criterion_value = find_raw_criticality_value (f, (char*)machine_get_folder_name (iter->data), list);
					criterion_type = CRITICALITY_CRITERION;
				}
				else if ( g_strcmp0 (data->graph_type, "matrix_activities") == 0) {
					criterion_value = find_activities_number_value (f, (char*)machine_get_folder_name (iter->data), list);
					criterion_type = ACTIVITIES_CRITERION;
				}
				else if ( g_strcmp0 (data->graph_type, "matrix_sequences") == 0) {
					criterion_value = find_sequences_number_value (f, (char*)machine_get_folder_name (iter->data), list);
					criterion_type = SEQUENCES_NUMBER_CRITERION;
				}
				else if ( g_strcmp0 (data->graph_type, "matrix_sequences_criticality") == 0) {
					criterion_value = find_sequences_criticality_value (f, (char*)machine_get_folder_name (iter->data), list);
					criterion_type = SEQUENCES_CRITICALITY_CRITERION;
				}
				
				// Draw a matrix item
				draw_matrix_item (f, cr, number_of_fields, machine_nb, y_pos, criterion_value, criterion_type);
				machine_nb++;
			}
		} while ((iter = g_list_next (iter)) != NULL);
		g_list_free (list);
	}

	return 0;
}


int find_raw_criticality_value (frame_t * f, char * machine_name,  GList * list)
{
	return 0;	
}



int find_activities_number_value (frame_t * f, char * machine_name,  GList * list)
{
	data_t *data = (data_t *) f->priv;

	GList * iter = g_list_copy (list);
	int machine_index = -1;
	
	while (iter != NULL && machine_index == -1) {
		if (g_strcmp0 (machine_get_folder_name (iter->data), machine_name) == 0) {
			machine_index = g_list_index (list, iter->data);
		}
	
		iter = g_list_next (iter);
	}

	int criterion_value = data->activity_number[machine_index];
	return criterion_value;	
}


int find_sequences_number_value (frame_t * f, char * machine_name,  GList * list)
{
	data_t *data = (data_t *) f->priv;

	GList * iter = g_list_copy (list);
	int machine_index = -1;
	
	while (iter != NULL && machine_index == -1) {
		if (g_strcmp0 (machine_get_folder_name (iter->data), machine_name) == 0) {
			machine_index = g_list_index (list, iter->data);
		}
	
		iter = g_list_next (iter);
	}

	int criterion_value = data->sequence_number[machine_index];
	return criterion_value;	
}


int find_sequences_criticality_value (frame_t * f, char * machine_name,  GList * list)
{
	data_t *data = (data_t *) f->priv;

	GList * iter = g_list_copy (list);
	int machine_index = -1;
	
	while (iter != NULL && machine_index == -1) {
		if (g_strcmp0 (machine_get_folder_name (iter->data), machine_name) == 0) {
			machine_index = g_list_index (list, iter->data);
		}
	
		iter = g_list_next (iter);
	}

	int criterion_value = data->sequence_criticality[machine_index];

	return criterion_value;	
}



int find_criterion_value (frame_t * f, char * machine_name, GList * list, int criterion_number)
{
	int criterion_value = 0;
	
	switch (criterion_number) {
		case CRITICALITY_CRITERION:
			criterion_value = find_raw_criticality_value (f, machine_name, list);
			break;
		case SEQUENCES_NUMBER_CRITERION:
			criterion_value = find_sequences_number_value (f, machine_name, list);
			break;
		case SEQUENCES_CRITICALITY_CRITERION:
			criterion_value = find_sequences_criticality_value (f, machine_name, list);
			break;
		case ACTIVITIES_CRITERION:
			criterion_value = find_activities_number_value (f, machine_name, list);
			break;
		default :
			criterion_value = find_sequences_criticality_value (f, machine_name, list);
			break;
	}
	
	return criterion_value;
}
