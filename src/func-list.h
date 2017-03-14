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
 * \file func-list.h
 * \brief Header for frame menu items constructors and utilities
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This header file contains the func_list_t structure that represents a frame
 * menu item, as well as constructors for this struct.
 */
#ifndef __FUNC_LIST_H
#define __FUNC_LIST_H
#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

#include "constants.h"



/** \struct func_list_t
 * \brief A list of (display name, function) couples used to populate a menu
 *
 * This struct is a list of couples (display name, function pointer). It
 * is used for passing frame type specific functions to the main application.
 * It can also be a submenu for more menu entries, or a menu separator.
 *
 * The lists created by each group will be displayed in a menu attached to each
 * frame.
 */
typedef struct __func_list_t {
	char *name;			/**< \brief Display name of the function/menu - set to NULL for SEPEATOR type*/
	enum {				/**< \brief Type of the func_list_t */
		FUNCTION,		/**< \brief Pointer to a function */
		SEPARATOR,		/**< \brief Menu separator */
		SUBMENU			/**< \brief Submenu */
	} type;
	
	union {				/**< \brief Data depending on the type of the func_list_t */
		void (*func)(GtkMenuItem *, gpointer);	/**< \brief Pointer to the function for FUNCTION type */
		GList *children;/**< \brief Pointer to a child submenu for SUBMENU type */
	} data;
} func_list_t;



/** \fn void func_list_add_new_func (GList **, const char *, void (*)(GtkMenuItem *, gpointer))
 * \brief Adds a function menu item to a func_list_t list
 *
 * This function adds a menu item of type FUNCTION to a list of menu items, and
 * updates the pointer to the list passed as a parameter.
 * @param[out] list a pointer to the list where to add the menu item
 * @param[in] name the display name (must be unique in the list) of the menu item
 * @param[in] func the function to call when this menu item is clicked
 */
void func_list_add_new_func (GList **list, const char *name, void (*func)(GtkMenuItem *, gpointer));



/** \fn void func_list_add_new_submenu (GList **, const char *, GList *)
 * \brief Adds a submenu menu item to a func_list_t list
 *
 * This function adds a menu item of type SUBMENU to a list of menu items, and
 * updates the pointer to the list passed as a parameter.
 * @param[out] list a pointer to the list where to add the menu item
 * @param[in] name the display name (must be unique in the list) of the menu item
 * @param[in] children the list of menu items in the submenu of the newly created one
 */
void func_list_add_new_submenu (GList **, const char *, GList *);



/** \fn void func_list_add_new_submenu (GList **, const char *, GList *)
 * \brief Adds a separator to a func_list_t list
 *
 * This function adds a menu item of type SEPARATOR to a list of menu items, and
 * updates the pointer to the list passed as a parameter.
 * @param[out] list a pointer to the list where to add the separator
 */
void func_list_add_new_separator (GList **);


/** \fn void func_list_t_free (func_list_t *)
 * \brief Frees a func_list_t struct
 *
 * This function frees the data in a func_list_t struct, and the struct itself.
 * @param[out] fn the func_list_t struct to free
 */
void func_list_t_free (func_list_t *);



/** \privatesection */
struct __frame_t; // Defined in frame-common.h
/** \publicsection */



/** \fn GtkWidget *func_list_t_setup_submenu (func_list_t *, frame_t *)
 * \brief Turns a func_list_t's children list into a GtkMenu and frees the list
 *
 * This function, given a func_list_t of type SUBMENU, turns its children list
 * into a proper GtkMenu and returns it.
 * @param[in,out] parent the func_list_t whose submenu should be setup
 * @param[in] f the frame in whose menu the parent func_list_t will be added
 */
GtkWidget *func_list_t_setup_submenu (func_list_t *, struct __frame_t *);
#ifdef __cplusplus
}
#endif
#endif
