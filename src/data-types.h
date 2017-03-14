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
 * \file data-types.h
 * \brief Header for various data types that are too little for having a single file
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This header file contains several data structures that are too little to be
 * in a standalone file. It includes:
 * size_ratio_t		a list of size ratios for a frame
 * refresh_freq_t	a refresh frequency in milliseconds
 */
#ifndef __DATA_TYPES_H
#define __DATA_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

#include "constants.h"



/** \enum size_ratio_t
 * \brief Size ratio of the frame
 *
 * This enum represents the size of the drawing area inside a frame. It can only
 * take a limited set of values.
 */
typedef enum {
	NORMAL=0,			/**< Default size of a frame. Around 300px height */
	LARGE=1,			/**< Twice as big as the default size */
	FULL=2,				/**< Future size ratio that will fit the whole screen area */
	LAST_SIZE_RATIO_T	// DO NOT USE THIS - FOR LOOP PURPOSE ONLY
} size_ratio_t;



/** \typedef refresh_freq_t
 * \brief Refresh frequency of a frame
 *
 * This value represents the refresh frequency of a frame, in milliseconds. A
 * special value, REFRESH_FREQ_AUTO, can also be used.
 */
typedef int refresh_freq_t;
#ifdef __cplusplus
}
#endif
#endif
