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
 * \brief Fake plugin written by the twentieth group
 * \author Steve Dodier <sidnioulz@gmail.com>
 * \author Timothée Ravier <timothee.romain.ravier@gmail.com>
 *
 * This file contains a fake plugin for demonstration purpose.
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



char * mod_security_graphic_title (frame_t *f)
{
	data_t 		*data 		= (data_t *) f->priv;
	char 		*title		= NULL;
	if (data->entier == 0)
		title = g_strdup("Attacks severity");
	else if (data->entier == 1)
		title = g_strdup("Types of attacks");
	else  
		title = g_strdup("Time of attacks");
	
	return title;
}

char *get_display_name (frame_t *f)
{
	return "Mod Security";
}


void plot_call(frame_t *f, char *plot_script)
{
	char *logdir = frame_get_absolute_reports_path (f, NULL, NULL);
	char *command=NULL;
	char *out_path=frame_get_absolute_tmp_path (f,"out.svg");
	char *path = frame_get_absolute_data_path (f,plot_script);
		
	command=g_strdup_printf("ploticus %s -dir %s -svg -o %s", path, logdir, out_path);

	system(command);
}


void attacks_severity(GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 0;
}



void type_of_attacks(GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;
	
	data->entier = 1;
}



void when_attacks_occur(GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 2;
}




// Here some functions for future versions which could be used
// for some further options, like dynamic scripts. (Not finished)

/*
void generate_as(GtkMenuItem *item, gpointer user_data)
{
	
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;
	
	char *path=NULL;
	path = frame_get_absolute_data_path (f, "as");

FILE* as;
as=fopen(path, "w+");


fprintf(as, "#proc page");
fprintf(as, "  pagesize: 20 4.5");
fprintf(as, "  #if @DEVICE in gif,png");
fprintf(as, "    scale: 0.5");
fprintf(as, "  #endif");
fprintf(as, "  title: Attacks Severity\n\n");


fprintf(as, "// Set up plotting area using proc areadef");
fprintf(as, "#proc areadef");
fprintf(as, "  title: Attacks Severity");
fprintf(as, "  rectangle: 1 1 4 4");
fprintf(as, "  xscaletype: time hh:mm");
fprintf(as, "  xrange: 00:01 16:21");
fprintf(as, "  yrange: 0 50");
fprintf(as, "  xaxis.stubs: inc 1 hour");
fprintf(as, "  xaxis.stubformat: h");
fprintf(as, "  xaxis.autoyears: yes");
fprintf(as, "  yaxis.stubs: inc 10");
fprintf(as, "  yaxis.labeldetails: adjust=-0.3,0\n\n");

fprintf(as, "// Read and process data file using proc getdata");
fprintf(as, "#proc getdata");
fprintf(as, "  file: log.dat");

fprintf(as, "#proc tabulate");
fprintf(as, "  axis1: x");
fprintf(as, "  datafield1: 1");
fprintf(as, "  datafield2: 2");
fprintf(as, "  valuelist2: Emergency, Alert, Critical, Error, Notice, Info");
fprintf(as, "  savetable: log.dat");

fprintf(as, "// draw bars for Critical Severity using proc bars");
fprintf(as, "#proc bars");
fprintf(as, "  lenfield: 2");
fprintf(as, "  locfield: 1");
fprintf(as, "  color: red");
fprintf(as, "  outline: no");
fprintf(as, "  barsrange: 00:01 16:21");
fprintf(as, "  legendlabel: Critical");
fprintf(as, "  barwidth: 0.07");
fprintf(as, "  outline: yes");
fprintf(as, "  #saveas B\n");

fprintf(as, "// draw bars for Alerts Severity, stacking them above Criticals, using proc bars");
fprintf(as, "#proc bars");
fprintf(as, "  #clone: B");
fprintf(as, "  lenfield: 3");
fprintf(as, "  color: orange");
fprintf(as, "  stackfields: 2");
fprintf(as, "legendlabel: Alerts\n");

fprintf(as, "// draw bars for Low Severity, stacking them above the others, using proc bars");
fprintf(as, "//#proc bars");
fprintf(as, "//  #clone: B");
fprintf(as, "//  lenfield: 4");
fprintf(as, "//  color: green");
fprintf(as, "//  stackfields: 2 3");
fprintf(as, "//  legendlabel: Low\n");

fprintf(as, "// render legend using labels defined above (proc legend)");
fprintf(as, "#proc legend");
fprintf(as, "  location: min+0.3 max");
fprintf(as, "  reverseorder: yes");
fprintf(as, "  seglen: 0.3");


fclose(as);

}



void generate_wao()
{

	char *path = frame_get_absolute_reports_path (f, "script", NULL);

*fopen(w+)
}



void generate_toa()
{

	char *path = frame_get_absolute_reports_path (f, "script", NULL);

*fopen(w+)
}
*/






int draw_area (frame_t *f, cairo_t *cr)
{

	char 				*path		= NULL;
	char 				*title		= NULL;
	data_t 				*data 		= (data_t *) f->priv;
	GError				*err		= NULL;
	RsvgHandle			*myhandle	= NULL;	

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);		
	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);	
	cairo_set_font_size (cr, frame_get_height (f)*0.07);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_move_to(cr, frame_get_width (f)*0.30, frame_get_height (f)*0.35);

	path = frame_get_absolute_tmp_path (f, "out.svg");
	myhandle = rsvg_handle_new_from_file (path, &err);
	g_free (path);
	if (err) { 
		g_clear_error (&err);
	}
	else {
		cairo_save (cr); 
		cairo_scale (cr, frame_get_width (f)/FRAME_WIDTH_NORMAL, 0.8*frame_get_height(f)/FRAME_HEIGHT_NORMAL );
			//cairo_translate (cr, 0.55, 0.2);
		rsvg_handle_render_cairo (myhandle, cr);
		cairo_restore (cr);
		g_object_unref (myhandle);
		
		title = mod_security_graphic_title (f); 
		cairo_set_font_size (cr, frame_get_height (f)*0.05);
		cairo_move_to (cr, frame_get_width (f)*0.1, frame_get_height (f)*0.05);		
		cairo_show_text (cr, title);
		g_free (title);
	}

	return 0;
}



GList *build_func_list ()
{
	GList *list = NULL;

	func_list_add_new_func (&list, "When Attacks Occur", when_attacks_occur);
	func_list_add_new_func (&list, "Types of Attacks", type_of_attacks);
	func_list_add_new_func (&list, "Attacks Severity", attacks_severity);

	return list;
}



GList *build_time_periods ()
{
	GList *list = NULL;
	time_period_t *p1, *p2, *p3, *p4;
	
	p1 = time_period_new (MONTH, 1);
	list = g_list_prepend (list, p1);
	
	p2 = time_period_new (WEEK, 1);
	list = g_list_prepend (list, p2);
	
	p3 = time_period_new (DAY, 1);
	list = g_list_prepend (list, p3);
	
	p4 = time_period_new (LIVE, 0);
	list = g_list_prepend (list, p4);
		
	return list;
}



int is_refreshing_needed (frame_t *f)
{
	//int refreshing_needed = !(rand() % 5);
	char *path = frame_get_absolute_reports_path (f, "mon_rapport.txt", NULL);

	//You must now stat path to get it's mtime. Make sure to manage errors properly.

	g_free (path);
	//return refreshing_needed;
	return 1;
}



int refresh_content (frame_t *f)
{
	data_t *data = (data_t *) f->priv;
		
	if (data->entier == 0) {
		if (f->display_period.type == LIVE)
			{plot_call(f, "as");}
		else if (f->display_period.type == DAY)
			{plot_call(f, "asd");}
		else if (f->display_period.type == WEEK)
			{plot_call(f, "asw");}
		else if (f->display_period.type == MONTH)
			{plot_call(f, "asm");}
	}
	else if (data->entier == 1) {
		if (f->display_period.type == LIVE)
			{plot_call(f, "toa");}
		else if (f->display_period.type == DAY)
			{plot_call(f, "toad");}
		else if (f->display_period.type == WEEK)
			{plot_call(f, "toaw");}
		else if (f->display_period.type == MONTH)
			{plot_call(f, "toam");}
	}
	else if (data->entier == 2) {
		if (f->display_period.type == LIVE)
			{plot_call(f, "wao");}
		else if (f->display_period.type == DAY)
			{plot_call(f, "waod");}
		else if (f->display_period.type == WEEK)
			{plot_call(f, "waow");}
		else if (f->display_period.type == MONTH)
			{plot_call(f, "waom");}
	}
		
	frame_request_redraw (f);
	
	return 0;
}



int init_private (frame_t *f)
{
	f->priv = malloc (sizeof (data_t));
	data_t *data = (data_t *) f->priv;
	
	data->entier = 1;

	return 0;
}


int restore_private (frame_t *f, GKeyFile *file)
{
	data_t *data = (data_t *) f->priv;
	GError *err = NULL;

	data->entier = g_key_file_get_integer (file, f->get_display_name (f), "Graph type", &err);
	if (err) {
		fprintf (stderr, "Could not load the graph type (%s)\n", err->message);
		g_clear_error (&err);
	}

	return 0;
}


int save_private (frame_t *f, GKeyFile *file)
{
	data_t *data = (data_t *) f->priv;

	g_key_file_set_integer (file, f->get_display_name (f), "Graph type", data->entier);

	return 0;
}


int free_private (frame_t *f)
{
	data_t *data = (data_t *) f->priv;
	free (data);

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


int button_press_handler (frame_t *f, GdkEventButton *button)
{
	return 0;
}

int button_release_handler (frame_t *f, GdkEventButton *button)
{
	return 0;
}
