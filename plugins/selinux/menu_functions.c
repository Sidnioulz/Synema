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
#include "menu_functions.h"
#include "selinux.h"


GList *build_func_list ()
{

	GList *list = NULL;
	GList *children_list = NULL;

	func_list_add_new_func (&list, "Text logs", selinux_show_text);
	func_list_add_new_separator (&list);

	func_list_add_new_func (&list, "Global Stats", selinux_machines);	
	func_list_add_new_separator (&list);

	func_list_add_new_func (&list, "Pie / Bar graph", selinux_type_change);
	func_list_add_new_separator (&list);


	func_list_add_new_func (&children_list, "By user", selinux_user);
	func_list_add_new_func (&children_list, "By type", selinux_type);
	func_list_add_new_func (&children_list, "By tclass", selinux_tclass);
	func_list_add_new_func (&children_list, "By result", selinux_result);
	func_list_add_new_submenu (&list, "Other alerts", children_list);

	children_list = NULL;
	func_list_add_new_func (&children_list, "tcontext_t", selinux_tcontext_t);
	func_list_add_new_func (&children_list, "tcontext_r", selinux_tcontext_r);
	func_list_add_new_func (&children_list, "tcontext_u", selinux_tcontext_u);
	func_list_add_new_submenu (&list, "Alerts by tcontext", children_list);

	children_list = NULL;
	func_list_add_new_func (&children_list, "scontext_t", selinux_scontext_t);
	func_list_add_new_func (&children_list, "scontext_r", selinux_scontext_r);
	func_list_add_new_func (&children_list, "scontext_u", selinux_scontext_u);
	func_list_add_new_submenu (&list, "Alerts by scontext", children_list);

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



void selinux_type_change(GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;
	
	if(g_strcmp0(data->graph_type, "pie") == 0) {	
		data_set_graph_type(data->graph_type);
		data->graph_type = strdup ("bar");
	}
	else {	
		data_set_graph_type(data->graph_type);
		data->graph_type = strdup ("pie");
	}
	
	if ( g_strcmp0(data->ploticus_script,"text") != 0) {
		name = selinux_ploticus_script_name (f);
		data_set_graph_type(data->ploticus_script);
		data->ploticus_script = strdup (name);
	}
	g_free(name);
	frame_request_content_refresh (f);
	frame_request_redraw (f);
}	

void selinux_machines (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;

	data_set_graph_type(data->ploticus_script);
	if (machine_is_all_machines (f->display_machine)) {
		data->ploticus_script = strdup ("agg_alerts_machines");
	}
	else {
		name = g_strdup_printf ("agg_machines_%i", f->display_period.type);
		data->ploticus_script = strdup (name);	
	}
	g_free(name);
	frame_request_content_refresh (f);
}

void selinux_type (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;

	data_set_graph_type(data->ploticus_script);

	name = g_strdup_printf ("%s_type_%i", data->graph_type, f->display_period.type);
	data->ploticus_script = strdup (name);
	g_free(name);
	frame_request_content_refresh (f);
}

void selinux_show_text (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;

	data_set_graph_type(data->ploticus_script);

	data->ploticus_script = strdup ("text");
	frame_request_redraw (f);
}

void selinux_scontext_u (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;

	data_set_graph_type(data->ploticus_script);

	name = g_strdup_printf ("%s_scontext_u_%i", data->graph_type, f->display_period.type);
	data->ploticus_script = strdup (name);
	g_free(name);
	frame_request_content_refresh (f);
}

void selinux_scontext_t (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;

	data_set_graph_type(data->ploticus_script);

	name = g_strdup_printf ("%s_scontext_t_%i", data->graph_type, f->display_period.type);
	data->ploticus_script = strdup (name);
	g_free(name);
	frame_request_content_refresh (f);
}

void selinux_scontext_r (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;
	data_set_graph_type(data->ploticus_script);

	name = g_strdup_printf ("%s_scontext_r_%i", data->graph_type, f->display_period.type);
	data->ploticus_script = strdup (name);
	g_free(name);
	frame_request_content_refresh (f);
}


void selinux_tcontext_u (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;
	data_set_graph_type(data->ploticus_script);

	name = g_strdup_printf ("%s_tcontext_u_%i",data->graph_type,  f->display_period.type);
	data->ploticus_script = strdup (name);
	g_free(name);
	frame_request_content_refresh (f);
}


void selinux_tcontext_t (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;
	data_set_graph_type(data->ploticus_script);

	name = g_strdup_printf ("%s_tcontext_t_%i",data->graph_type,  f->display_period.type);
	data->ploticus_script = strdup (name);
	g_free(name);	
	frame_request_content_refresh (f);
}


void selinux_tcontext_r (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;
	data_set_graph_type(data->ploticus_script);

	name = g_strdup_printf ("%s_tcontext_r_%i", data->graph_type, f->display_period.type);
	data->ploticus_script = strdup (name);
	g_free(name);	
	frame_request_content_refresh (f);
}


void selinux_result (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;
	data_set_graph_type(data->ploticus_script);

	name = g_strdup_printf ("%s_result_%i", data->graph_type, f->display_period.type);
	data->ploticus_script = strdup (name);
	g_free(name);	
	frame_request_content_refresh (f);
}


void selinux_tclass (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;
	data_set_graph_type(data->ploticus_script);

	name = g_strdup_printf ("%s_tclass_%i",data->graph_type,  f->display_period.type);
	data->ploticus_script = strdup (name);
	g_free(name);
	frame_request_content_refresh (f);
}


void selinux_user (GtkMenuItem *item, gpointer user_data)
{
	frame_t 	*f 		= (frame_t *) user_data;
	data_t 		*data		= (data_t *) f->priv;
	char 		*name 		= NULL;
	data_set_graph_type(data->ploticus_script);

	name = g_strdup_printf ("%s_user_%i", data->graph_type, f->display_period.type);
	data->ploticus_script = strdup (name);
	g_free(name);
	frame_request_content_refresh (f);
}
