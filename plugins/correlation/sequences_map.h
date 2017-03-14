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
 * \file sequences_map.h
 * \brief Correlation plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 *
 * This header file contains functions for the SELinux plugin.
 */
#ifndef __LSSEQUENCESMAP2_H
#define __LSSEQUENCESMAP2_H
#ifdef __cplusplus
extern "C" {
#endif

#include <pcre.h>

#include "frame-common.h"
#include "time-period.h"
#include "criticality.h"



/** \fn int display_sequences_map (frame_t *f, cairo_t *cr)
 * \brief Draw the sequences map
 *
 * This function draws axis and legend for the sequences map, 
 * then calls the function displaying sequences. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
int display_sequences_map(frame_t * f, cairo_t * cr);


/** \fn draw_subsequence (frame_t * f, cairo_t * cr,  time_t current_time, int sequence_number, Sequence_t * sequence_info, float * last_position);
 * \brief Draw a part of a sequence
 *
 * This function draws a sub-sequence : activities and links 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] current_time the date to put at the end of xaxis
 * @param[in] sequence_number the number of the sequence being drawn
 * @param[in] sequence_info the structure with every info abour the subsequence
 * @param[in] last_position the position of the last activity who was drawn (to draw links)
 */
float * draw_subsequence (frame_t * f, cairo_t * cr,  time_t current_time, int sequence_number, Sequence_t * sequence_info, float * last_position);


/** \fn draw_sequence (frame_t * f, cairo_t * cr, time_t current_time, int sequence_number, GList * sequence_occurence, float * last_position);
 * \brief Draw a sequence
 *
 * This function draws a sequence by calling draw_subsequence() for 
 * each subsequence
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] current_time the date to put at the end of xaxis
 * @param[in] sequence_number the number of the sequence being drawn
 * @param[in] sequence_info the Sequence_t list with informations about each sub-sequence of the sequence
 * @param[in] last_position the position of the last activity who was drawn (to draw links)
 */
float * draw_sequence (frame_t * f, cairo_t * cr, time_t current_time, int sequence_number, GList * sequence_occurence, float * last_position);


/** \fn int draw_sequences_confidence_plot (frame_t * f, cairo_t * cr, time_t current_time);
 * \brief Draw the sequences confidence plot
 *
 * This function draws the sequences confidence plot
 * from the values in the activities list
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] current_time the date to put at the end of xaxis
 */
int draw_sequences_confidence_plot (frame_t * f, cairo_t * cr, time_t current_time);


/** \fn void draw_sequence_axis(frame_t *f, cairo_t *cr, time_t current_time, int period_type);
 * \brief Draw the alerts map
 *
 * This function draws axis and legend for the alerts map, 
 * then calls the function displaying alerts. 
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] current_time the date to put at the end of xaxis
 * @param[in] period_type the actual period type
 */
void draw_sequence_axis(frame_t *f, cairo_t *cr, time_t current_time, int period_type);

#endif
#ifdef __cplusplus
}
#endif
