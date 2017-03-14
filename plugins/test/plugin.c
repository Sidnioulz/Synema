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



char *get_display_name (frame_t *f)
{
	return "Testing";
}



GList *build_func_list ()
{
	return NULL;
}



GList *build_time_periods ()
{
	GList *list = NULL;
	time_period_t *p1, *p2;
	
	p1 = time_period_new (MONTH, 6);
	list = g_list_prepend (list, p1);
	
	p2 = time_period_new (LIVE, 0);
	list = g_list_prepend (list, p2);
	
	return list;
}



int is_refreshing_needed (frame_t *f)
{
	int refreshing_needed = !(rand() % 5);
	char *path = frame_get_absolute_reports_path (f, "mon_rapport.txt", NULL);

	// TODO You must now stat path to get it's mtime. Make sure to manage errors properly.

	g_free (path);
	return refreshing_needed;
}



int refresh_content (frame_t *f)
{
	// TODO system(ploticus) here

	return 0;
}



int init_private (frame_t *f)
{
	f->priv = malloc (sizeof (data_t));
	// Recommended to access f->priv
	data_t *data = (data_t *) f->priv;
	
	data->chaine = strdup ("Hello kitty.\n");
	data->entier = 42;
	
	return 0;
}



int restore_private (frame_t *f, GKeyFile *file)
{
	return 0;
}



// Install 'devhelp' and 'libgtk2.0-doc' to get documentation about g_key_file
int save_private (frame_t *f, GKeyFile *file)
{
	return 0;
}



int free_private (frame_t *f)
{
	// Recommended to access f->priv
	data_t *data = (data_t *) f->priv;
	
/*	printf ("Here I do not forget to free all the memory I allocated.\n");*/
	
	free (data->chaine);
	free (data);

	return 0;
}



int display_machine_changed (frame_t *f)
{
/*	printf ("Here, I must change my internal data to reflect the machine change.\n");*/
	frame_request_content_refresh (f);
	return 0;
}



int time_period_changed (frame_t *f)
{
/*	printf ("Here, I must change my internal data to reflect the time period change.\n");*/
	frame_request_content_refresh (f);
	return 0;
}



int frame_size_changed (frame_t *f)
{
/*	printf ("Here, I must change my internal data to reflect the frame size change.\n");*/
	frame_request_content_refresh (f);
	return 0;
}

int draw_area (frame_t *f, cairo_t *cr)
{
 	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
 	cairo_set_font_size (cr, frame_get_height (f)*0.06);
 	cairo_set_source_rgb (cr, 0.6, 0.4, 0.4);
 	cairo_move_to (cr, 30.0, 30.0);
    cairo_show_text (cr, "Hello World");

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
