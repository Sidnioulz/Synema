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
 * \file frame-open-panel.h
 * \brief Header for functions related to the frame opening panel.
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This header file contains the definition of functions related to the frame
 * opening panel.
 */
#ifndef __FRAME_OPEN_PANEL_H
#define __FRAME_OPEN_PANEL_H
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	FILE_TAB,
	FILE_FRAME,
	LAST_FILETYPE_T	// DO NOT USE - FOR LOOP PURPOSE ONLY
} filetype_t;


void frame_open_setup_widgets ();
#ifdef __cplusplus
}
#endif
#endif
