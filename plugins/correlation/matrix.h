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
 * \file matrix.h
 * \brief Correlation plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 *
 * This header file contains functions for the SELinux plugin.
 */
#ifndef __LSMATRIX_H
#define __LSMATRIX_H
#ifdef __cplusplus
extern "C" {
#endif

#include <pcre.h>

#include "frame-common.h"
#include "time-period.h"
#include "criticality.h"


#define NUMBER_OF_CRITERIONS 4
#define CRITICALITY_CRITERION 1
#define SEQUENCES_NUMBER_CRITERION 2
#define SEQUENCES_CRITICALITY_CRITERION 3
#define ACTIVITIES_CRITERION 4


/** \fn int matrix_all_handler (frame_t * f, cairo_t * cr, int * number_of_fields, GList * list);
 * \brief Draw a matrix with all criterions
 *
 * This function draws a matrix with every existing criterion
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] number_of_fields the matrix size
 * @param[in] list list of machines names
 */
int matrix_all_handler (frame_t * f, cairo_t * cr, int * number_of_fields, GList * list);


/** \fn int matrix_one_criterion_handler (frame_t * f, cairo_t * cr, int * number_of_fields, GList * list, GList * first_part_ip_list);
 * \brief Draw a one criterion matrix
 *
 * This function draws a matrix for one criterion
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] number_of_fields the matrix size
 * @param[in] list list of machines names
 * @param[in] first_part_ip_list list with all beginnings of ip found in list
 */
int matrix_one_criterion_handler (frame_t * f, cairo_t * cr, int * number_of_fields, GList * list, GList * first_part_ip_list);


/** \fn int draw_matrix_item (frame_t *f, cairo_t * cr, int * number_of_fields, int x_pos, int y_pos, int criterion_value, int criterion_index);
 * \brief Draw one matrix item
 *
 * This function draws a matrix item
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] number_of_fields the matrix size
 * @param[in] x_pos the x position of the item
 * @param[in] y_pos the y position of the item
 * @param[in] criterion_value the criterion value for this item
 * @param[in] criterion_index the criterion number
 */
int draw_matrix_item (frame_t *f, cairo_t * cr, int * number_of_fields, int x_pos, int y_pos, int criterion_value, int criterion_index);


/** \fn int select_criterion_color (cairo_t * cr, int criterion_nb, int criterion_value);
 * \brief Select the color to draw a matrix item
 *
 * This function select a color according to the criterion and its value
 * @param[in] cr cairo surface
 * @param[in] criterion_nb number of the criterion
 * @param[in] criterion_value the value it has for a particular machine
 */
int select_criterion_color (cairo_t * cr, int criterion_nb, int criterion_value);


/** \fn int draw_matrix (frame_t * f, cairo_t * cr);
 * \brief Draw a criterion matrix 
 *
 * This function draws a criterion matrix when a matrix mode is selected
 * in the menu.
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 */
int draw_matrix (frame_t * f, cairo_t * cr);


/** \fn int draw_matrix_axis (frame_t * f, cairo_t * cr, GList * list, GList * first_part_ip_list, int * number_of_fields);
 * \brief Draws matrix axis
 *
 * This function draws x and y axis for a criterion matrix
 * @param[in] f the frame which needs to refresh its content
 * @param[in] cr cairo surface
 * @param[in] list list of machines names
 * @param[in] first_part_ip_list list with all beginnings of ip found in list
 * @param[in] number_of_fields an array with the matrix size
 */
int draw_matrix_axis (frame_t * f, cairo_t * cr, GList * list, GList * first_part_ip_list, int * number_of_fields);


/** \fn char * get_criterion (int i);
 * \brief Get a criterion name
 *
 * This function returns the name of a criterion
 * @param[in] i number of the criterion 
 */
char * get_criterion (int i);


/** \fn int find_*_value (frame_t * f, char * machine_name,  GList * list);
 * \brief Get a criterion value
 *
 * This function find the value of a particular criterion.
 * @param[in] f the frame which needs to refresh its content
 * @param[in] machine_name the name of the machine for which we want to know the criterion value
 * @param[in] list list of available machines
 */
int find_activities_number_value (frame_t * f, char * machine_name,  GList * list);
int find_sequences_number_value (frame_t * f, char * machine_name,  GList * list);
int find_sequences_criticality_value (frame_t * f, char * machine_name,  GList * list);
int find_raw_criticality_value (frame_t * f, char * machine_name,  GList * list);

/** \fn int find_criterion_value (frame_t * f, char * machine_name, GList * list, int criterion_number);
 * \brief Get a criterion value
 *
 * This function find the value of the criterion given sa a parameter.
 * @param[in] f the frame which needs to refresh its content
 * @param[in] machine_name the name of the machine for which we want to know the criterion value
 * @param[in] list list of available machines
 * @param[in] criterion_number number of the criterion we want to know the value for.
 */
int find_criterion_value (frame_t * f, char * machine_name, GList * list, int criterion_number);


#endif
#ifdef __cplusplus
}
#endif
