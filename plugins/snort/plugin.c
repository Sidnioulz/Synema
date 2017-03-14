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
 * \brief Fake Snort plugin written by the twentieth group
 * \author Vincent Jaouen <vincent.jaouen@ensi-bourges.fr>
 * \author Bénédicte Augustin <benedicte.augustin@ensi-bourges.com>
 *
 * This file contains a fake Snort plugin for demonstration purpose.
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
#include "file_manip.h"
#include "time-period.h"
#include "position.h"
#include "fonctions.h"


int button_press_handler (frame_t *f, GdkEventButton *event)
{
	data_affichage * data = (data_affichage *) f->priv;

	GtkMenuItem * item=NULL;

        if(data->continent == WORLD && data->frame_size == NORMAL) {
            if(event->x <= 180 && event->x >= 126 && event->y >= 52 && event->y <= 115)
                continent_is_europe(item, (gpointer) f);
            else if(event->x <= 100 && event->x >= 20 && event->y >= 13 && event->y <= 140)
                continent_is_north_america(item, (gpointer) f);
            else if(event->x <= 120 && event->x >= 75 && event->y >= 149 && event->y <= 220)
                continent_is_south_america(item, (gpointer) f);
            else if(event->x <= 185 && event->x >= 125 && event->y >= 115 && event->y <= 200)
                continent_is_africa(item, (gpointer) f);
            else if(event->x <= 290 && event->x >= 180 && event->y >= 15 && event->y <= 125)
                continent_is_asia(item, (gpointer) f);
            else if(event->x <= 290 && event->x >= 225 && event->y >= 152 && event->y <= 214)
                continent_is_oceania(item, (gpointer) f);
        }

        else if(data->continent == WORLD && data->frame_size == LARGE) {
            if(event->x <= 340 && event->x >= 250 && event->y >= 90 && event->y <= 225)
                continent_is_europe(item, (gpointer) f);
            else if(event->x <= 210 && event->x >= 20 && event->y >= 8 && event->y <= 280)
                continent_is_north_america(item, (gpointer) f);
            else if(event->x <= 240 && event->x >= 160 && event->y >= 280 && event->y <= 450)
                continent_is_south_america(item, (gpointer) f);
            else if(event->x <= 370 && event->x >= 250 && event->y >= 225 && event->y <= 390)
                continent_is_africa(item, (gpointer) f);
            else if(event->x <= 560 && event->x >= 340 && event->y >= 50 && event->y <= 260)
                continent_is_asia(item, (gpointer) f);
            else if(event->x <= 550 && event->x >= 450 && event->y >= 300 && event->y <= 420)
                continent_is_oceania(item, (gpointer) f);
        }
        else 
        	continent_is_world(item, (gpointer) f);

	frame_request_content_refresh (f);
	return 0;
}

int button_release_handler (frame_t *f, GdkEventButton *button)
{
	return 0;
}


char *get_display_name (frame_t *f)
{
	return "Snort";
}


void continent_is_africa (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_affichage * data = (data_affichage *) f->priv;

	data->type = TYPE_GEOLOCALISATION;	
	data->continent = AFRICA;
	data->name_background_file = frame_get_absolute_data_path (f, "africa.png");
	data->name_foreground_file = frame_get_absolute_tmp_path (f, "ploticus.png");

	create_ploticus_image(user_data);

	frame_request_content_refresh (f);
}

void continent_is_north_america (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_affichage * data = (data_affichage *) f->priv;

	data->type = TYPE_GEOLOCALISATION;	
	data->continent = NORTH_AMERICA;
	data->name_background_file = frame_get_absolute_data_path (f, "north_america.png");
	data->name_foreground_file = frame_get_absolute_tmp_path (f, "ploticus.png");

	create_ploticus_image(user_data);

	frame_request_content_refresh (f);
}

void continent_is_south_america (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_affichage * data = (data_affichage *) f->priv;

	data->type = TYPE_GEOLOCALISATION;	
	data->continent = SOUTH_AMERICA;
	data->name_background_file = frame_get_absolute_data_path (f, "south_america.png");
	data->name_foreground_file = frame_get_absolute_tmp_path (f, "ploticus.png");

	create_ploticus_image(user_data);
	frame_request_content_refresh (f);
}

void continent_is_oceania (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_affichage * data = (data_affichage *) f->priv;

	data->type = TYPE_GEOLOCALISATION;	
	data->continent = OCEANIA;
	data->name_background_file = frame_get_absolute_data_path (f, "oceania.png");
	data->name_foreground_file = frame_get_absolute_tmp_path (f, "ploticus.png");

	create_ploticus_image(user_data);
	frame_request_content_refresh (f);
}

void continent_is_middle_east (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_affichage * data = (data_affichage *) f->priv;

	data->type = TYPE_GEOLOCALISATION;	
	data->continent = MIDDLE_EAST;
	data->name_background_file = frame_get_absolute_data_path (f, "middle_east.png");
	data->name_foreground_file = frame_get_absolute_tmp_path (f, "ploticus.png");

	create_ploticus_image(user_data);
	frame_request_content_refresh (f);
}

void continent_is_asia (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_affichage * data = (data_affichage *) f->priv;

	data->type = TYPE_GEOLOCALISATION;	
	data->continent = ASIA;
	data->name_background_file = frame_get_absolute_data_path (f, "asia.png");
	data->name_foreground_file = frame_get_absolute_tmp_path (f, "ploticus.png");

	create_ploticus_image(user_data);

	frame_request_content_refresh (f);
}

void continent_is_europe (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_affichage * data = (data_affichage *) f->priv;

	data->type = TYPE_GEOLOCALISATION;
	data->continent = EUROPE;
	data->name_background_file = frame_get_absolute_data_path (f, "europe.png");
	data->name_foreground_file = frame_get_absolute_tmp_path (f, "ploticus.png");

	create_ploticus_image(user_data);

	frame_request_content_refresh (f);
}

void continent_is_world (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_affichage *data = (data_affichage *) f->priv;

	data->type = TYPE_GEOLOCALISATION;
	data->continent = WORLD;
	data->name_background_file = frame_get_absolute_data_path (f, "world.png");
	data->name_foreground_file = frame_get_absolute_tmp_path (f, "ploticus.png");

	create_ploticus_image(user_data);

	frame_request_content_refresh (f);
}


void call_script_priorite (GtkMenuItem * item, gpointer user_data)
{
	script_priorite (user_data);
}

void call_script_attacks (GtkMenuItem * item, gpointer user_data)
{
	script_attacks (user_data);
}

GList *build_func_list ()
{
	GList *list = NULL;
	GList *children_list_geoloc = NULL;
	GList *children_list_attacks = NULL;
	func_list_add_new_func (&children_list_geoloc, "Asia", continent_is_asia);
	func_list_add_new_func (&children_list_geoloc, "Oceania", continent_is_oceania);
	func_list_add_new_func (&children_list_geoloc, "North America", continent_is_north_america);
	func_list_add_new_func (&children_list_geoloc, "South America", continent_is_south_america);
	func_list_add_new_func (&children_list_geoloc, "Europe", continent_is_europe);
	func_list_add_new_func (&children_list_geoloc, "Africa", continent_is_africa);
	func_list_add_new_func (&children_list_geoloc, "World", continent_is_world);
	func_list_add_new_submenu (&list, "Geolocalisation", children_list_geoloc);
	func_list_add_new_separator (&list);
	func_list_add_new_func (&children_list_attacks, "Priority", call_script_priorite);
	func_list_add_new_func (&children_list_attacks, "Types of attacks", call_script_attacks);
	func_list_add_new_submenu (&list, "Attacks", children_list_attacks);

	return list;
}



GList *build_time_periods ()
{
	GList *list = NULL;
	time_period_t *p1, *p2, *p3, *p4;
	
	p1 = time_period_new (WEEK, 2);
	list = g_list_prepend (list, p1);
	
	p2 = time_period_new (DAY, 1);
	list = g_list_prepend (list, p2);
	
	p3 = time_period_new (HOUR, 2);
	list = g_list_prepend (list, p3);
	
	p4 = time_period_new (LIVE, 0);
	list = g_list_prepend (list, p4);
	
	return list;
}



int is_refreshing_needed (frame_t *f)
{
	int refreshing_needed = !(rand() % 5);
	char *path = frame_get_absolute_reports_path (f, "mon_rapport.txt", NULL);

	//You must now stat path to get it's mtime. Make sure to manage errors properly.

	g_free (path);
	return refreshing_needed;
//	return 1;
}



int refresh_content (frame_t *f)
{	
	data_affichage * data = (data_affichage *) f->priv;

	if (f->compute) {
		if (data->type == TYPE_GEOLOCALISATION)
		{
			create_ploticus_image ((gpointer) f);
		}
		frame_request_redraw (f);
	}

	return 0;
}



int init_private (frame_t *f)
{
	f->priv = malloc (sizeof (data_affichage));
	data_affichage * data = (data_affichage *) f->priv;

	data->type = TYPE_GEOLOCALISATION;
	data->continent = WORLD;
	data->name_background_file = frame_get_absolute_data_path (f, "world.png");
	data->name_foreground_file = frame_get_absolute_tmp_path (f, "world.png");
	data->name_report_file = get_report_name (f);
  	data->frame_size = f->size;

	create_ploticus_image((gpointer) f);
	frame_request_content_refresh (f);

	return 0;
}



int free_private (frame_t *f)
{
	data_affichage *data = (data_affichage *) f->priv;

	free(data->name_background_file);
	free(data->name_foreground_file);

	free(data->name_report_file);
	free(data);

	return 0;
}


int restore_private (frame_t *f, GKeyFile *file)
{
	data_affichage *data = (data_affichage *) f->priv;
	GError *err = NULL;
			
	data->name_background_file = g_key_file_get_string (file, f->get_display_name (f), "Background file", &err);
	if (err) {
		fprintf (stderr, "Could not load the Background file (%s)\n", err->message);
		g_clear_error (&err);
	}

	data->name_foreground_file = g_key_file_get_string (file, f->get_display_name (f), "Foreground file", &err);
	if (err) {
		fprintf (stderr, "Could not load the foreground file (%s)\n", err->message);
		g_clear_error (&err);
	}

	
	data->name_report_file = g_key_file_get_string (file, f->get_display_name (f), "Report file", &err);
	if (err) {
		fprintf (stderr, "Could not load the report file (%s)\n", err->message);
		g_clear_error (&err);
	}

	data->type = g_key_file_get_integer (file, f->get_display_name (f), "Type", &err);
	if (err) {
		fprintf (stderr, "Could not load the type (%s)\n", err->message);
		g_clear_error (&err);
	}
	
	data->continent = g_key_file_get_integer (file, f->get_display_name (f), "Continent", &err);
	if (err) {
		fprintf (stderr, "Could not load the continent (%s)\n", err->message);
		g_clear_error (&err);
	}

	data->attack_type = g_key_file_get_integer (file, f->get_display_name (f), "Attack type", &err);
	if (err) {
		fprintf (stderr, "Could not load the attack type (%s)\n", err->message);
		g_clear_error (&err);
	}

	data->frame_size = g_key_file_get_integer (file, f->get_display_name (f), "Frame size", &err);
	if (err) {
		fprintf (stderr, "Could not load the frame size (%s)\n", err->message);
		g_clear_error (&err);
	}

	return 0;
}


int save_private(frame_t *f, GKeyFile *file)
{
	data_affichage 		*data 		= (data_affichage *) f->priv;

	g_key_file_set_string (file, f->get_display_name (f), "Background file" ,data->name_background_file);
	g_key_file_set_string (file, f->get_display_name (f), "Foreground file" ,data->name_foreground_file);
	g_key_file_set_string (file, f->get_display_name (f), "Report file" ,data->name_report_file);
	g_key_file_set_integer (file, f->get_display_name (f), "Type" ,data->type);
	g_key_file_set_integer (file, f->get_display_name (f), "Continent" ,data->continent);
	g_key_file_set_integer (file, f->get_display_name (f), "Attack type" ,data-> attack_type);
	g_key_file_set_integer (file, f->get_display_name (f), "Frame size" ,data->frame_size);
			
	return 0;
}


int display_machine_changed (frame_t *f)
{
	data_affichage *data = (data_affichage *) f->priv;
	data->name_report_file = get_report_name (f);

	if (data->type == TYPE_GEOLOCALISATION) {
		create_ploticus_image ((gpointer) f);
	}
	else { 
		if (data->attack_type == PRIORITY)
			script_priorite ((gpointer) f);
		else
			script_attacks ((gpointer) f);
	}
	frame_request_content_refresh (f);
	return 0;
}



int time_period_changed (frame_t *f)
{
	data_affichage *data = (data_affichage *) f->priv;
	data->name_report_file = get_report_name (f);

	if (data->type == TYPE_GEOLOCALISATION) {
		create_ploticus_image ((gpointer) f);
	}
	else { 
		if (data->attack_type == PRIORITY)
			script_priorite ((gpointer) f);
		else
			script_attacks ((gpointer) f);
	}
	frame_request_content_refresh (f);

	return 0;
}



int frame_size_changed (frame_t *f)
{
	data_affichage *data = (data_affichage *) f->priv;
	data->frame_size = f->size;

	frame_request_content_refresh (f);
	return 0;
}


int draw_area (frame_t *f, cairo_t *cr)
{
	data_affichage 			*data 		= (data_affichage *) f->priv;

	cairo_surface_t			*image		= NULL;
	double				tmp1, tmp2;
	int				h, w;
    
	char 				*path		= NULL;
	GError				*err		= NULL;
	RsvgHandle			*myhandle	= NULL;	
	char 				*title		= NULL;

	path = frame_get_absolute_tmp_path(f, "img.svg");
	if (data->type == TYPE_GEOLOCALISATION) { // Display the background if Geolocalisation is required
		image = cairo_image_surface_create_from_png (data->name_background_file);
    
		h = cairo_image_surface_get_height (image);
		w = cairo_image_surface_get_width (image);

		tmp1 = frame_get_height (f) / h;
        	tmp2 = frame_get_width (f) / w;
    		cairo_save (cr);
    		cairo_scale (cr, tmp2, tmp1);
		cairo_set_source_surface (cr, image, 0, 0);
    		cairo_paint (cr);
    		cairo_surface_destroy (image);
    		cairo_restore (cr);

		image = cairo_image_surface_create_from_png (data->name_foreground_file);
		h = cairo_image_surface_get_height (image);

        	w = cairo_image_surface_get_width (image) - 53;

		tmp1 = frame_get_width (f)/ w;
		tmp2 = frame_get_height (f) / h;

		cairo_save (cr);
		cairo_scale (cr, tmp1, tmp2);
		cairo_set_source_surface (cr, image, 0, 0);
		cairo_paint (cr);
		cairo_surface_destroy (image);
		cairo_restore (cr);
	}
	else {
		myhandle = rsvg_handle_new_from_file (path, &err);
		g_free (path);
		if (err) { 
			g_clear_error (&err);
		}
		else {
			cairo_save (cr); 
			cairo_scale (cr, frame_get_width (f)*0.5, frame_get_height (f)*0.8);			
			cairo_translate (cr, 0.52, 0.2);
			cairo_rotate (cr, M_PI);
			rsvg_handle_render_cairo (myhandle, cr);
			cairo_restore (cr);
			g_object_unref (myhandle);
		}
	}

	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);	
	title = snort_graphic_title (f); 
	cairo_set_font_size (cr, frame_get_height (f)*0.05);
	cairo_move_to (cr, frame_get_width (f)*0.1, frame_get_height (f)*0.05);		
	cairo_show_text (cr, title);
	g_free (title);

	return 0;
}


char * snort_graphic_title (frame_t *f)
{
	data_affichage	*data 		= (data_affichage *) f->priv;
	char 		*title		= NULL;

	if (data->type == TYPE_GEOLOCALISATION) {
		if (data->continent == AFRICA)
			title = g_strdup("Africa: Incoming attacks");
		else if (data->continent == NORTH_AMERICA)
			title = g_strdup("North America: Incoming attacks");
		else if (data->continent == SOUTH_AMERICA)
			title = g_strdup("South America: Incoming attacks");
		else if (data->continent == OCEANIA)
			title = g_strdup("Oceania: Incoming attacks");
		else if (data->continent == MIDDLE_EAST)
			title = g_strdup("Middle east: Iincoming attacks");
		else if (data->continent == ASIA)
			title = g_strdup("Asia: Incoming attacks");
		else if (data->continent == EUROPE)
			title = g_strdup("Europe: Incoming attacks");
		else 
			title = g_strdup("World: Incoming attacks");
	}
	else {
		if (data->attack_type == PRIORITY)
			title = g_strdup("Priority of attacks");
		else
			title = g_strdup("Type of attacks");
	}

	return title;
}

