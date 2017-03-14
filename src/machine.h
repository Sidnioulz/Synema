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
 * \file machine.h
 * \brief Header for the machines related functions in Synema
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This header files provides utilities related to machines and describes the
 * machine_t data structure.
 */
#ifndef __MACHINE_H
#define __MACHINE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <glib.h>



/** \struct machine_t
 * \brief A machine with it's internal data and list of available tools 
 *
 * This structure contains data about a particular machine: folder where it's
 * data is contained, list of monitoring tools available, and OS and hardware
 * information.
 */
typedef struct __machine_t {
	gboolean	_virtual;			/**< \brief Tells if the machine is a virtual one (such as ALL MACHINES) */
	gchar		*display_name;		/**< \brief Display name for the machine */
	gchar		*folder_name;		/**< \brief Name of the machine's folder. Can be used as an identifier */
	GList		*monitoring_tools;	/**< \brief A list of gchar* identifying monitoring tools available for this machine */
} machine_t;



/** \fn GList *machine_list_new (const gchar*, GList *)
 * \brief Scans a machines folder and returns a list of machines present in this folder
 *
 * This function scans a folder to find machine directories inside it, and builds
 * a list of machine_t for each machine.
 * @param[in] top_path the path in which the machines are
 * @param[in] plugins the list of currently available plugins
 * @return a list of machines
 */
GList *machine_list_new (const gchar *, GList *);



/** \fn void machine_list_free (GList *)
 * \brief Frees a list of machines and the machines it contains
 *
 * This functions frees all the machines present in a given list, and then frees
 * the list.
 * @param[in] list the list to free
 */
void machine_list_free (GList *);



/** \fn gint machine_list_remove (GList **, machine_t *)
 * \brief Removes a machine from a machines list without freeing it.
 *
 * This functions removes a machine from a machines list. It does not free the
 * machine.
 * @param[in,out] list a pointer to the list (can be modified) 
 * @param[in,out] elem the element to remove
 * @return 0 if the machine was in the list and removed, -1 otherwise
 */
gint machine_list_remove (GList **, machine_t *);



/** \fn machine_t *machine_list_find_by_name (GList *, const gchar *)
 * \brief Searches a machines list for a machine whose name is given as a parameter
 *
 * This function searches through a list of machines for one whose folder name
 * matches the one given as a parameter. It returns NULL if the machine is not
 * in the list.
 * @param[in] list the list of machines to browse
 * @param[in] name the folder name to look for
 * @return the machine if it was found, NULL otherwise
 */
machine_t *machine_list_find_by_name (GList *, const gchar *);



/** \fn gint machine_cmp (machine_t *, machine_t *)
 * \brief Compares two machines
 *
 * This functions compares two machine_t structs and tells if the machines are
 * identical. It may be appropriate for sorting if both parameters are not NULL.
 * It will then return the alphabetical order of the folder names of the machines.
 * @param[in] m1 the first machine
 * @param[in] m2 the second machine
 * @return 0 if the machines are identical, a negative value if m1<m2 and a positive value if m1>m2
 */
gint machine_cmp (machine_t *, machine_t *);



/** \fn const gchar *machine_get_display_name (machine_t *)
 * \brief Returns the display name of a machine
 *
 * This functions returns the display name of a machine. It should not be freed
 * because it belongs to the machine_t struct.
 * @param[in] m the machine whose name you want
 * @return the display name of the machine (you must not free it)
 */
const gchar *machine_get_display_name (machine_t *);



/** \fn const gchar *machine_get_folder_name (machine_t *)
 * \brief Returns the folder name of a machine
 *
 * This functions returns the folder name of a machine. It should not be freed
 * because it belongs to the machine_t struct.
 * @param[in] m the machine whose name you want
 * @return the folder name of the machine (you must not free it)
 */
const gchar *machine_get_folder_name (machine_t *);



/** \fn gint machine_is_all_machines (machine_t *)
 * \brief Tells if a machine_t struct represents all the machines
 *
 * This functions tells if the given machine_t is identical to a special virtual
 * machine_t struct that represents all the machines available in the application.
 * @param[in] m the machine to check
 * @return 0 if the machine is not the virtual machine_t representing all the machines, 1 if it is.
 */
gint machine_is_all_machines (machine_t *);



/** \fn GList *machine_get_list_for_plugin (const gchar *, gboolean)
 * \brief Returns a list of machines that have reports for a given plugin
 *
 * This function goes through the machines list of the current Synema instance
 * and returns a list of pointers to machines that have reports for a plugin whose
 * name is given as a parameter.
 * @param[in] plugin_name the name of the plugin for which you want a list of machines
 * @param[in] add_all_machines if TRUE, the virtual machine "All Machines" is added to the list (if it's not empty)
 * @return a list of machines which have reports for the plugin, NULL if none
 */
GList *machine_get_list_for_plugin (const gchar *, gboolean);

#ifdef __cplusplus
}
#endif
#endif
