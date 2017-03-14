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
 * \brief Header of the demonstration plugin written by the twentieth group
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains nothing interesting.
 */
#ifndef __LSPLUGIN_H
#define __LSPLUGIN_H
#ifdef __cplusplus
extern "C" {
#endif

#include "frame-common.h"
#include "time-period.h"



typedef struct __data_t {
	int entier;
	char *chaine;
} data_t;



GList *build_func_list ();
GList *build_time_periods ();
int refresh_content (frame_t *);
int init_private (frame_t *);
int restore_private (frame_t *, GKeyFile *);
int save_private (frame_t *, GKeyFile *);
int free_private (frame_t *);
int display_machine_changed (frame_t *);
int time_period_changed (frame_t *);
int frame_size_changed (frame_t *);
int is_refreshing_needed (frame_t *);
int draw_area (frame_t *, cairo_t *);
char *get_display_name (frame_t *);
int button_press_handler (frame_t *, GdkEventButton *);
int button_release_handler (frame_t *, GdkEventButton *);
#endif
#ifdef __cplusplus
}
#endif
