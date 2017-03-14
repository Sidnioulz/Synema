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
 * \file constants.h
 * \brief File containing various constants common to all frames
 * \author Steve Dodier <sidnioulz@gmail.com>
 *
 * This file describes contents that are not specific to a frame but that are
 * likely to be used in the whole project. It includes machine and frame related
 * constants, and group IDs. 
 */
#ifndef __CONSTANTS_H
#define __CONSTANTS_H
#ifdef __cplusplus
extern "C" {
#endif


/* Application name and version */
#define APP_NAME					"Synema"
#define APP_NAME_LOWER				"synema"
#define APP_NAME_CAPITAL			"SYNEMA"


/* Environment variables */
#define ENV_CONFIG					APP_NAME_CAPITAL"_CONFIG_DIR"
#define ENV_DATA					APP_NAME_CAPITAL"_DATA_DIR"
#define ENV_MACHINES				APP_NAME_CAPITAL"_MACHINES_DIR"
#define ENV_PLUGINS					APP_NAME_CAPITAL"_PLUGINS_DIR"
#define ENV_TMP						APP_NAME_CAPITAL"_TMP_DIR"


/* Folders and files */
#define PATH_TO_TMP_DIR				"/tmp"
#define APP_TMP_DIR					PATH_TO_TMP_DIR"/"APP_NAME_LOWER
#define APP_CONF_FILE				APP_NAME_LOWER".conf"
#define MACHINE_CONF_FILE			"machine.conf"
#define DATA_DIR					"data"
#define MACHINES_DIR				"machines"
#define LOGS_DIR					"logs"
#define REPORTS_DIR					"reports"
#define PLUGINS_DIR					"plugins"
#define SAVED_FRAMES_DIR			"saved_frames"
#define APP_LOCK_FILE				APP_NAME_LOWER".lock"
#define TAB_SAVE_SUFFIX				".tar"
#define FRAME_SAVE_SUFFIX			".frame"
#define PATH_TO_PLUGINS_DIR			CMAKE_LIB_PATH
#define FRAME_SAVING_DIR			"frame_saving"
#define FRAME_OPENING_DIR			"frame_opening"

/* Machine constants */
#define ALL_MACHINES_DISPLAY_NAME	"All Machines"
#define ALL_MACHINES_FOLDER_NAME	"__all__machines__"


/* Frame sizes (in pixels and table slots) */
#define SUPPOSED_FRAME_BORDER_SIZE		30

#define FRAME_WIDTH_NORMAL 	300.0
#define FRAME_HEIGHT_NORMAL 225.0

#define FRAME_WIDTH_LARGE 	600.0
#define FRAME_HEIGHT_LARGE 	450.0

#define NORMAL_WIDTH_SLOTS	1
#define NORMAL_HEIGHT_SLOTS	1
#define NORMAL_NB_SLOTS		1
#define NORMAL_LABEL		"Normal"

#define LARGE_WIDTH_SLOTS	2
#define LARGE_HEIGHT_SLOTS	2
#define LARGE_NB_SLOTS		4
#define LARGE_LABEL			"Large"

#define FULL_LABEL			"Full Screen"


/* Refresh time */
#define REFRESH_FREQ_AUTO					-1
#define AVG_REFRESH_TIME_NB_PASS			5
#define AUTO_REFRESH_FREQ_TIMEOUT_LEN		600


/* Settings */
#define SETTINGS_GROUP_FRAMES				"Frames"
#define SETTINGS_GROUP_FRAME_ACTIONS		"Frame Actions"
#define SETTINGS_GROUP_FRAME_TABLES			"Frame Tables"

#define SETTINGS_REFRESH_FREQ_DEFAULT		1000
#define SETTINGS_REFRESH_FREQ_MIN			250
#define SETTINGS_REFRESH_FREQ_MAX			10000
#define SETTINGS_REFRESH_FREQ_NAME			"RefreshFreq"

#define SETTINGS_AUTOSAVE_FRAMES_DEFAULT	TRUE
#define SETTINGS_AUTOSAVE_FRAMES_NAME		"AutoSaveFrames"

#define SETTINGS_LAST_CURRENT_TAB_DEFAULT	0
#define SETTINGS_LAST_CURRENT_TAB_NAME		"LastCurrentTab"

#define SETTINGS_ACTION_SET_MACHINE_DEFAULT	0
#define SETTINGS_ACTION_SET_MACHINE_NAME	"OnUnhandledMachine"

#define SETTINGS_ACTION_SET_PERIOD_DEFAULT	0
#define SETTINGS_ACTION_SET_PERIOD_NAME		"OnUnhandledPeriod"


/* Frame file header */
#define FRAME_HEADER						"## "CMAKE_BINARY"\n## "APP_NAME" Frame"


/* Frames to GKeyFile consts */
#define FTGKF_GROUP_FRAME					"Frame data"
#define FTGKF_TYPE							"Plugin Type"
#define FTGKF_SIZE							"Frame Size"
#define FTGKF_MACHINE						"Display Machine"
#define FTGKF_PERIOD						"Time Period"
#define FTGKF_LAST_MENU						"Last Called Menu"

#define FTGKF_GROUP_POSITION				"Positioning"
#define FTGKF_POSITION						"Position In Table"
#define FTGKF_NOTEBOOK_INDEX				"Notebook Index"


/* Log player */
#define PLAYER_SPEED_SCALE_INDEX_NORMAL		5
#define PLAYER_DATE_BUTTON_BUFFER_LEN		1000
#define PLAYER_DATE_BUTTON_STRFTIME_FORMAT	"%x"

#define PLAYER_PATH "/fr/ensi_bourges/"APP_NAME_LOWER"/log_player"
#define PLAYER_SERVICE_NAME "fr.ensi_bourges."APP_NAME_LOWER".log_player"
#define PLAYER_INTERFACE_NAME "fr.ensi_bourges."APP_NAME_LOWER".log_player"


#ifdef __cplusplus
}
#endif
#endif
