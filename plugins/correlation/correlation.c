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

void correlation_draw_axis (frame_t *f, cairo_t *cr, time_t current_time, int period_type) 
{
	data_t *data = (data_t *) f->priv;

 	cairo_set_source_rgb (cr, 0, 0, 0);
 	cairo_set_line_width (cr, 1);
	
	char * title = NULL;	

	// Draw y axis 
 	cairo_move_to (cr, frame_get_width (f)*0.16, frame_get_height (f)*0.9);
	cairo_line_to (cr, frame_get_width (f)*0.16, frame_get_height (f)*0.1);
 	cairo_move_to (cr, frame_get_width (f)*0.14, frame_get_height (f)*0.13);
	cairo_line_to (cr, frame_get_width (f)*0.16, frame_get_height (f)*0.1);
 	cairo_move_to (cr, frame_get_width (f)*0.16, frame_get_height (f)*0.1);
 	cairo_line_to (cr, frame_get_width (f)*0.18, frame_get_height (f)*0.13);
	cairo_stroke (cr);
	// Draw y stubs
	int i = 0;
	for (i = 1 ; i < data->Number_Of_Sensors + 1 ; i++) {
	 	cairo_move_to (cr, frame_get_width (f)*0.14, ((0.1+(i*0.8)/(data->Number_Of_Sensors + 1))*frame_get_height(f)));
		cairo_line_to (cr, frame_get_width (f)*0.18, ((0.1+(i*0.8)/(data->Number_Of_Sensors + 1))*frame_get_height(f)));
	 	cairo_move_to (cr, 0, ((0.1+(i*0.8)/(data->Number_Of_Sensors + 1))*frame_get_height(f)));
	 	cairo_show_text (cr, correlation_get_stub_name(f, i));
		cairo_stroke (cr);
	}
	// Print yaxis name
 	cairo_move_to (cr, frame_get_width (f)* 0.18, frame_get_height(f)*0.1);	
	title = correlation_get_yaxis_title (period_type);
 	cairo_show_text (cr, title);
	cairo_stroke (cr);
		
 	// Draw xaxis 
 	cairo_move_to (cr, frame_get_width (f)*0.16, frame_get_height (f)*0.9);
	cairo_line_to (cr, frame_get_width (f)*0.96, frame_get_height (f)*0.9);
 	cairo_move_to (cr, frame_get_width (f)*0.93, frame_get_height (f)*0.92);
	cairo_line_to (cr, frame_get_width (f)*0.96, frame_get_height (f)*0.9);
 	cairo_move_to (cr, frame_get_width (f)*0.96, frame_get_height (f)*0.9);
 	cairo_line_to (cr, frame_get_width (f)*0.93, frame_get_height (f)*0.88);
	cairo_stroke (cr);
	// Draw x stubs
	correlation_draw_xstubs (f, cr, current_time, period_type);
	// Print xaxis name
 	cairo_move_to (cr, frame_get_width (f)* 0.90, frame_get_height(f)*0.85);	
	title = correlation_get_xaxis_title (period_type);
 	cairo_show_text (cr, title);
	cairo_stroke (cr);
		
	g_free (title);		
}

char * correlation_get_stub_name(frame_t * f, int i)
{
	data_t *data = (data_t *) f->priv;

	return g_slist_nth_data (data->Sensors_List, i-1);
}


int correlation_draw_xstubs (frame_t *f, cairo_t *cr, time_t current_time, int period_type)
{
 	cairo_set_source_rgb (cr, 0, 0, 0);
 	cairo_set_line_width (cr, 1);
	
	int time_period = correlation_get_period(period_type);
	int period = 0;
	int stubs_nb = 0;
	time_t time = 0;
	
	struct tm * end_time;
	end_time = localtime ((time_t*)&current_time);
	time_t end_time_t = 0;
	
	switch (period_type) {
		case LIVE:
			stubs_nb = 6;
			period = time_period / 6;
			end_time->tm_hour += 1;
			end_time->tm_min = 0;
			end_time->tm_sec = 0;
			end_time_t = mktime (end_time);
			time = end_time_t - (6*period);
			break;
		case HOUR:
			stubs_nb = 6;
			period = time_period / 6;
			end_time->tm_min = 0;
			end_time->tm_sec = 0;
			end_time_t = mktime (end_time);
			time = end_time_t - (6*period);
			break;
		case DAY:
			stubs_nb = 4;
			period = time_period / 4;
			end_time->tm_mday += 1;
			end_time->tm_hour = 0;
			end_time->tm_min = 0;
			end_time->tm_sec = 0;
			end_time_t = mktime (end_time);
			time = end_time_t - (4*period);
			break;
		case WEEK:
			stubs_nb = 7;
			period = time_period / 7;
			end_time->tm_hour = 0;
			end_time->tm_min = 0;
			end_time->tm_sec = 0;
			end_time_t = mktime (end_time);
			time = end_time_t - (6*period);
			break;
		case MONTH:
			stubs_nb = 5;
			period = time_period / 5;
			end_time->tm_mday += 1;
			end_time->tm_hour = 0;
			end_time->tm_min = 0;
			end_time->tm_sec = 0;
			end_time_t = mktime (end_time);
			time = end_time_t - (5*period);
			break;
		default:
			stubs_nb = 5;
			period = time_period / 5;
			end_time->tm_hour = 0;
			end_time->tm_min = 0;
			end_time->tm_sec = 0;
			end_time_t = mktime (end_time);
			time = end_time_t - (4*period);
			break;
	}
	
	char * xstub = NULL;

	struct tm * time_tm;
	time_tm = localtime ((time_t*)&time);

	show_actual_date (f, cr, period_type, time_tm);

	// Draw xstubs
	int i = 0;
	while (time <= end_time_t) {
	 	cairo_move_to (cr, frame_get_width (f)* (0.16 + 0.80*i/stubs_nb), frame_get_height(f)*0.88);
		cairo_line_to (cr, frame_get_width (f)* (0.16 + 0.80*i/stubs_nb), frame_get_height(f)*0.92);
	 	cairo_move_to (cr, frame_get_width (f)* (0.11 + 0.80*i/stubs_nb), frame_get_height(f)*0.98);
	 	time_tm = localtime ((time_t*)&time);
	 	xstub = correlation_get_xstub_name(period_type, time_tm);
		cairo_show_text (cr, xstub);
		cairo_stroke (cr);
	 	time += period;
		i++;
	}

	g_free (xstub);

	return 0;
}

char * correlation_get_xstub_name(int period_type, struct tm * time_tm) 
{
	char * stub_name = NULL;
	if (period_type == LIVE || period_type == HOUR || period_type == DAY)
		stub_name = g_strdup_printf("%i:%i",time_tm->tm_hour, time_tm->tm_min);
	else 
		stub_name = g_strdup_printf ("%i/%i",time_tm->tm_mday, time_tm->tm_mon+1);
	
	return stub_name;
}

char * correlation_get_xaxis_title (int period_type)
{
	char * axis_title = NULL;
	if (period_type == LIVE || period_type == HOUR || period_type == DAY)
		axis_title = g_strdup("Hour");
	else 
		axis_title = g_strdup("Day");
	
	return axis_title;
}


char * correlation_get_yaxis_title (int period_type)
{
	char * axis_title = NULL;

	axis_title = g_strdup("Sensors");
	
	return axis_title;
}


int correlation_get_period (int period_name) 
{
	switch (period_name) {
		case LIVE : 
			return 21600;
		case HOUR : 
			return 3600;
		case DAY : 
			return 86400;
		case WEEK : 
			return 604800;
		case MONTH : 
			return 2592000;
		default : 
			return 604800;
	}
}


char * correlation_get_period_name(int period_type)
{
	switch (period_type) {
		case LIVE : 
			return "live";
		case HOUR : 
			return "hour";
		case DAY : 
			return "day";
		case WEEK : 
			return "week";
		case MONTH : 
			return "month";
		default : 
			return "month";
	}
}



void draw_arrows_for_map (frame_t *f, cairo_t *cr) 
{
	char				*path_scroll		= NULL;
	cairo_surface_t			*image			= NULL;
	double				tmp = 0;	
	int				w,h;

	path_scroll = frame_get_absolute_data_path (f, "arrows.png");
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



int show_actual_date (frame_t * f, cairo_t * cr, int period_type, struct tm * time_tm)
{
	if (period_type == LIVE || period_type == HOUR || period_type == DAY) {
	 	cairo_move_to (cr, 0.35 * frame_get_width (f), 0.042 * frame_get_height(f));
	 	char * day = g_strdup_printf ("%i/%i/%i", time_tm->tm_mday, time_tm->tm_mon+1, time_tm->tm_year + 1900);
		cairo_show_text (cr, day);
		cairo_stroke (cr);
		g_free (day);
	}
	else {
	 	cairo_move_to (cr, 0.35 * frame_get_width (f), 0.042 * frame_get_height(f));
	 	char * year = g_strdup_printf ("%i", time_tm->tm_year + 1900);
		cairo_show_text (cr, year);
		cairo_stroke (cr);
		g_free (year);
	}
	
	return 0;
}


int print_title (frame_t * f, cairo_t * cr, char * title) 
{
	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);	
	cairo_set_font_size (cr, frame_get_height (f)*0.046);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_move_to(cr, 2, frame_get_height (f)*0.042);
	cairo_show_text (cr, title);
	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

	return 0;
}


