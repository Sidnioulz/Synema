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
 * \file machine.c
 * \brief Source code of the functions related to machines
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file contains the source code of all the machine related functions
 * decribed in machine.h.
 */
#include <dlfcn.h>
#include <errno.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "application-data.h"
#include "constants.h"
#include "machine.h"
#include "plugins.h"



/** \fn static gint machine_parse_config_file (GFile *, machine_t *)
 * \brief Parses the config file of a machine and fills it's machine_t struct
 *
 * \warning This is a private function.
 * \warning This function is currently not doing anything.
 *
 * \todo Do the actual file parsing with a GKeyFile parser.
 *
 * This function parses a known-to-exist configuration file for a given machine
 * (the machine is represented by a GFile, and the file's path is deducted
 * from this GFile). The function will at the same time fill the information
 * in the machine_t struct whose pointer has been given as a parameter.
 * @param[in] machine_file a GFile representing the machine's folder
 * @param[out] machine the struct where to put the parsed information
 * @return 0 if the configuration file was parsed, another value on error
 */
static gint machine_parse_config_file (GFile *machine_file, machine_t *machine)
{
	return 0; // XXX Not needed yet
	gchar		*name			= NULL;
	gchar		*path			= NULL;
	GError		*err			= NULL;
	GKeyFile	*keyfile		= g_key_file_new ();	

	path = g_file_get_path (machine_file);
	name = g_strdup_printf ("%s/%s", path, MACHINE_CONF_FILE);
	if (g_key_file_load_from_file (keyfile, name, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &err)) {
		g_free (path);
		g_free (name);
		g_debug ("Configuration for machine %s successfully loaded.\n", machine->display_name);
		g_key_file_free (keyfile);
		return 0;
	}
	else { // Error while loading config file
		g_warning ("%s", err->message);
		g_clear_error (&err);
		g_free (path);
		g_free (name);
		return -1;
	}
}



/** \fn static gint machine_identify_monitoring_tools (GFile *, machine_t *, GList *)
 * \brief Deducts monitoring tools from a machine folder's entries.
 *
 * \warning This is a private function.
 *
 * This function scans a directory containing reports for a given machine 
 * (identified by a GFile), in order to identify available monitoring tools for
 * this machine. It then adds the tool to the machine_t struct if it found a
 * plugin to manage it.
 * @param[in] machine_file the GFile identifying the machine
 * @param[out] machine the machine_t struct where to set the tools flags
 * @param[in] plugins the list of currently available plugins
 * @return 0 once all the tools are found, another value if an error occurred
 */
static gint machine_identify_monitoring_tools (GFile *machine_file, machine_t *machine, GList *plugins)
{
	gchar			*name			= NULL;
	gchar			*path			= NULL;
	GError			*err			= NULL;
	GFile			*reports_dir	= NULL;
	GFileEnumerator	*reports_enum	= NULL;
	GFileInfo		*current		= NULL;

	path = g_file_get_path (machine_file);
	name = g_strdup_printf ("%s/%s", path, REPORTS_DIR);
	reports_dir = g_file_new_for_path (name);
	g_free (path);
	g_free (name);

	reports_enum = g_file_enumerate_children (reports_dir, "standard::name", G_FILE_QUERY_INFO_NONE, NULL, &err);
	if (err) {
		g_warning ("machine_identify_monitoring_tools::enumerate_children: %s", err->message);
		g_clear_error (&err);
		g_object_unref (reports_dir);
		return -1;
	}

	while ((current = g_file_enumerator_next_file (reports_enum, NULL, &err)) != NULL) {
		if (err) {
			g_warning ("machine_identify_monitoring_tools::next_file: %s", err->message);
			g_clear_error (&err);
		} else {
			if (plugin_list_find_custom_list (plugins, g_file_info_get_name (current)))
				machine->monitoring_tools = g_list_prepend (machine->monitoring_tools,
												g_strdup (g_file_info_get_name (current)));
			g_object_unref (current);
		}
	}

	g_object_unref (reports_enum);
	g_object_unref (reports_dir);
	return 0;
}



/** \fn static gint machine_get_info (GFile *, GFileInfo *, machine_t *, GList *)
 * \brief Fills a machine_t struct for a given machine identified by a GFileInfo.
 *
 * \warning This is a private function.
 *
 * This function, given a GFileInfo identifying a machine folder on
 * the filesystem, fills a machine_t struct already allocated with the data it
 * finds: monitoring tools, configuration file, etc.
 * @param[in] top_dir the directory containing machine folders
 * @param[in] info the GFileInfo identifying a machine folder
 * @param[out] machine the data structure to fill with the information found
 * @param[in] plugins the list of currently available plugins
 * @return 0 if the information retrieval succeeded, another value on error
 */
static gint machine_get_info (GFile *top_dir, GFileInfo *info, machine_t *machine, GList *plugins)
{
	gchar			*name			= NULL;
	gchar			*path			= NULL;
	GError			*err			= NULL;
	GFile			*machine_file	= NULL;
	GFileEnumerator	*machine_enum	= NULL;
	GFileInfo		*current		= NULL;

	path = g_file_get_path (top_dir);
	name = g_strdup_printf ("%s/%s", path, g_file_info_get_name (info));
	machine_file = g_file_new_for_path (name);
	g_free (path);
	g_free (name);

	machine_enum = g_file_enumerate_children (machine_file, "standard::name, standard::display-name", G_FILE_QUERY_INFO_NONE, NULL, &err);
	if (err) {
		g_warning ("machine_get_info::enumerate_children: %s", err->message);
		g_clear_error (&err);
		g_object_unref (machine_file);
		return -1;
	}

	machine->_virtual = FALSE;
	machine->display_name = g_strdup (g_file_info_get_display_name (info));
	machine->folder_name = g_strdup (g_file_info_get_name (info));
	machine->monitoring_tools = NULL;

	while ((current = g_file_enumerator_next_file (machine_enum, NULL, &err)) != NULL) {
		if (err) {
			g_warning ("machine_get_info::next_file: %s", err->message);
			g_clear_error (&err);
			err = NULL;
		} else {
			if (g_strcmp0 (g_file_info_get_name (current), MACHINE_CONF_FILE) == 0) {
				if (machine_parse_config_file (machine_file, machine)) {
					g_object_unref (current);
					g_object_unref (machine_file);
					return -1;
				}
			}

			if (g_strcmp0 (g_file_info_get_name (current), REPORTS_DIR) == 0) {
				if (machine_identify_monitoring_tools (machine_file, machine, plugins)) {
					g_object_unref (current);
					g_object_unref (machine_file);
					return -1;
				}
			}
			g_object_unref (current);
		}
	}
	
	g_object_unref (machine_enum);
	g_object_unref (machine_file);
	return 0;
}



/** \fn static void g_free_func (gpointer, gpointer)
 * \brief Frees some data (GFunc version)
 *
 * \warning This is a private function.
 *
 * This function is a g_free alias, that can be used as a GFunc.
 * @param[out] data the machine to free
 * @param[out] user_data a NULL gpointer
 */
static void g_free_func (gpointer data, gpointer user_data)
{
	g_free (data);	
}



/** \fn static void machine_free (machine_t *)
 * \brief Frees a machine_t struct
 *
 * \warning This is a private function.
 *
 * This function frees a machine_t struct and its dynamically allocated members.
 * @param[out] machine the machine to free
 */
static void machine_free (machine_t *machine)
{
	g_return_if_fail (machine != NULL);

	g_list_foreach (machine->monitoring_tools, g_free_func, NULL);
	g_list_free (machine->monitoring_tools);

	g_free (machine->display_name);
	g_free (machine->folder_name);
	g_free (machine);
}



/** \fn static void machine_free_func (gpointer, gpointer)
 * \brief Frees a machine_t struct (GFunc version)
 *
 * \warning This is a private function.
 *
 * This function is a machine_free alias, that can be used as a GFunc.
 * @param[out] data the machine to free
 * @param[out] user_data a NULL gpointer
 */
static void machine_free_func (gpointer data, gpointer user_data)
{
	machine_free ((machine_t *) data);	
}



GList *machine_list_new (const gchar *top_path, GList *plugins)
{
	GError			*err		= NULL;
	GFile			*top_dir 	= NULL;
	GFileEnumerator	*dir_enum	= NULL;
	GFileInfo		*current	= NULL;
	GList			*list		= NULL;
	machine_t		*allm		= NULL;

	top_dir = g_file_new_for_path (top_path);
	dir_enum = g_file_enumerate_children (top_dir, "standard::name,standard::display-name", G_FILE_QUERY_INFO_NONE, NULL, &err);
	if (err) {
		g_warning ("machine_list_new::enumerate_children: %s", err->message);
		g_clear_error (&err);
		return NULL;
	}

	allm = g_malloc (sizeof (machine_t));
	allm->_virtual = TRUE;
	allm->display_name = g_strdup (ALL_MACHINES_DISPLAY_NAME);
	allm->folder_name = g_strdup (ALL_MACHINES_FOLDER_NAME);
	allm->monitoring_tools = NULL;

	while ((current = g_file_enumerator_next_file (dir_enum, NULL, &err)) != NULL) {
		if (err) {
			g_warning ("machine_list_new::next_file: %s", err->message);
			g_clear_error (&err);
		} else {
			if (g_strcmp0 (g_file_info_get_name (current), ALL_MACHINES_FOLDER_NAME) != 0) {
				machine_t *machine = g_malloc (sizeof (machine_t));
				if (machine_get_info (top_dir, current, machine, plugins) != 0)
					g_free (machine);
				else
					list = g_list_prepend (list, machine);
				g_object_unref (current);
			}
		}
	}

	if (list) {
		list = g_list_append (list, allm);
	} else {
		machine_free (allm);
	}

	g_object_unref (dir_enum);
	g_object_unref (top_dir);
	return list;
}



void machine_list_free (GList *list)
{
	g_return_if_fail (list != NULL);

	g_list_foreach (list, machine_free_func, NULL);
	g_list_free (list);
}



machine_t *machine_list_find_by_name (GList *list, const gchar *name)
{
	GList *iter = list;
	
	if (iter) {
		do {
			if (g_strcmp0 (machine_get_folder_name (iter->data), name) == 0)
				return iter->data;
		} while ((iter = g_list_next (iter)) != NULL);
	}

	return NULL;
}



/** \fn static gint machine_cmp_func (gconstpointer, gconstpointer)
 * \brief Compares two machines (GCompareFunc version)
 *
 * \warning This is a private function.
 *
 * This function is a machine_cmp alias, that can be used as a GCompareFunc.
 * @param[in] m1 the first machine
 * @param[in] m2 the second machine
 */
static gint machine_cmp_func (gconstpointer m1, gconstpointer m2)
{
	return machine_cmp ((machine_t *) m1, (machine_t *) m2);
}



gint machine_list_remove (GList **list, machine_t *elem)
{
	g_return_val_if_fail (*list != NULL, -1);

	GList	*elemptr = g_list_find_custom (*list, (gpointer) elem, machine_cmp_func);
	if (elemptr) {
		*list = g_list_remove (*list, elemptr->data);
		return 0;
	} else {
		return -1;
	}
}



gint machine_cmp (machine_t *m1, machine_t *m2)
{
	if (m1 == NULL)
		return (m2 == NULL) ? 0 : -1;

	if (m2 == NULL)
		return 1;

	return g_strcmp0 (m1->folder_name, m2->folder_name);
}



const gchar *machine_get_display_name (machine_t *m)
{
	return (m) ? m->display_name : NULL;
}



const gchar *machine_get_folder_name (machine_t *m)
{
	return (m) ? m->folder_name : NULL;
}



gint machine_is_all_machines (machine_t *m)
{
	g_return_val_if_fail (m != NULL, 0);

	if ((m->_virtual == FALSE) || (g_strcmp0 (m->folder_name, ALL_MACHINES_FOLDER_NAME)))
		return 0;
	else
		return 1;
}



GList *machine_get_list_for_plugin (const gchar *plugin_name, gboolean add_all_machines)
{
	synema_instance_t	*inst			= synema_instance ();
	gchar				*reports_dir	= NULL;
	GList				*final_list		= NULL;
	GList				*iter			= inst->machines_list;
	struct stat			buff;

	if (iter) {
		do {
			if ((machine_is_all_machines (iter->data)) && (add_all_machines)) {
				final_list = g_list_prepend (final_list, iter->data);
			} else {
				reports_dir = g_strdup_printf ("%s/%s/"REPORTS_DIR"/%s",
											inst->machines_dir,
											machine_get_folder_name (iter->data),
											plugin_name);

				if (g_stat (reports_dir, &buff) == 0) {
					if (S_ISDIR (buff.st_mode))
						final_list = g_list_prepend (final_list, iter->data);
				}

				g_free (reports_dir);
			}
		} while ((iter = g_list_next (iter)) != NULL);
	}

	return final_list;
}
