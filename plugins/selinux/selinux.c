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
 * \file selinux.c
 * \brief SELinux plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 * \author Zaina AFOULKI <zaina.afoulki@ensi-bourges.fr>
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
#include "selinux.h"
#include "menu_functions.h"


char * colors[10] = {"blue","red", "yellow", "orange", "green", "black", "darkblue", "magenta", "claret", "purple"};

char * selinux_graphic_title (frame_t * f)
{
	data_t 		*data 		= (data_t *) f->priv;
	char 		*title		= NULL;
	if (g_strrstr(data->ploticus_script, "machines")!=NULL)
	    title = g_strdup("Alerts By machine");
	else if (g_strrstr(data->ploticus_script, "type")!=NULL)
	    title = g_strdup("Alerts By Type");
	else if (g_strrstr(data->ploticus_script, "user")!=NULL)
	    title = g_strdup("Alerts By User");
	else if (g_strrstr(data->ploticus_script, "tclass")!=NULL)
	    title = g_strdup("Alerts By tclass");
	else if (g_strrstr(data->ploticus_script, "result")!=NULL)
	    title = g_strdup("Alerts By result");
	else if (g_strrstr(data->ploticus_script, "tcontext_r")!=NULL)
	    title = g_strdup("Alerts By tcontext_r");
	else if (g_strrstr(data->ploticus_script, "tcontext_u")!=NULL)
	    title = g_strdup("Alerts By tcontext_u");
	else if (g_strrstr(data->ploticus_script, "tcontext_t")!=NULL)
	    title = g_strdup("Alerts By tcontext_t");
	else if (g_strrstr(data->ploticus_script, "scontext_r")!=NULL)
	    title = g_strdup("Alerts By scontext_r");
	else if (g_strrstr(data->ploticus_script, "scontext_u")!=NULL)
	    title = g_strdup("Alerts By scontext_u");
	else if (g_strrstr(data->ploticus_script, "scontext_t")!=NULL)
	    title = g_strdup("Alerts By scontext_t");
	else
	    title = g_strdup(" ");
	return title;
}

void data_set_graph_type (char * graph_type) 
{
	if (graph_type != NULL) 
		g_free (graph_type);
}

int selinux_generate_machines_script(frame_t * f)
{
	char 		*path 			= frame_get_absolute_tmp_path (f, "agg_alerts_machines");
	char 		*reportname		= NULL;
	char 		*absolutereport 	= NULL;
	int 		i 			= 0;
	int 		datafield 		= 0;
	char 		*label 			= NULL;
	FILE 		*file 			= fopen(path, "w+"); 
	
	g_free(path);
	if (file != NULL) {
		if (f->display_period.type == LIVE || f->display_period.type == DAY) {
			datafield = 3;
			label = g_strdup_printf("Hour");
		}
		else if (f->display_period.type == WEEK || f->display_period.type == MONTH) {
			datafield = 2;
			label = g_strdup_printf("Day");
		}
		fprintf(file, "#proc getdata\n"
					"#intrailer\n\n"
					"#proc tabulate\n"
					" axis1: x\n"
					" datafield1: %i\n"
					"#proc page \n"
					" backgroundcolor: white\n"
					" color: black\n\n"
					"#proc areadef\n"
					" xautorange: datafield=1\n"
					" xaxis.stubs: inc\n"
					" xaxis.label: %s\n"
					" yautorange: datafield=2 mininit=1\n"
					" yaxis.stubs: inc\n"
					" yaxis.label: Number of alerts\n\n",
					datafield,
					label);
		g_free(label);
		GList *list = machine_get_list_for_plugin (f->type, FALSE);
		if (list) {
			GList *iter = list;
			do {
				if (g_strcmp0 (machine_get_folder_name (iter->data), "__all__machines__") != 0) {
					fseek(file, 0, SEEK_END);
					reportname = g_strdup_printf ("selinux_%i.txt", f->display_period.type);
					absolutereport = frame_get_absolute_reports_path (f, reportname, machine_get_folder_name (iter->data));
					g_free (reportname);				
	
					fprintf (file, "#proc getdata\n"
								" file: %s\n\n"
								"#proc tabulate\n"
								" axis1: x\n"
								" datafield1: %i\n\n"
								"#proc lineplot\n"
								" yfield: 2\n"
								" xfield: 1\n"
								" linedetails: width=2 color=%s\n"
								" legendlabel: %s\n\n",
								absolutereport,
								datafield,
								colors[i],
								machine_get_folder_name (iter->data));
					i = (i+1)%10;
					g_free (absolutereport);
				}
			} while ((iter = g_list_next (iter)) != NULL);
		}
		g_list_free (list);
		fprintf(file, "#proc legend\n"  
						" location: max-0.6 max\n\n"
						"#proc trailer\n");
		reportname = g_strdup_printf ("selinux_%i.txt", f->display_period.type);
		absolutereport = frame_get_absolute_reports_path (f, reportname, ALL_MACHINES_FOLDER_NAME);
		fprintf(file,"file: %s\n\n", absolutereport);
		g_free (reportname);
		g_free (absolutereport);
		fclose(file);
	}
	else {
		fprintf(stderr, "Error opening the ploticus script file \n");
	}
	
	return 0;
}

void selinux_draw_alert_level_bar (frame_t *f, cairo_t *cr, double alert_level) 
{
	cairo_surface_t			*image			= NULL;
	char				*path_warning		= NULL;
	double				tmp;
	int				w,h;
	cairo_pattern_t 		*gradient		= NULL;

	if (alert_level > 1.0) {
		alert_level = 1.0;
	}
		
	/* Setting the gradient overlay for the alert level bar*/
	gradient = cairo_pattern_create_linear(0,frame_get_height (f),frame_get_width (f),frame_get_height (f) );
	cairo_pattern_add_color_stop_rgb ( gradient, 0.0, 0.0, 1.0, 0.0); // green
	cairo_pattern_add_color_stop_rgb ( gradient, 0.5, 1.0, 1.0, 0.0); // yellow 
	cairo_pattern_add_color_stop_rgb ( gradient, 1.0, 1.0, 0.0, 0.0); // red
	cairo_rectangle (cr, 6, frame_get_height (f)*0.95 , alert_level*frame_get_width (f) - 12, frame_get_height(f)*0.05);
	cairo_set_source(cr,gradient);
	cairo_fill (cr);
	cairo_set_source_rgb (cr, 0, 0, 0); 
	cairo_stroke (cr);
	
	// When the alert level is very high, display a warning message
	if (alert_level >= 0.9) {
	
		path_warning = frame_get_absolute_data_path (f, "devil-network-offline.png");
		image = cairo_image_surface_create_from_png (path_warning);
		w = cairo_image_surface_get_width (image);
		h = cairo_image_surface_get_height (image);
		tmp = frame_get_height (f) / h;
	
		cairo_save (cr);
		cairo_scale (cr, tmp, tmp);
		cairo_set_source_surface (cr, image, 0, 0);
		cairo_paint_with_alpha(cr, 1);
		
		cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);	
		cairo_set_font_size (cr, frame_get_height (f)*0.096);
		cairo_set_source_rgb (cr,1, 0, 0);
		cairo_move_to(cr, frame_get_width (f)*0.7, frame_get_height (f)*0.45);
		cairo_show_text (cr, "1220397810937 ");

	
		
		cairo_surface_destroy (image);
		cairo_restore (cr);	
	}
	g_free(path_warning);
	cairo_pattern_destroy (gradient);
}

void selinux_draw_scroll (frame_t *f, cairo_t *cr) 
{
	char				*path_scroll		= NULL;
	cairo_surface_t			*image			= NULL;
	double				tmp = 0;	
	int				w,h;

	path_scroll = frame_get_absolute_data_path (f, "scroll.png");
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

int selinux_display_latest_text_alerts(frame_t *f, cairo_t *cr)
{
	data_t 		*data 		= (data_t *) f->priv;
	double 		alert_level 	= 0;
	char 		*file_name	= g_strdup_printf ("selinux_%i.txt", f->display_period.type);
	char 		*path 		= frame_get_absolute_reports_path (f, file_name, NULL);
	FILE 		* file 		= NULL;
	char 		cursor;
	char text[256], text2[256], type[256], date[256], action[256], acct[256], ip[256], hour[256], comm[256], cwd[256], exe[256], gid[256], res[256], result[256], scontext[256], subj[256], suid[256], syscall[256], tclass[256], tcontext[256], uid[256], user[256];
	int 		i		= 0;
	int 		j		= 0;

	cairo_set_source_rgb (cr, 255, 255, 255); 
	cairo_paint_with_alpha (cr, 1);
	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);	
	cairo_set_font_size (cr, frame_get_height (f)*0.046);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_move_to(cr, 2, frame_get_height (f)*0.042);
	cairo_show_text (cr, "Date           Hour      Type        User ");
	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

	file = fopen(path, "r");
	if (file != NULL) {
		/*Set the cursor NUMBER_OF_LINES + data->line_number_offset lines before the end*/
		fseek(file, 0, SEEK_END);
		cursor = fgetc(file);
		if ((NUMBER_OF_LINES + data->line_number_offset) <  selinux_count_alerts(path)) {
			while (j < (NUMBER_OF_LINES + data->line_number_offset)) {
				do {
					fseek(file, -2, SEEK_CUR); 
					cursor = fgetc(file); 
				} while (cursor !='\n');
				j++;
			}
		}		
		else {
			rewind(file);
			j = selinux_count_alerts(path);
		}
		while (j>0 && i< NUMBER_OF_LINES) { /*Read the specified number of lines*/
			fgets(text, 256 ,file); 
			sscanf(text, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",ip , date, hour, type, action, acct, comm, cwd, exe, gid, res, result, scontext, subj, suid, syscall, tclass, tcontext, uid, user);
			sprintf(text2, "%s %s %s %s", date, hour, type, comm);
			/*Increase the alert level when particular alert types are found in the logs.*/
			if (g_strcmp0(type,"AVC")==0 || g_strcmp0(type,"USER_ROLE_CHANGE")==0 || g_strcmp0(type, "SYSCALL")==0 || g_strcmp0(type, "ADD_USER")==0 || g_strcmp0(type,"ADD_GROUP")==0 || g_strcmp0(type,"CONFIG_CHANGE")==0 ) {
				alert_level += 0.06;
				cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
			}
			else {
				alert_level += 0.02;
				cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
			} 
			cairo_move_to(cr, 2, (0.052*i + 0.1) * frame_get_height (f));
			cairo_show_text (cr, text2);			
			i++;
			j--;
		}
		fclose(file); 
	}
	else {
		printf("Error opening the log file %s\n", path);
	}

	g_free(path);
	g_free(file_name);
	selinux_draw_alert_level_bar(f, cr, alert_level);
	selinux_draw_scroll(f, cr);

	return 0;
}

int selinux_display_text_statistics(frame_t *f, cairo_t *cr)
{
	char 		*path 			= NULL;
	char 		*file_name 		= NULL;
	char 		*count_message 		= NULL;
	char 		*period			= NULL;
	char 		*intro_message 		= NULL; 
	int 		i	 		= 0;

	cairo_set_source_rgb (cr, 255, 255, 255);
	cairo_paint_with_alpha (cr, 1);

	selinux_draw_scroll(f, cr);
	GList *list = machine_get_list_for_plugin (f->type, FALSE);
	if (list) {
		GList *iter = list;
		do {
			file_name = g_strdup_printf ("selinux_%i.txt", f->display_period.type);
			path = frame_get_absolute_reports_path (f, file_name, machine_get_folder_name (iter->data));
			count_message = g_strdup_printf (" %s had %i alerts.", 
										machine_get_folder_name (iter->data),
										selinux_count_alerts(path));
			if(selinux_count_alerts(path) == 0) 
				cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
			else 
				cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
			cairo_set_font_size (cr, frame_get_height (f)*0.05);
			cairo_move_to(cr, 0, (0.06*i + 0.19) * frame_get_height (f));
			cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);	
			cairo_show_text (cr, count_message);	
			g_free (count_message);
			g_free (path);
			g_free (file_name);
			i++;
		} while ((iter = g_list_next (iter)) != NULL);
		g_list_free (list);
	}
			switch (f->display_period.type) {
			case LIVE : 
				period = g_strdup_printf(" During the past 6 hours");
				break;
			case DAY : 
				period = g_strdup_printf(" During the past 24 hours");
				break;
			case WEEK : 
				period = g_strdup_printf(" During the past week");
				break;
			case MONTH : 
				period = g_strdup_printf(" During the past month");
				break;
			default : 
				period = g_strdup_printf(" During the past 24 hours");
				break;
			}

	cairo_set_font_size (cr, frame_get_height (f)*0.06);
	cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);	
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_move_to(cr, 0, frame_get_height (f)*0.06);
	intro_message = g_strdup_printf (" There are %i machines using SELinux", i);
	cairo_show_text (cr, intro_message);
	cairo_set_font_size (cr, frame_get_height (f)*0.05);
	cairo_move_to(cr, 0, frame_get_height (f)*0.13);
	cairo_show_text (cr, period);
	g_free (intro_message);
	g_free (period);

	return 0;
}


char * selinux_ploticus_script_name (frame_t *f) 
{
	data_t 		*data 		= (data_t *) f->priv;
	char 		*name		= NULL;
	
	if (g_strrstr(data->ploticus_script, "machines")!=NULL && machine_is_all_machines (f->display_machine))
	    name = g_strdup_printf ("agg_alerts_machines");
	else if (g_strrstr(data->ploticus_script, "machines")!=NULL && !(machine_is_all_machines (f->display_machine)))
	    name = g_strdup_printf ("agg_machines_%i", f->display_period.type);
	else if (g_strrstr(data->ploticus_script, "type")!=NULL)
	    name = g_strdup_printf ("%s_type_%i", data->graph_type, f->display_period.type);
	else if (g_strrstr(data->ploticus_script, "user")!=NULL)
	    name = g_strdup_printf ("%s_user_%i", data->graph_type, f->display_period.type);
	else if (g_strrstr(data->ploticus_script, "tclass")!=NULL)
	    name = g_strdup_printf ("%s_tclass_%i", data->graph_type, f->display_period.type);
	else if (g_strrstr(data->ploticus_script, "result")!=NULL)
	    name = g_strdup_printf ("%s_result_%i", data->graph_type, f->display_period.type);
	else if (g_strrstr(data->ploticus_script, "tcontext_r")!=NULL)
	    name = g_strdup_printf ("%s_tcontext_r_%i", data->graph_type, f->display_period.type);
	else if (g_strrstr(data->ploticus_script, "tcontext_u")!=NULL)
	    name = g_strdup_printf ("%s_tcontext_u_%i", data->graph_type, f->display_period.type);
	else if (g_strrstr(data->ploticus_script, "tcontext_t")!=NULL)
	    name = g_strdup_printf ("%s_tcontext_t_%i", data->graph_type, f->display_period.type);
	else if (g_strrstr(data->ploticus_script, "scontext_r")!=NULL)
	    name = g_strdup_printf ("%s_scontext_r_%i", data->graph_type, f->display_period.type);
	else if (g_strrstr(data->ploticus_script, "scontext_u")!=NULL)
	    name = g_strdup_printf ("%s_scontext_u_%i", data->graph_type, f->display_period.type);
	else if (g_strrstr(data->ploticus_script, "scontext_t")!=NULL)
	    name = g_strdup_printf ("%s_scontext_t_%i", data->graph_type, f->display_period.type);
	else 
		name = g_strdup_printf ("text");

	return name;    
}


int selinux_count_alerts (char * path)
{
	FILE 		*file 		= NULL;
	int 		num_alerts 	= 0;
	char 		*line 		= NULL;
        size_t 		len 		= 0;

	file = fopen(path, "r");
	if (file != NULL) { 
		while (getline(&line, &len, file) != -1) 
        	       	num_alerts++;
		if (line) 
			free(line);
		fclose(file);
	}
	else {
		fprintf (stderr, "Error opening the log file %s\n", path);
	}

	return num_alerts;
}
