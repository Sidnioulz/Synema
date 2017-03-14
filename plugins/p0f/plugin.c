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
 * \brief Fake P0F plugin written by the twentieth group
 * \author Steve Dodier <sidnioulz@gmail.com>
 * \author Timothée Ravier <timothee.romain.ravier@gmail.com>
 *
 * This file contains a fake P0F plugin for demonstration purpose.
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
	return "P0F";
}



void set_graph_OSgeneralPIE (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = OSgeneralPIE;
	frame_request_content_refresh (f);
}

void set_graph_OSgeneralBAR (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = OSgeneralBAR;
	frame_request_content_refresh (f);
}

void set_graph_OSlinuxBAR (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = OSlinuxBAR;
	frame_request_content_refresh (f);
}

void set_graph_OSlinuxPIE (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = OSlinuxPIE;
	frame_request_content_refresh (f);
}

void set_graph_OSmacBAR (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = OSmacBAR;
	frame_request_content_refresh (f);
}

void set_graph_OSmacPIE (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = OSmacPIE;
	frame_request_content_refresh (f);
}

void set_graph_fwBAR (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = fwBAR;
	frame_request_content_refresh (f);
}

void set_graph_fwPIE (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = fwPIE;
	frame_request_content_refresh (f);
}

void set_graph_OSwindowsPIE (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = OSwindowsPIE;
	frame_request_content_refresh (f);
}
	
void set_graph_OSwindowsBAR (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = OSwindowsBAR;
	frame_request_content_refresh (f);
}

void set_graph_distanceBAR (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;
	
	data->graph_type = distanceBAR;
	frame_request_content_refresh (f);
}

void set_graph_distancePIE (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = distancePIE;
	frame_request_content_refresh (f);
}

void set_graph_natBAR (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = natBAR;
	frame_request_content_refresh (f);
}

void set_graph_natPIE (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = natPIE;
	frame_request_content_refresh (f);
}

void set_graph_linkBAR (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = linkBAR;
	frame_request_content_refresh (f);
}

void set_graph_linkPIE (GtkMenuItem *item, gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_t *data = (data_t *) f->priv;

	data->graph_type = linkPIE;
	frame_request_content_refresh (f);
}


GList *build_func_list ()
{
	GList *list = NULL;
	GList *children_list = NULL;
	GList *children2_list = NULL;
	
	
	func_list_add_new_func (&children_list, "Kernel general pie", set_graph_OSgeneralPIE);
	//func_list_add_new_func (&children_list, "Kernel linux pie", set_graph_OSlinuxPIE);
	//func_list_add_new_func (&children_list, "Kernel winodws pie", set_graph_OSwindowsPIE);
	func_list_add_new_func (&children_list, "Firewall pie", set_graph_fwPIE);
	func_list_add_new_func (&children_list, "Distance pie", set_graph_distancePIE);
	//func_list_add_new_func (&children_list, "Kernel mac pie", set_graph_OSmacPIE);
	func_list_add_new_func (&children_list, "NAT pie", set_graph_natPIE);
	func_list_add_new_func (&children_list, "Link pie", set_graph_linkPIE);
	func_list_add_new_submenu (&list, "Pie Graphs", children_list);
	func_list_add_new_separator (&list);

	func_list_add_new_func (&children2_list, "Kernel general bar", set_graph_OSgeneralBAR);
	func_list_add_new_func (&children2_list, "Kernel linux bar", set_graph_OSlinuxBAR);
	func_list_add_new_func (&children2_list, "Kernel windows bar", set_graph_OSwindowsBAR);
	func_list_add_new_func (&children2_list, "Kernel mac bar", set_graph_OSmacBAR);
	func_list_add_new_func (&children2_list, "Firewall bar", set_graph_fwBAR);
	func_list_add_new_func (&children2_list, "Distance bar", set_graph_distanceBAR);
	func_list_add_new_func (&children2_list, "NAT bar", set_graph_natBAR);
	func_list_add_new_func (&children2_list, "Link bar", set_graph_linkBAR);
	func_list_add_new_submenu (&list, "Bar Graphs", children2_list);

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
	
	p3 = time_period_new (MONTH, 2);
	list = g_list_prepend (list, p3);
	
	p4 = time_period_new (LIVE, 0);
	list = g_list_prepend (list, p4);
	
	return list;
}



int is_refreshing_needed (frame_t *f)
{
	
	return 1;
}



int refresh_content (frame_t *f)
{
	data_t *data = (data_t *) f->priv;
		
	int b;
	//if type = bar, refresh_bar
	
	if (data->graph_type == OSgeneralPIE) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursOSpie.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			//printf("%s\n",report);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
			fprintf (stderr, "Error while ploticus was generating the picture");
			return -1;
		

			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysOSpie.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksOSpie.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			
			}

		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsOSpie.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			
			}
		}
	}
	
	
	if (data->graph_type == OSgeneralBAR) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursOSbar.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysOSbar.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksOSbar.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsOSbar.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}
	
	
	if (data->graph_type == OSlinuxPIE) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursOSlinuxPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysOSlinuxPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksOSlinuxPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsOSlinuxPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}
	
	
	if (data->graph_type == OSlinuxBAR) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursOSlinuxBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysOSlinuxBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksOSlinuxBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsOSlinuxBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}
	

	if (data->graph_type == OSwindowsPIE) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursOSwindowsPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysOSwindowsPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksOSwindowsPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsOSwindowsPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}
	
	
	if (data->graph_type == OSwindowsBAR) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursOSwindowsBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysOSwindowsBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksOSwindowsBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsOSwindowsBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}
	
	
	if (data->graph_type == OSmacPIE) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursOSmacPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysOSmacPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksOSmacPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsOSmacPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}
	
	
	if (data->graph_type == OSmacBAR) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursOSmacBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysOSmacBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksOSmacBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsOSmacBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}

	if (data->graph_type == fwBAR) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursfwBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysfwBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksfwBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsfwBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}
	
	if (data->graph_type == fwPIE) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursfwPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysfwPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksfwPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsfwPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}
	
	if (data->graph_type == linkBAR) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hourslinkBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2dayslinkBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weekslinkBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthslinkBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}
	
	if (data->graph_type == linkPIE) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hourslinkPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2dayslinkPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weekslinkPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthslinkPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}
	
	if (data->graph_type == natBAR) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursnatBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysnatBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksnatBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsnatBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}
	
	if (data->graph_type == natPIE) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursnatPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysnatPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksnatPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsnatPIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}
	
	if (data->graph_type == distanceBAR) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursdistanceBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysdistanceBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksdistanceBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsdistanceBAR.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}

	if (data->graph_type == distancePIE) {
	
		if(f->display_period.type == LIVE){
		
			char * script=frame_get_absolute_data_path (f, "2hoursdistancePIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == DAY){
		
			char * script=frame_get_absolute_data_path (f, "2daysdistancePIE.pl");
			printf("SCRIPT = %s\n\n", script);
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			printf("SVG IMAGE FILENAME = %s\n\n", p0f);
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			printf("reportsdir = %s\n\n", report);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			printf("COMMUN = %s\n\n", commun);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == WEEK){
		
			char * script=frame_get_absolute_data_path (f, "2weeksdistancePIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
		
		if(f->display_period.type == MONTH){
		
			char * script=frame_get_absolute_data_path (f, "2monthsdistancePIE.pl");
			char * p0f=frame_get_absolute_tmp_path (f, "image_p0f.svg");
			char * report= frame_get_absolute_reports_path (f, NULL, NULL);
			char * commun=g_strdup_printf("ploticus %s -svg -dir %s -o %s",script,report,p0f);
			system(commun);
			g_free(report);
			if (b == -1) {
				fprintf (stderr, "Error while ploticus was generating the picture");
				return -1;
			}
		}
	}

//	printf ("The ploticus image has been recomputed, now we should redraw the drawing area.\n");
	frame_request_redraw (f);

	return 0;
}



int init_private (frame_t *f)
{
	f->priv = malloc (sizeof (data_t));
	// Recommended to access f->priv
	data_t *data = (data_t *) f->priv;
	
//	printf ("Initialising my plugin's private data here.\n");
	
	data->chaine = strdup ("Hello kitty.\n");
	data->entier = 42;
	
	return 0;
}



int free_private (frame_t *f)
{
	// Recommended to access f->priv
	data_t *data = (data_t *) f->priv;
	
//	printf ("Here I do not forget to free all the memory I allocated.\n");
	
	free (data->chaine);
	free (data);

	return 0;
}



int display_machine_changed (frame_t *f)
{
//	printf ("Here, I must change my internal data to reflect the machine change.\n");
	frame_request_content_refresh (f);
	return 0;
}



int time_period_changed (frame_t *f)
{
//	printf ("Here, I must change my internal data to reflect the time period change.\n");
	frame_request_content_refresh (f);
	return 0;
}



int frame_size_changed (frame_t *f)
{
//	printf ("Here, I must change my internal data to reflect the frame size change.\n");
	frame_request_content_refresh (f);
	return 0;
}



/* À voir
 * CAIRO
 * 	http://library.gnome.org/devel/cairo/stable/cairo-context.html
 * 	http://www.cairographics.org/manual/cairo-transformations.html
 *
 * IMAGES svg AVEC CAIRO
 * 	http://library.gnome.org/devel/cairo/stable/cairo-image-surface.html
 *
 * IMAGES SVG AVEC CAIRO
 * 	http://library.gnome.org/devel/rsvg/stable/rsvg-Core-API.html
 * 	http://library.gnome.org/devel/rsvg/stable/rsvg-Cairo.html
 * 
 */
 
 
 
 
 
 
 
 
 
int draw_area (frame_t *f, cairo_t *cr)
{
	data_t 		*data 			= (data_t *) f->priv;	
	char 				*path		= NULL;
	char 				*title		= NULL;
	GError				*err		= NULL;
	RsvgHandle			*myhandle	= NULL;	
	
		cairo_set_source_rgb(cr, 1, 1, 1);
		cairo_paint(cr);		
		cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);	
		cairo_set_font_size (cr, frame_get_height (f)*0.07);
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		cairo_move_to(cr, frame_get_width (f)*0.30, frame_get_height (f)*0.35);
//		cairo_show_text (cr, "Loading ...");
		
		path = frame_get_absolute_tmp_path (f,"image_p0f.svg");	

		myhandle = rsvg_handle_new_from_file (path, &err);
		g_free (path);
		if (err) { 
			g_clear_error (&err);
		}
		else {
			cairo_save (cr); 
			//DÉSOLÉE !!! :)
			if (data->graph_type == OSgeneralBAR || data->graph_type == OSlinuxBAR ||  data->graph_type ==fwBAR  || data->graph_type ==OSmacBAR  || data->graph_type ==OSwindowsBAR  || data->graph_type == natBAR  || data->graph_type == linkBAR  || data->graph_type == distanceBAR) {
				cairo_scale (cr, frame_get_width (f)*0.6, frame_get_height (f)*0.8);			
				cairo_translate (cr, 0.4, 0.2);
			}
			else {
				cairo_scale (cr, frame_get_width (f)*0.45, frame_get_height (f)*0.5);			
				cairo_translate (cr, 0.5, 0.3);			
			}
			
			cairo_rotate (cr, M_PI);
			rsvg_handle_render_cairo (myhandle, cr);
			cairo_restore (cr);
			g_object_unref (myhandle);
	
			/*Display the graphic title*/		
			show_graphic_title(1, f, cr);
		}
				
	return 0;
}









void show_graphic_title(int entier, frame_t *f, cairo_t *cr) 
{
	data_t 		*data 			= (data_t *) f->priv;	
			cairo_set_font_size (cr, frame_get_height (f)*0.05);
			cairo_move_to (cr, frame_get_width (f)*0.1, frame_get_height (f)*0.05);	
		
		
		 
	if (data->graph_type == OSgeneralPIE || data->graph_type == OSgeneralBAR) {
			cairo_show_text (cr, "General Alerts by OS");
	}
	else if (data->graph_type == OSlinuxBAR || data->graph_type == OSlinuxPIE) {
			cairo_show_text (cr, "Alerts by OS: Linux");	
	}
	else if (data->graph_type == OSmacBAR || data->graph_type == OSmacPIE) {
			cairo_show_text (cr, "Alerts by OS: Mac");	
	}

	else if (data->graph_type == OSwindowsBAR || data->graph_type == OSwindowsPIE) {
			cairo_show_text (cr, "Alerts by OS: Windows");	
	}
	
	else if (data->graph_type == fwBAR || data->graph_type == fwPIE) {
			cairo_show_text (cr, "Alerts by Firewall");	
	}
	else if (data->graph_type == distanceBAR || data->graph_type == distancePIE) {
			cairo_show_text (cr, "Alerts by distance");	
	}
	else if (data->graph_type == natBAR || data->graph_type == natPIE) {
			cairo_show_text (cr, "Alerts by NAT");	
	}
	else if (data->graph_type == linkBAR || data->graph_type == linkPIE) {
			cairo_show_text (cr, "Alerts by Link");	
	}
	else 
			cairo_show_text (cr, " ");	
}

