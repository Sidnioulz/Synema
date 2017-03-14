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
 * \file plugin.h
 * \brief Correlation plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 *
 * This header file contains functions for the SELinux plugin.
 */
#ifndef __LSPLUGIN_H
#define __LSPLUGIN_H
#ifdef __cplusplus
extern "C" {
#endif

#include "frame-common.h"
#include "time-period.h"

typedef struct __data_t {
	char *ploticus_script;
	char *graph_type;
	int line_number_offset;
	int osiris_file_nb;

int refreshing_is_allowed;

	int display_axis_legend;

	int time_period_offset;

	GList ** activity_list;
	GList ** sequences_list;
	
	GSList * Sensors_List;
	int Number_Of_Sensors;
	int Number_Of_Activities;
	int Number_Of_Inter_Activities_Rules;
	int Number_Of_Sequences;
	long * file_end_position;

	char ** colors_array;
	char ** activity_name_array;
	
	int * activity_number;
	int * sequence_number;
	int * sequence_criticality;
} data_t;


char **get_machines_for_tool (char *prefix, char *tool);

int refresh_content (frame_t *);
int init_private (frame_t *);
int free_private (frame_t *);
int display_machine_changed (frame_t *);
int time_period_changed (frame_t *);
int frame_size_changed (frame_t *);
int is_refreshing_needed (frame_t *);
int draw_area (frame_t *, cairo_t *);
char *get_display_name (frame_t *);
int restore_private(frame_t *, GKeyFile *);
int save_private(frame_t *, GKeyFile *);




/** \fn int button_press_handler (frame_t *f, GdkEventButton *event, cairo_t *cr)
 * \brief Changes the graphic type when mouse button is released inside the frame.
 *
 * When text logs are enabled, this function changes the value of data->line_number_offset
 * to reflect the amount of lines to be scrolled. 
 * When criticalities, activities or sequences graphs are enabled, it modifies 
 * data->time_period_offset to move the displayed time.
 * @param[in] event the button event struct
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
int button_press_handler (frame_t *f, GdkEventButton *event, cairo_t *cr);



/** \fn int button_release_handler (frame_t *f, GdkEventButton *event, cairo_t *cr)
 * \brief Changes the graphic type when mouse button is released inside the frame.
 *
 * When in sequences or matrix mode, this function show/hide the yaxis legend
 * @param[in] event the button event struct 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
int button_release_handler (frame_t *f, GdkEventButton *event, cairo_t *cr);


#endif
#ifdef __cplusplus
}
#endif
