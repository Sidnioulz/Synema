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
 * \file frame-actions.h
 * \brief Header for functions of the frame actions toolbutton menu
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains functions that are linked to the menu items of the
 * frame actions toolbutton menu.
 */
#ifndef __FRAME_ACTIONS_H
#define __FRAME_ACTIONS_H
#ifdef __cplusplus
extern "C" {
#endif
#include <gtk/gtk.h>



/** \enum unhandled_action_t
 * \brief List of things to do when an error happens on a frame action
 *
 * This enum is used to store constants that represent different actions to do
 * when an action triggers an error on a frame.
 */
typedef enum {
	UA_DO_NOTHING=0,		/**< Do nothing and stay on the previously working state */
	UA_ERROR_MESSAGE=1,		/**< Display an error message in the frame and let the user fix things */
	UA_TRY_NEAREST=2,		/**< When the error is due to an unhandled numeric value, try to use the nearest handled one */
	LAST_UNHANDLED_ACTION_T	// DO NOT USE THIS - FOR LOOP PURPOSE ONLY
} unhandled_action_t;



/** \fn void frame_action_setup_widgets ()
 * \brief Setups the frame actions panel's widgets (connects handlers, etc)
 *
 * This function connects the frame actions panel's gtk widgets to their
 * respective handlers.
 */
void frame_action_setup_widgets ();
#ifdef __cplusplus
}
#endif
#endif
