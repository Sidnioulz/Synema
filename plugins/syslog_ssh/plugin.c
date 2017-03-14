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
 *Free
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file plugin.c
 * \brief Fake PIGA-IDS plugin written by the twentieth group
 * \author Steve Dodier <sidnioulz@gmail.com>
 * \author Timothée Ravier <timothee.romain.ravier@gmail.com>
 *
 * This file contains a fake PIGA-IDS plugin for demonstration purpose.
 */
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "constants.h"
#include "frame-common.h"
#include "frame-utilities.h"
#include "machine.h"
#include "plugin.h"
#include "time-period.h"



char *get_display_name (frame_t *f)
{
	return "Syslog SSH";
}



void user (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 1;
}



void ip (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 2;
}

void connex (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 3;
}

GList *build_func_list ()
{
	GList *list = NULL;

	func_list_add_new_func (&list, "Connexions", connex);

	func_list_add_new_separator (&list);

	func_list_add_new_func (&list, "IP Addresses", ip);

	return list;
}


GList *build_time_periods ()
{
	GList *list = NULL;
	time_period_t *p0, *p1, *p2, *p3;

	p0 = time_period_new (WEEK, 2);
	list = g_list_prepend (list, p0);

	p1 = time_period_new (DAY, 1);
	list = g_list_prepend (list, p1);

	p2 = time_period_new (HOUR, 1);
	list = g_list_prepend (list, p2);

	p3 = time_period_new (LIVE, 0);
	list = g_list_prepend (list, p3);


	return list;
}


int is_refreshing_needed (frame_t *f)
{
	int refreshing_needed = 1;
	struct stat buf;
	static time_t time_access=0;	
	char *path = frame_get_absolute_reports_path (f, "ip_report_live.txt", NULL);
	stat(path, &buf);

	if(buf.st_mtime > time_access)
	{
		refreshing_needed=1;
		time_access = buf.st_mtime;
	}	

	g_free (path);
	return refreshing_needed;
}


int refresh_content (frame_t *f)
{

	data_t * data_plug = f->priv;

	char *command=NULL;
	char *tmppath = NULL; 
	char *reportspath = NULL;
	tmppath =frame_get_absolute_tmp_path(f, "img.svg");


	if (data_plug->entier==2)
	{
		if (f->display_period.type == LIVE) {
			reportspath =frame_get_absolute_reports_path(f, "ip_report_live", NULL);
		}
		else if (f->display_period.type == HOUR) {
			reportspath =frame_get_absolute_reports_path(f, "ip_report_hour", NULL);
		}
		else if (f->display_period.type == DAY) {
			reportspath =frame_get_absolute_reports_path(f, "ip_report_day", NULL);
		}
		else if (f->display_period.type == WEEK) {
			reportspath =frame_get_absolute_reports_path(f, "ip_report_week", NULL);
		}	
	
		command= g_strdup_printf("ploticus -prefab vbars -svg -o %s data=%s x=1 y=3 y2=5  xlbl=Ip ylbl='Number of Attempts'  name=Successful name2=Failed color=green color2=red barwidth=0.1 stubvert=yes", tmppath, reportspath);
	}

	else 
	{
		if (f->display_period.type == LIVE) {
			reportspath =frame_get_absolute_reports_path(f, "date_report_live", NULL);
		}
		else if (f->display_period.type == HOUR) {
			reportspath =frame_get_absolute_reports_path(f, "date_report_hour", NULL);
		}
		else if (f->display_period.type == DAY) {
			reportspath =frame_get_absolute_reports_path(f, "date_report_day", NULL);
		}
		else if (f->display_period.type == WEEK) {
			reportspath =frame_get_absolute_reports_path(f, "date_report_week", NULL);
		}
		
		command= g_strdup_printf("ploticus -prefab stack -svg -o %s data=%s x=2 y=4 y2=6 xlbl=Hour ylbl='Number of Connections'  name=Successful name2=Failed color=green color2=red", tmppath, reportspath);
	}

	system(command);
	frame_request_redraw(f);
	g_free(command);
	g_free(reportspath);
	g_free(tmppath);

	return 0;
}



int init_private (frame_t *f)
{
	f->priv = malloc (sizeof (data_t));
	data_t *data = (data_t *) f->priv;

	data->entier = 2;

	frame_request_redraw (f);	
	frame_request_content_refresh (f);
	
	return 0;
}


int free_private (frame_t *f)
{
	data_t *data = (data_t *) f->priv;

	free (data);

	return 0;
}

int restore_private(frame_t *f, GKeyFile *file)
{
	data_t 	*data = (data_t *) f->priv;
	GError 	*err = NULL;

	data->entier = g_key_file_get_integer (file, f->get_display_name (f), "Graphic", &err);
	if (err) {
		fprintf (stderr, "Could not load the graphic type (%s)\n", err->message);
		g_clear_error (&err);
	}

	return 0;
}

int save_private(frame_t *f, GKeyFile *file)
{
	data_t 		*data 		= (data_t *) f->priv;

	g_key_file_set_integer (file, f->get_display_name (f), "Graphic" ,data->entier);

	return 0;
}


int display_machine_changed (frame_t *f)
{
	frame_request_redraw (f);
	frame_request_content_refresh (f);
	return 0;
}



int time_period_changed (frame_t *f)
{
	frame_request_redraw (f);
	frame_request_content_refresh (f);
	return 0;
}



int frame_size_changed (frame_t *f)
{
	frame_request_redraw (f);
	frame_request_content_refresh (f);
	return 0;
}



int draw_area (frame_t *f, cairo_t *cr)
{
	char 				*path		= NULL;
	data_t* 			data_plug	= (data_t*)f->priv;
	GError				*err		= NULL;
	RsvgHandle			*myhandle	= NULL;	
	
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);		
	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);	
	cairo_set_font_size (cr, frame_get_height (f)*0.07);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_move_to(cr, frame_get_width (f)*0.30, frame_get_height (f)*0.35);

	path =frame_get_absolute_tmp_path(f, "img.svg");
	myhandle = rsvg_handle_new_from_file (path, &err);
	g_free (path);
	if (err) { 
		g_clear_error (&err);
	}
	else {
		cairo_save (cr); 
		
		cairo_scale (cr, 0.8*frame_get_width (f)/FRAME_WIDTH_NORMAL, 0.8*frame_get_height(f)/FRAME_HEIGHT_NORMAL );
//		cairo_translate (cr, 0.4, 0.2);
		rsvg_handle_render_cairo (myhandle, cr);
		cairo_restore (cr);
		g_object_unref (myhandle);

		show_graphic_title(data_plug->entier, f, cr);
	}
				
	return 0;
}


void show_graphic_title(int entier, frame_t *f, cairo_t *cr) 
{

	cairo_set_font_size (cr, frame_get_height (f)*0.05);
	cairo_move_to (cr, frame_get_width (f)*0.1, frame_get_height (f)*0.05);	
		
	if (entier == 1)
		cairo_show_text (cr, "Number of alerts by User");

	else if (entier == 2)
		cairo_show_text (cr, "Number of alerts by IP Address");

	else if (entier == 3)
		cairo_show_text (cr, "Number of alerts by connection attempt");

}


int button_press_handler (frame_t *f, GdkEventButton *button)
{
	return 0;
}

int button_release_handler (frame_t *f, GdkEventButton *button)
{
	return 0;
}
