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
 * \file frame-saving.h
 * \brief Header for saving and restoring Synema frames
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This header file contains functions for saving and loading Synema
 * frames.
 */
#ifndef __FRAME_SAVING_H
#define __FRAME_SAVING_H
#ifdef __cplusplus
extern "C" {
#endif
#include <glib.h>

#include "frame-common.h"



/** \fn GKeyFile *frame_to_key_file (frame_t *, guint, guint)
 * \brief Saves the frame's data into a GKeyFile
 *
 * This function saves all the members of a frame_t into a GKeyFile so that it
 * can be restored from the file. The GKeyFile must be freed by calling functions,
 * with g_key_file_free
 * @param[in] frame the frame to turn into a GKeyFile
 * @param[in] position the position of the frame in it's table
 * @return the GKeyFile containing the frame's data. You must free it with g_key_file_free
 */
GKeyFile *frame_to_key_file (frame_t *, guint);



/** \fn int frame_table_save_to_files (const char *, frame_table_t *)
 * \brief Takes a list of frames from a table and saves them all in files
 *
 * This function grabs all the frames associated to a table from the Synema
 * instance's frames list, and saves all of them into different files, which are
 * written in a directory whose path is given as a parameter.
 * @param[in] savedirpath the directory in which the saved files should be put
 * @param[in] tab the frame_table_t containing the frames list to save
 * @return 0 if no error occurred, -1 otherwise
 */
int frame_table_save_to_files (const char *, frame_table_t *);



/** \fn frame_t *file_to_frame (const char *)
 * \brief Creates a frame from a .frame file
 *
 * This function turns a saved frame (with the GKeyFile syntax) into an actual
 * frame. The returned frame is initialised with the contents of the .frame file
 * and must be attached to a frame table by the calling function.
 * @param[in] path the path to the file containing the saved frame
 * @return the frame that has been restored from the file, initialised
 */
frame_t *file_to_frame (const char *);



/** \fn int frame_table_load_from_files (const char *, frame_table_t *)
 * \brief Loads frames from a save and puts them into a frames table
 *
 * This function loads frames from .frame files that have been previously saved
 * in a directory given as a parameter, and puts them in the frames table passed
 * as a parameter as well.
 * @param[in] subdirpath the path to the subdirectory where saved files are
 * @param[out] tab the frames table in which frames should be added
 * @return 0 if loading succeeds, -1 on error
 */
int frame_table_load_from_files (const char *, frame_table_t *);



//TODO doc
void save_frames_dialog_show (GtkToolButton *, gpointer);
#ifdef __cplusplus
}
#endif
#endif
