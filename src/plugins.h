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
 * \file plugins.h
 * \brief Header for plugin utilities
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This header file contains the plugin_t data structure and plugin related
 * functions.
 */
#ifndef __PLUGINS_H
#define __PLUGINS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <glib.h>



/** \struct plugin_t
 * \brief Describes a Synema plugin
 *
 * This structure describes a plugin, which is made of a name that is proper to
 * this plugin, and a dlhandle used for loading the plugin's symbols dynamically.
 */
typedef struct {
	gchar			*name;		/**< \brief name of the plugin (unique identifier) */
	void			*handle;	/**< \brief dlhandle for the plugin's shared library */
} plugin_t;



/** \fn GList *plugin_list_new (const gchar *)
 * \brief Creates a list of plugins available in the given directory
 *
 * This function creates a list of plugins based on the .so files
 * available in the directory given as a parameter.
 * @param[in] plugins_dir the directory to scan for plugins
 * @return a list of pointers to plugin_t structs
 */
GList *plugin_list_new (const gchar *);



/** \fn void plugin_list_free (GList *)
 * \brief Frees a list of plugins
 *
 * This function frees the plugin_t structs contained in a given list of plugin_t
 * pointers.
 * @param[out] list the list of plugins to free
 */
void plugin_list_free (GList *list);



/** \fn plugin_t *plugin_new (const gchar *, const gchar *)
 * \brief Creates a new plugin
 *
 * This function creates a plugin, with a name given as a parameter. It gets the
 * dlhandle from a .so file which should be in plugins_dir.
 * @param[in] plugins_dir the directory in which to look for the .so file matching
 * name
 * @param[in] name the name to give to the plugin
 * @return a new plugin_t that should be freed with plugin_free
 */
plugin_t *plugin_new (const gchar *, const gchar *);



/** \fn void *plugin_get_symbol (plugin_t *, const char *, gboolean)
 * \brief Gets a symbol in a plugin's library file
 *
 * This function gets a symbol in a plugin. The name of the symbol is given as
 * a parameter.
 * @param[in] pt the plugin whose symbol to get
 * @param[in] symbol_name the name of the symbol
 * @param[in] display_warnings whether to display warnings if the symbol isn't
 * found.
 * @return the symbol if it was found, or NULL otherwise
 */
void *plugin_get_symbol (plugin_t *, const char *, gboolean);



/** \fn void plugin_free (plugin_t *)
 * \brief Frees a plugin_t struct
 *
 * This function frees a plugin.
 * @param[out] pt the plugin to free
 */
void plugin_free (plugin_t *);



/** \fn plugin_t *plugin_list_find_custom_list (GList *, const gchar *)
 * \brief Find a plugin matching name in a given list of plugins
 *
 * This function searches through a list of plugin_t structs given as a parameter
 * for a plugin whose name is also given as a parameter.
 * @param[in] list the list in which to search
 * @param[in] name the name of the plugin to find
 * @return the plugin if it was found, NULL otherwise
 */
plugin_t *plugin_list_find_custom_list (GList *, const gchar *);


/** \fn plugin_t *plugin_list_find (const gchar *)
 * \brief Searches the Synema instance for a plugin
 *
 * This function is a wrapper for plugin_list_find_custom_list which will use
 * the application instance's list of plugins as a list.
 * @param[in] name the name of the wanted plugin
 * @return the plugin if it was found, NULL otherwise
 */
plugin_t *plugin_list_find (const gchar *);



/** \fn gboolean plugin_exists (const gchar *)
 * \brief Checks if a plugin exists
 *
 * This function checks if a plugin exists by calling plugin_list_find and
 * checking if the result is different from NULL.
 * @param[in] name the name of the plugin to look for
 * @return TRUE if the plugin exists, FALSE otherwise
 */
gboolean plugin_exists (const gchar *);



/** \fn gchar *plugin_get_display_name (plugin_t *)
 * \brief Returns the display name of a plugin 
 *
 * This function returns the display name of a plugin, as a static string. Don't
 * modify or free it.
 * @param[in] pt the plugin
 * @return the display name of the plugin. Do not modify or free this string
 */
gchar *plugin_get_display_name (plugin_t *);



/** \fn GList *plugin_get_time_periods (plugin_t *)
 * \brief Returns the time periods available for a plugin
 *
 * This function returns a GList of pointers to time_period_t available for a
 * given plugin.
 * @param[in] pt the plugin
 * @return a GList of time periods available for this plugin
 */
GList *plugin_get_time_periods (plugin_t *);
#ifdef __cplusplus
}
#endif
#endif
