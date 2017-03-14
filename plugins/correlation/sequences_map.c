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
 * \file sequences_map.c
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
#include "sequences_map.h"


int display_sequences_map(frame_t * f, cairo_t * cr)
{
	data_t *data = (data_t *) f->priv;
	
	// Draw background and title 
	cairo_set_source_rgb (cr, 255, 255, 255); 
	cairo_paint_with_alpha (cr, 1);
	print_title (f, cr, "Sequences");
	// Draw legend and axis 
	legend_handler (f, cr, SEQUENCES_FILE);
	cairo_set_font_size (cr, frame_get_height (f)*0.042);
	time_t current_time = time(NULL);
	current_time -= data->time_period_offset * correlation_get_period (HOUR);
	draw_sequence_axis(f, cr, current_time, f->display_period.type);
	// Draw arrows to go backward and forward
	if (f->display_period.type != LIVE)
		draw_arrows_for_map (f, cr);

	// All sequence_links_number are set to 0
	int 		i 		= 0;
	GList 		* tmp 		= NULL;

	for (i=0 ; i < data->Number_Of_Activities ; i++) {
		tmp = g_list_copy(data->activity_list[i]);
		while ( tmp != NULL ) {
			((Activity_t *)(tmp->data))->sequence_links_number = 0;
			tmp = g_list_next (tmp);
		}
	}

	// Draw sequences graph
	char 		** sequence_array_tmp 	= NULL;
	
	for (i = 0 ; i < data->Number_Of_Sequences ; i++) { // For each sequence
		tmp = g_list_copy (data->sequences_list[i]);
		select_activity_color (cr, data->colors_array[i]);

		while (tmp != NULL) { // Draw each occurence of sequence #i
			float * last_position = malloc (2*sizeof(float));
			last_position[0] = 0;
			last_position[1] = 0;
			draw_sequence (f, cr, current_time, i, tmp, last_position);
			tmp = g_list_next (tmp);
			g_free (last_position);
		}
	}
	g_strfreev (sequence_array_tmp);

	draw_sequences_confidence_plot (f, cr, current_time);

	return 0;
}


float * draw_sequence (frame_t * f, cairo_t * cr, time_t current_time, int sequence_number, GList * sequence_occurence, float * last_position)
{
	GList * sub_sequence = g_list_copy (sequence_occurence->data);

	// Draw every sub_sequence in sequence_occurence
	while (sub_sequence != NULL) {
		last_position = draw_subsequence (f, cr, current_time, sequence_number, ((Sequence_t *)(sub_sequence->data)), last_position);
		sub_sequence = g_list_next (sub_sequence);
	}

	return last_position;
}


float * draw_subsequence (frame_t * f, cairo_t * cr,  time_t current_time, int sequence_number, Sequence_t * sequence_info, float * last_position)
{
	data_t *data = (data_t *) f->priv;

	// Get the time to use for the end of xaxis
	struct tm * end_time;
	end_time = localtime ((time_t*)&current_time);
	time_t end_time_t = 0;
	end_time = fill_time_struct_for_drawing (f, end_time);
	end_time_t = mktime (end_time);

	int period = correlation_get_period(f->display_period.type );
	cairo_set_line_width (cr, 1);

	float y_pos = 0, x_pos_1 = 0, x_pos_2 = 0;
	GList * tmp = NULL;

	// If sub-sequence is an activity
	if (sequence_info->sub_sequence_type == 0) {
		y_pos = (0.1+((sequence_info->sub_sequence_number+1)*0.8)/(data->Number_Of_Activities + 1))*frame_get_height(f);
		y_pos += ((double)sequence_number/(double)(data->Number_Of_Sequences+1))*(0.8*frame_get_height(f) / (8*data->Number_Of_Activities + 1));

		tmp = g_list_nth ( data->activity_list[sequence_info->sub_sequence_number], sequence_info->sub_sequence_index );
		((Activity_t *) (tmp->data))->sequence_links_number++;
		
		x_pos_1 = ((Activity_t *) (tmp->data))->beginning_date;
		x_pos_1 = frame_get_width (f) - ( frame_get_width (f)* (0.04 + 0.80*(end_time_t - x_pos_1)/period) );

		x_pos_2 = ((Activity_t *) (tmp->data))->end_date;
		x_pos_2 = frame_get_width (f) - ( frame_get_width (f)* (0.04 + 0.80*(end_time_t - x_pos_2)/period) );

		// If the activity happened during the displayed period
		if (x_pos_2 > 0.16*frame_get_width (f) 	&& x_pos_1 < 0.96*frame_get_width (f) ) {
			// Draw activity
			if (x_pos_1 > 0.16*frame_get_width (f) )
				cairo_move_to (cr, x_pos_1, y_pos);
			else 
				cairo_move_to (cr, 0.16*frame_get_width (f), y_pos);

			if (x_pos_2 < 0.96*frame_get_width (f)) 
				cairo_line_to (cr, x_pos_2 + 1, y_pos);
			else 
				cairo_line_to (cr, 0.96*frame_get_width (f) + 1, y_pos);

			//Draw link between last drawn sub-sequence and the new one
			if (last_position[0] != 0) {
				cairo_move_to (cr, last_position[0], last_position[1]);
				cairo_line_to (cr, x_pos_2, y_pos);
			}
			
			cairo_stroke (cr);
		}
		last_position[0] = x_pos_1;
		last_position[1] = y_pos;
	}
	// If sub-sequence is a sequence
	else {
		// Draw this sub-sequence 
		tmp = g_list_nth ( data->sequences_list[sequence_info->sub_sequence_number], sequence_info->sub_sequence_index );
		last_position = draw_sequence (f, cr, current_time, sequence_number, tmp, last_position);
	}

	cairo_stroke (cr);

	return last_position;
}


int draw_sequences_confidence_plot (frame_t * f, cairo_t * cr, time_t current_time)
{
	data_t *data = (data_t *) f->priv;

	int 		i	= 0;
	GList 		* tmp 	= NULL;
	
	// For each activity occurence, draw a bar for confidence plot
	for (i = 0 ; i < data->Number_Of_Activities ; i++) {
		tmp = g_list_copy(data->activity_list[i]);
		while ( tmp != NULL ) {
			draw_curve (f, cr, current_time, ((Activity_t *)(tmp->data))->beginning_date
							, ((Activity_t *)(tmp->data))->sequence_links_number);
			tmp = g_list_next (tmp);
		}
	}

	return 0;
}



void draw_sequence_axis(frame_t *f, cairo_t *cr, time_t current_time, int period_type)
{
	data_t 		*data 		= (data_t *) f->priv;
	
	char 		* title 	= NULL;	

	// Draw yaxis for the sequences graph
 	cairo_set_source_rgb (cr, 0, 0, 0);
 	cairo_set_line_width (cr, 1);
 	cairo_move_to (cr, frame_get_width (f)*0.16, frame_get_height (f)*0.9);
	cairo_line_to (cr, frame_get_width (f)*0.16, frame_get_height (f)*0.1);
 	cairo_move_to (cr, frame_get_width (f)*0.14, frame_get_height (f)*0.13);
	cairo_line_to (cr, frame_get_width (f)*0.16, frame_get_height (f)*0.1);
 	cairo_move_to (cr, frame_get_width (f)*0.16, frame_get_height (f)*0.1);
 	cairo_line_to (cr, frame_get_width (f)*0.18, frame_get_height (f)*0.13);
	cairo_stroke (cr);

	// Draw ystubs for the sequences graph
	char 		* ystub_name 	= NULL;
	int 		i 		= 0;
	for (i = 1; i < data->Number_Of_Activities + 1; i++) {
	 	cairo_move_to (cr, frame_get_width (f)*0.14, ((0.1+(i*0.8)/(data->Number_Of_Activities + 1))*frame_get_height(f)));
		cairo_line_to (cr, frame_get_width (f)*0.18, ((0.1+(i*0.8)/(data->Number_Of_Activities + 1))*frame_get_height(f)));
	 	cairo_move_to (cr, 0, ((0.1+(i*0.8)/(data->Number_Of_Activities + 1))*frame_get_height(f)));
	 	
	 	if (data->display_axis_legend == 1) {
		 	cairo_show_text (cr, data->activity_name_array[i-1]);
		}
		else {
		 	ystub_name = g_strndup (data->activity_name_array[i-1], 9);
		 	cairo_show_text (cr, ystub_name);
		}
		cairo_stroke (cr);
	}

 	cairo_move_to (cr, frame_get_width (f)* 0.18, frame_get_height(f)*0.1);	
 	cairo_show_text (cr, "Activities");
 	g_free (ystub_name);
	cairo_stroke (cr);
		
	// Draw xaxis for the sequences graph
 	cairo_move_to (cr, frame_get_width (f)*0.16, frame_get_height (f)*0.9);
	cairo_line_to (cr, frame_get_width (f)*0.96, frame_get_height (f)*0.9);
 	cairo_move_to (cr, frame_get_width (f)*0.93, frame_get_height (f)*0.92);
	cairo_line_to (cr, frame_get_width (f)*0.96, frame_get_height (f)*0.9);
 	cairo_move_to (cr, frame_get_width (f)*0.96, frame_get_height (f)*0.9);
 	cairo_line_to (cr, frame_get_width (f)*0.93, frame_get_height (f)*0.88);
	cairo_stroke (cr);
	// Draw xstubs for the sequences graph
	correlation_draw_xstubs (f, cr, current_time, period_type);
	// Draw x title for the sequences graph
 	cairo_move_to (cr, frame_get_width (f)* 0.90, frame_get_height(f)*0.85);	
	title = correlation_get_xaxis_title (period_type);
 	cairo_show_text (cr, title);
	cairo_stroke (cr);
		
	g_free (title);		
}

