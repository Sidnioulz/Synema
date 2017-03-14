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
 * \brief Fake Bash History plugin written by the twentieth group
 * \author Steve Dodier <sidnioulz@gmail.com>
 * \author Timothée Ravier <timothee.romain.ravier@gmail.com>
 *
 * This file contains a fake Bash History plugin for demonstration purpose.
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

char * bash_history_graphic_title (frame_t *f)
{
	data_t 		*data 		= (data_t *) f->priv;
	char 		*title		= NULL;
	if (data->entier == 0)
		title = g_strdup("Alerts by type");
	else if (data->entier == 1)
		title = g_strdup("Communication");
	else if (data->entier == 2)
		title = g_strdup("Files download");
	else if (data->entier == 3)
		title = g_strdup("Files edition");
	else if (data->entier == 4)
		title = g_strdup("Files manipulation");
	else if (data->entier == 5)
		title = g_strdup("Files unarchiving");
	else if (data->entier == 6)
		title = g_strdup("Forcing");
	else if (data->entier == 7)
		title = g_strdup("Local rebound");
	else if (data->entier == 8)
		title = g_strdup("Machine administration");
	else if (data->entier == 9)
		title = g_strdup("Machine inspection");
	else if (data->entier == 10)
		title = g_strdup("Program execution");
	else 
		title = g_strdup("Source code compilation");
	
	return title;
}

char *get_display_name (frame_t *f)
{
	return "Bash History";
}



void communication_tab (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 1;
}

void files_download_tab (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 2;
}

void files_edition_tab (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 3;
}

void files_manipulation_tab (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 4;
}

void files_unarchiving_tab (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 5;
}

void forcing_tab (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 6;
}

void local_rebound_tab (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 7;
}

void machine_administration_tab (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 8;
}

void machine_inspection_tab (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 9;
}

void program_execution_tab (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 10;
}

void source_code_compilation_tab (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 11;
}

void PART_tab (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->entier = 0;
}



GList *build_func_list ()
{
	GList *list = NULL;

	func_list_add_new_func (&list, "Source code compilation", source_code_compilation_tab);
	func_list_add_new_func (&list, "Program execution", program_execution_tab);
	func_list_add_new_func (&list, "Machine inspection", machine_inspection_tab);
	func_list_add_new_func (&list, "Machine administration", machine_administration_tab);
	func_list_add_new_func (&list, "Local rebound", local_rebound_tab);
	func_list_add_new_func (&list, "Forcing", forcing_tab);
	func_list_add_new_func (&list, "Files unarchiving", files_unarchiving_tab);
	func_list_add_new_func (&list, "Files manipulation", files_manipulation_tab);
	func_list_add_new_func (&list, "Files edition", files_edition_tab);
	func_list_add_new_func (&list, "Files download", files_download_tab);
	func_list_add_new_func (&list, "Communication", communication_tab);
	func_list_add_new_separator (&list);
	func_list_add_new_func (&list, "Alerts by type", PART_tab);
	

	return list;
}



GList *build_time_periods ()
{
	GList *list = NULL;
	time_period_t *p1, *p2, *p3, *p4;

	p1 = time_period_new (WEEK, 2);
	list = g_list_prepend (list, p1);

	p2 = time_period_new (DAY, 2);
	list = g_list_prepend (list, p2);

	p3 = time_period_new (HOUR, 6);
	list = g_list_prepend (list, p3);

	p4 = time_period_new (LIVE, 0);
	list = g_list_prepend (list, p4);

	return list;
}



int is_refreshing_needed (frame_t *f)
{
	int refreshing_needed = !(rand() % 1);
	char *path = frame_get_absolute_reports_path (f, "report.txt", NULL);

	g_free (path);
	return refreshing_needed;
}



int refresh_content (frame_t *f)
{
	// All the code should be embed in if (f->compute) blocks. This is to ensure
	// the function will quit as fast as possible if it's asked not to compute
	// anymore.
	char *command=NULL;
	char *path=frame_get_absolute_tmp_path (f,"image.svg");
	char *report_path=NULL;
	data_t *data = (data_t *) f->priv;

	switch (data->entier)
	{	
		case 0:
			report_path=frame_get_absolute_reports_path (f,"report.txt",NULL);
			command=g_strdup_printf("ploticus -prefab pie explode=0 colors=auto labelfarout=1.3 -svg -o %s data=%s labelfmtstring=\"@1\\n(@PCT%%)\" values=2",path,report_path);
			break;

		case 1:
			report_path=frame_get_absolute_reports_path (f,"report_communication.txt",NULL);
			command=g_strdup_printf("ploticus -prefab pie explode=0 colors=auto -svg -o %s data=%s labelfmtstring=\"@1\\n(@PCT%%)\" values=2",path,report_path);
			break;

		case 2:
			report_path=frame_get_absolute_reports_path (f,"report_files_download.txt",NULL);
			command=g_strdup_printf("ploticus -prefab pie explode=0 colors=auto -svg -o %s data=%s labelfmtstring=\"@1\\n(@PCT%%)\" values=2",path,report_path);
			break;

		case 3:
			report_path=frame_get_absolute_reports_path (f,"report_files_edition.txt",NULL);
			command=g_strdup_printf("ploticus -prefab pie explode=0 colors=auto -svg -o %s data=%s labelfmtstring=\"@1\\n(@PCT%%)\" values=2",path,report_path);
			break;

		case 4:
			report_path=frame_get_absolute_reports_path (f,"report_files_manipulation.txt",NULL);
			command=g_strdup_printf("ploticus -prefab pie explode=0 colors=auto -svg -o %s data=%s labelfmtstring=\"@1\\n(@PCT%%)\" values=2",path,report_path);
			break;

		case 5:
			report_path=frame_get_absolute_reports_path (f,"report_files_unarchiving.txt",NULL);
			command=g_strdup_printf("ploticus -prefab pie explode=0 colors=auto -svg -o %s data=%s labelfmtstring=\"@1\\n(@PCT%%)\" values=2",path,report_path);
			break;

		case 6:
			report_path=frame_get_absolute_reports_path (f,"report_forcing.txt",NULL);
			command=g_strdup_printf("ploticus -prefab pie explode=0 colors=auto -svg -o %s data=%s labelfmtstring=\"@1\\n(@PCT%%)\" values=2",path,report_path);
			break;

		case 7:
			report_path=frame_get_absolute_reports_path (f,"report_local_rebound.txt",NULL);
			command=g_strdup_printf("ploticus -prefab pie explode=0 colors=auto -svg -o %s data=%s labelfmtstring=\"@1\\n(@PCT%%)\" values=2",path,report_path);
			break;

		case 8:
			report_path=frame_get_absolute_reports_path (f,"report_machine_administration.txt",NULL);
			command=g_strdup_printf("ploticus -prefab pie explode=0 colors=auto -svg -o %s data=%s labelfmtstring=\"@1\\n(@PCT%%)\" values=2",path,report_path);
			break;

		case 9:
			report_path=frame_get_absolute_reports_path (f,"report_machine_inspection.txt",NULL);
			command=g_strdup_printf("ploticus -prefab pie explode=0 colors=auto -svg -o %s data=%s labelfmtstring=\"@1\\n(@PCT%%)\" values=2",path,report_path);
			break;

		case 10:
			report_path=frame_get_absolute_reports_path (f,"report_program_execution.txt",NULL);
			command=g_strdup_printf("ploticus -prefab pie explode=0 colors=auto -svg -o %s data=%s labelfmtstring=\"@1\\n(@PCT%%)\" values=2",path,report_path);
			break;

		case 11:
			report_path=frame_get_absolute_reports_path (f,"report_source_code_compilation.txt",NULL);
			command=g_strdup_printf("ploticus -prefab pie explode=0 colors=auto -svg -o %s data=%s labelfmtstring=\"@1\\n(@PCT%%)\" values=2",path,report_path);
			break;

		default:
			return 1;
	}

	system (command);
	frame_request_redraw (f);

	return 0;
}



int init_private (frame_t *f)
{
	f->priv = malloc (sizeof (data_t));
	// Recommended to access f->priv
	data_t *data = (data_t *) f->priv;

	data->entier = 0;

	return 0;
}



int free_private (frame_t *f)
{
	// Recommended to access f->priv
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



		cairo_set_source_rgb(cr, 1, 1, 1);
		cairo_paint(cr);		
		cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);	
		cairo_set_font_size (cr, frame_get_height (f)*0.07);
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		cairo_move_to(cr, frame_get_width (f)*0.30, frame_get_height (f)*0.35);
	
		path = frame_get_absolute_tmp_path (f, "image.svg");
		myhandle = rsvg_handle_new_from_file (path, &err);
		g_free (path);
		if (err) { 
			g_clear_error (&err);
		}
		else {
			cairo_save (cr); 
				cairo_scale (cr, 0.9*frame_get_width (f)/FRAME_WIDTH_NORMAL, 0.9*frame_get_height(f)/FRAME_HEIGHT_NORMAL );
			//	cairo_translate (cr, 0.55, 0.2);
			//cairo_rotate (cr, M_PI);
			rsvg_handle_render_cairo (myhandle, cr);
			cairo_restore (cr);
			g_object_unref (myhandle);
	
			title = bash_history_graphic_title (f); 
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
