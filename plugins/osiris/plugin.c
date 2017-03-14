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
 * \brief Fake Osiris plugin written by the twentieth group
 * \author Steve Dodier <sidnioulz@gmail.com>
 * \author Timothée Ravier <timothee.romain.ravier@gmail.com>
 *
 * This file contains a fake Osiris plugin for demonstration purpose.
 */
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "constants.h"
#include "frame-common.h"
#include "frame-utilities.h"
#include "machine.h"
#include "plugin.h"
#include "time-period.h"



char *get_display_name (frame_t *f)
{
	return "Osiris";
}

char * osiris_graphic_title (frame_t *f)
{
	data_t 		*data 		= (data_t *) f->priv;
	char 		*title		= NULL;
	if (data->entier == 1)
		title = g_strdup("Type of changes");
	else if (data->entier == 2)
		title = g_strdup("Location of changes");
	else if (data->entier == 3)
		title = g_strdup("Type of module");
	else if (data->entier == 4)
		title = g_strdup("Sensitivity of modules");
	else if (data->entier == 5)
		title = g_strdup("Lists of modules (High)");
	else if (data->entier == 6)
		title = g_strdup("Lists of modules (Medium)");
	else if (data->entier == 7)
		title = g_strdup("Lists of modules (Unknow)");
	else if (data->entier == 8)
		title = g_strdup("List of ports");
	else 
		title = g_strdup("Hours of changes");
	
	
	return title;
}


GList *build_func_list ()
{
	GList *liste1 = NULL;
	GList *liste14 = NULL;
	GList *liste13 = NULL;
	GList *liste11 = NULL;
	GList *liste12 = NULL;

	func_list_add_new_func (&liste13, "High", list_m_high);
	func_list_add_new_func (&liste13, "Medium", list_m_medium);
	func_list_add_new_func (&liste13, "Unknown", list_m_unknown);	
	func_list_add_new_submenu (&liste1, "Lists of modules", liste13);

	func_list_add_new_func (&liste14, "All", list_ports);	
	func_list_add_new_submenu (&liste1, "List of ports", liste14);

	func_list_add_new_separator (&liste1);	

	func_list_add_new_func (&liste11, "Hours", f_hour);
	func_list_add_new_func (&liste11, "Type of changes", f_type);
	func_list_add_new_func (&liste11, "Locations", f_loc);
	func_list_add_new_submenu (&liste1, "Files", liste11);

	func_list_add_new_func (&liste12, "Type of modules", m_class);
	func_list_add_new_func (&liste12, "Sensitivity", m_sens);	
	func_list_add_new_submenu (&liste1, "Modules", liste12);


	return liste1;
}

void f_type(GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier= 1;
}

void f_loc(GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier= 2;	
}

void m_class(GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier= 3;	
}

void m_sens(GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier= 4;	
}

void list_m_high(GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier= 5;
}	

void list_m_medium(GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier= 6;
}	

void list_m_unknown(GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier= 7;
}	

void list_ports(GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier= 8;
}	

void f_hour(GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier= 9;
}

GList *build_time_periods ()
{
	GList *list = NULL;
	time_period_t *p1, *p2, *p3, *p4, *p5, *p6;

	p1 = time_period_new (DAY, 4);
	list = g_list_prepend (list, p1);

	p2 = time_period_new (DAY, 3);
	list = g_list_prepend (list, p2);

	p3 = time_period_new (DAY, 2);
	list = g_list_prepend (list, p3);

	p4 = time_period_new (DAY, 1);
	list = g_list_prepend (list, p4);

	p5 = time_period_new (DAY, 0);
	list = g_list_prepend (list, p5);

	p6 = time_period_new (MONTH, 1);
	list = g_list_prepend (list, p6);

	return list;
}



int is_refreshing_needed (frame_t *f)
{
	int refreshing_needed = !(rand() % 1);
	char *path = frame_get_absolute_reports_path (f, "mon_rapport.txt", NULL);

	//You must now stat path to get it's mtime. Make sure to manage errors properly.

	g_free (path);
	return refreshing_needed;
}



int refresh_content (frame_t *f)
{
	data_t	*data_plug = f->priv;
	char *command = NULL;
	char * tmppath = frame_get_absolute_tmp_path(f, "test.svg");
	char *path = NULL;
	char *path2 = NULL;
	char *path1 = NULL;


	if (f->display_period.type == DAY && f->display_period.value == 0) // DAY 0
	{
		if (data_plug->entier == 1 ) // f_type 		DAY 0, file->type of changes
		{	
			path1 = frame_get_absolute_data_path (f, "D0FC.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 2) // f_loc	DAY 0, file->Location
		{	
			path1 = frame_get_absolute_data_path (f, "D0FL.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 3) // m_class	DAY 0, Module->type of module
		{	
			path1 = frame_get_absolute_data_path (f, "D0MC.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 4) // m_sens	DAY 0, Module->Sensitivity
		{	
			path1 = frame_get_absolute_data_path (f, "D0MS.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}
		
		else if (data_plug->entier == 9) // f_hour	DAY 0, list_hour->all
		{	
			path1 = frame_get_absolute_data_path (f, "D0H.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

	}

	else if (f->display_period.type == DAY && f->display_period.value == 1) // DAY -1
	{
		if (data_plug->entier == 1 ) // f_type		DAY -1, file->type of change
		{	
			path1 = frame_get_absolute_data_path (f, "D1FC.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);
			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 2) // f_loc	DAY -1, file->location
		{	
			path1 = frame_get_absolute_data_path (f, "D1FL.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 3) // m_class	DAY -1, Module->type of module
		{	
			path1 = frame_get_absolute_data_path (f, "D1MC.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 4) // m_sens	DAY -1, Module->Sensitivity
		{	
			path1 = frame_get_absolute_data_path (f, "D1MS.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}
	
		else if (data_plug->entier == 9) // f_hour	DAY -1, list_hour->all
		{	
			path1 = frame_get_absolute_data_path (f, "D1H.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}
	}	


	else if (f->display_period.type == DAY && f->display_period.value == 2) // DAY -2
	{
		if (data_plug->entier == 1 ) // f_type 		DAY -2, file->type of change
		{	
			path1 = frame_get_absolute_data_path (f, "D2FC.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 2) // f_loc	DAY -2,	file->Location
		{	
			path1 = frame_get_absolute_data_path (f, "D2FL.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 3) // m_class	DAY -2, Module->type of module
		{	
			path1 = frame_get_absolute_data_path (f, "D2MC.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 4) // m_sens	DAY -2, Module->Sensitivity
		{	
			path1 = frame_get_absolute_data_path (f, "D2MS.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}
		
		else if (data_plug->entier == 9) // f_hour	DAY -2, list_hour->all
		{	
			path1 = frame_get_absolute_data_path (f, "D2H.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}
	}


	else if (f->display_period.type == DAY && f->display_period.value == 3) // DAY -3
	{
		if (data_plug->entier == 1 ) // f_type 		DAY -3, file->type of change
		{	
			path1 = frame_get_absolute_data_path (f, "D3FC.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 2) // f_loc	DAY -3, file->Location
		{	
			path1 = frame_get_absolute_data_path (f, "D3FL.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 3) // m_class	DAY -3, module->type of module
		{	
			path1 = frame_get_absolute_data_path (f, "D3MC.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 4) // m_sens	DAY -3, module->Sensitivity
		{	
			path1 = frame_get_absolute_data_path (f, "D3MS.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}
		
		else if (data_plug->entier == 9) // f_hour	DAY -3, list_hour->all
		{	
			path1 = frame_get_absolute_data_path (f, "D3H.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}
	}


	else if (f->display_period.type == DAY && f->display_period.value == 4) // DAY -4
	{
		if (data_plug->entier == 1 ) // f_type		DAY -4, file->type of change
		{	
			path1 = frame_get_absolute_data_path (f, "D4FC.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 2) // f_loc	DAY -4, file->Location
		{	
			path1 = frame_get_absolute_data_path (f, "D4FL.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 3) // m_class	DAY -4, Module->type of module
		{	
			path1 = frame_get_absolute_data_path (f, "D4MC.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 4) // m_sens	DAY -4, module->Sensitivity
		{	
			path1 = frame_get_absolute_data_path (f, "D4MS.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 9) // f_hour	DAY -4, list_hour->all
		{	
			path1 = frame_get_absolute_data_path (f, "D4H.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);

			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}
	}


	else if (f->display_period.type == MONTH) // MONTH
	{
		if (data_plug->entier == 1 ) // f_type 		MONTH, file->type of change
		{	
			path = frame_get_absolute_reports_path (f, "file_class.txt", NULL);
			command= g_strdup_printf("ploticus -prefab stack -svg -o %s data=%s x=1 y=2 y2=3 y3=4 stubvert=yes name=%s name2=%s name3=%s", tmppath,path,"change","new","missing");
			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 2) // f_loc	MONTH, file->Location
		{	
			path = frame_get_absolute_reports_path (f, "file_location.txt", NULL);
			command= g_strdup_printf("ploticus -prefab stack -svg -o %s data=%s x=1 y=2 y2=3 y3=4 y4=5 y5=6 y6=7 y7=8 stubvert=yes name=%s name2=%s name3=%s name4=%s name5=%s name6=%s name7=%s", tmppath,path,"bin","sbin","var","lib","sys","boot","usr");
			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 3) // m_class	MONTH, Module->type of module
		{	
			path = frame_get_absolute_reports_path (f, "module_class.txt", NULL);
			command= g_strdup_printf("ploticus -prefab stack -svg -o %s data=%s x=1 y=2 y2=3 y3=4 y4=5 stubvert=yes name=%s name2=%s name3=%s name4=%s", tmppath,path,"users","groups","ports","kern");
			system(command);
			frame_request_redraw (f);	
		}

		else if (data_plug->entier == 4) // m_sens	MONTH, Module->Sensitivity
		{	
			path = frame_get_absolute_reports_path (f, "module_sensitivity.txt", NULL);
			command= g_strdup_printf("ploticus -prefab stack -svg -o %s data=%s x=1 y=2 y2=3 y3=4 stubvert=yes name=%s name2=%s name3=%s", tmppath,path,"high","medium","unknown");
			system(command);
			frame_request_redraw (f);	
		}
		
		else if (data_plug->entier == 9) // f_hour	MONTH, file_hour
		{	
			path = frame_get_absolute_reports_path (f, "file_hour.txt", NULL);
			command= g_strdup_printf("ploticus -prefab stack -svg -o %s data=%s x=1 y=2 y2=3 y3=4 stubvert=yes name=%s name2=%s name3=%s", tmppath,path,"1h","2h","3h");
			system(command);
			frame_request_redraw (f);
		}
	}
	
	if (data_plug->entier == 5) // list_m_high 	list_m->high
		{	
			path1 = frame_get_absolute_data_path (f, "LMH.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);
			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);
		}
	
	if (data_plug->entier == 6) // list_m_medium	list_m->medium
		{	
			path1 = frame_get_absolute_data_path (f, "LMM.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);
			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

	if (data_plug->entier == 7) // list_m_unknown	list_m->unknown
		{	
			path1 = frame_get_absolute_data_path (f, "LMU.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);
			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}
	if (data_plug->entier == 8) // list_port	list_port->all
		{	
			path1 = frame_get_absolute_data_path (f, "LP.pl");
			path2 = frame_get_absolute_reports_path (f, NULL, NULL);
			command= g_strdup_printf("ploticus %s  -dir %s -svg -o %s ",path1,path2,tmppath);

			system(command);
			frame_request_redraw (f);	
		}

	g_free(command);
	g_free(tmppath);
	g_free(path);
	g_free(path1);
	g_free(path2);
	return 0;
}

int init_private (frame_t *f)
{
	f->priv = malloc (sizeof (data_t));
	data_t *data = (data_t *) f->priv;

	data->entier = 4;

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
	frame_request_content_refresh (f);
	return 0;
}



int time_period_changed (frame_t *f)
{
	frame_request_content_refresh (f);

	return 0;
}



int frame_size_changed (frame_t *f)
{
	frame_request_content_refresh (f);
	return 0;
}


int draw_area (frame_t *f, cairo_t *cr)
{
	char 				*path		= NULL;
	char 				*title		= NULL;
	data_t 				*data 		= (data_t *) f->priv;
	GError				*err		= NULL;
	RsvgHandle			*myhandle	= NULL;	
	cairo_surface_t *image;


	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);		
	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);	
	cairo_set_font_size (cr, frame_get_height (f)*0.07);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_move_to(cr, frame_get_width (f)*0.30, frame_get_height (f)*0.35);

	path = frame_get_absolute_tmp_path (f, "test.svg");
	myhandle = rsvg_handle_new_from_file (path, &err);
	g_free (path);
	if (err) { 
		g_clear_error (&err);
	}
	else {
		cairo_save (cr); 
		cairo_scale (cr, 0.9*frame_get_width (f)/FRAME_WIDTH_NORMAL, 0.9*frame_get_height(f)/FRAME_HEIGHT_NORMAL );			
		rsvg_handle_render_cairo (myhandle, cr);
		cairo_restore (cr);
		g_object_unref (myhandle);
		
		title = osiris_graphic_title (f); 
		cairo_set_font_size (cr, frame_get_height (f)*0.05);
		cairo_move_to (cr, frame_get_width (f)*0.1, frame_get_height (f)*0.05);		
		cairo_show_text (cr, title);
		g_free (title);
	}	


	
	return 0;
}


int button_press_handler (frame_t *f, GdkEventButton *button)
{
	return 0;
}

int button_release_handler (frame_t *f, GdkEventButton *button)
{
	return 0;
}
