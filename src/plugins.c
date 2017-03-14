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
 * \file plugins.c
 * \brief Source code for plugin utilities
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This source code file contains plugin related functions.
 */
#include <gio/gio.h>

#include "application-data.h"
#include "plugins.h"



GList *plugin_list_new (const gchar *plugins_dir)
{
	const gchar		*filename	= NULL;
	gchar			*pluginname	= NULL;
	GError			*err		= NULL;
	GFile			*file		= g_file_new_for_path (plugins_dir);
	GFileEnumerator	*fileenum	= g_file_enumerate_children (file, "standard::name", G_FILE_QUERY_INFO_NONE, NULL, &err);
	GFileInfo		*current	= NULL;
	gint			len			= 0;
	GList			*list		= NULL;

	if (err) {
		g_warning ("plugin_list_new: Error while trying to read the directory containing plugins (%s)", err->message);
		g_clear_error (&err);
		return NULL;
	}

	while ((current = g_file_enumerator_next_file (fileenum, NULL, &err)) != NULL) {
		if (err) {
			g_warning ("plugin_list_new: Error while reading a file in the plugins directory (%s)", err->message);
			g_clear_error (&err);
		} else {
			filename = g_file_info_get_name (current);
			len = g_strrstr (filename, ".so") - filename;

			pluginname = g_strndup (filename, len);
			list = g_list_prepend (list, plugin_new (plugins_dir, pluginname));

			g_free (pluginname);
			g_object_unref (current);
		}
	}

	g_object_unref (fileenum);
	g_object_unref (file);
	return list;
}



void plugin_list_free (GList *list)
{
	if (list) {
		GList *iter = list;
		do {
			plugin_free (iter->data);
		} while ((iter = g_list_next (iter)) != NULL);
	}

	g_list_free (list);
}



plugin_t *plugin_new (const gchar *plugins_dir, const gchar *name)
{
	plugin_t	*pt		= g_malloc (sizeof (plugin_t));
	gchar		*path	= g_strdup_printf ("%s/%s.so", plugins_dir, name);
	char		*err	= NULL;

	pt->name = g_strdup (name);	
	pt->handle = dlopen (path, RTLD_NOW);
	g_free (path);
	
	err = dlerror ();
	if (err) {
		g_warning ("plugin_new: %s", err);
		return NULL;
	}

	return pt;
}



void *plugin_get_symbol (plugin_t *pt, const char *symbol_name, gboolean display_warnings)
{
	g_return_val_if_fail (pt != NULL, NULL);

	char	*err	= NULL;
	void	*symbol	= NULL;

	* (void **) &symbol = dlsym (pt->handle, symbol_name);
	err = dlerror ();
	if (err) {
		if (display_warnings)
			g_warning ("plugin_get_symbol: %s", err);
		return NULL;
	}

	return symbol;
}



void plugin_free (plugin_t *pt)
{
	dlclose (pt->handle);
	g_free (pt->name);
	g_free (pt);
}


plugin_t *plugin_list_find_custom_list (GList *list, const gchar *name)
{
	GList			*iter	= list;
	plugin_t		*pt		= NULL;

	if (iter) {
		do {
			pt = iter->data;
			if (g_strcmp0 (name, pt->name) == 0)
				return pt;
		} while ((iter = g_list_next (iter)) != NULL);
	}

	return NULL;
}



plugin_t *plugin_list_find (const gchar *name)
{
	synema_instance_t	*inst	= synema_instance ();
	return (plugin_list_find_custom_list (inst->plugins_list, name));
}



gboolean plugin_exists (const gchar *name)
{
	return plugin_list_find (name) != NULL;
}



gchar *plugin_get_display_name (plugin_t *pt)
{
	g_return_val_if_fail (pt != NULL, NULL);

	gchar *(*func)(frame_t *) = NULL;

	*(void **) (&func) = plugin_get_symbol (pt, "get_display_name", TRUE);
	if (!func) {
		g_warning ("plugin_get_display_name: Failed to get the display name");
		return NULL;
	}

	return func(NULL);
}



GList *plugin_get_time_periods (plugin_t *pt)
{
	g_return_val_if_fail (pt != NULL, NULL);

	GList *(*func)() = NULL;

	*(void **) (&func) = plugin_get_symbol (pt, "build_time_periods", TRUE);
	if (!func) {
		g_warning ("plugin_get_time_periods: Failed to get the list of time periods");
		return NULL;
	}

	return func();
}
