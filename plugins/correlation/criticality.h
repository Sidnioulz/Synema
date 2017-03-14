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
 * \file criticality.h
 * \brief Correlation plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 *
 * This header file contains functions for the SELinux plugin.
 */

#ifndef __CRITICALITY_H__
#define __CRITICALITY_H__
#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pcre.h>
#include <time.h>

#include "correlation.h"

/* Regex files */
#define REGEX_FILE "./plugins/correlation/data/regex"
#define VALUE_FILE "./plugins/correlation/data/value"

/* Sensors */
#define OSIRIS "osiris"
#define SELINUX "selinux"
#define SNORT "snort"
#define SYSLOG "syslog"


typedef struct
{
	int regex_rule_number;
	int value_rule_number;
	int value;
	struct tm * criticality_time;
}criticality_struct;


typedef struct
{
	char	Year[5];
	char	Month[3];
	char	Day[3];
	char	Hour[3];
	char	Min[3];
	char	Sec[3];
}date_struct;


/** \int find_pattern (char * sensor, char * log)
 * \brief Select the pattern for the appropriate sensor
 *
 * This function selects each pattern in the regex file for the given sensor.
 * Then, it tries to match the log and the new regex.
 * @param[in] sensor the sensor for which regex are looked for 
 * @param[in] log the log to be analysed
 * @param[in] criticality the struct in which the log informations are stocked
 */
int find_pattern (char * sensor, char * log, criticality_struct * criticality, FILE * report_file, FILE * error_log_file );


/** \fn int read_criticality (int rule, char * value)
 * \brief Determine the criticality level.
 *
 * This function looks for the criticity associated with the value 
 * of the regex find in a log.
 * @param[in] criticality the struct in which the log informations are stocked
 * @param[in] value the value taken by the regex
 */
int read_criticality (criticality_struct * criticality, char * value, FILE * error_log_file);


/** \fn int save_criticality (criticality_struct * criticality)
 * \brief Write the criticality in a file
 *
 * This function writes the log criticality and other informations 
 * in a report file.
 * @param[in] criticality the struct in which the log informations are stocked
 */
int save_criticality (char * sensor, criticality_struct * criticality, FILE * report_file );


/** \int log_date_handler(char * sensor, char * log, char * regex, criticality_struct * criticality)
 * \brief Handle the log date for each sensor
 *
 * This function extracts dates from logs for every sensor and puts
 * them in a specified format.
 * @param[in] sensor the sensor from which came the date 
 * @param[in] log the log to be analysed
 * @param[in] regex the looked-for date format
 * @param[in] criticality the struct in which the log informations are stocked
 */
int log_date_handler(char * sensor, char * log, char * regex, criticality_struct * criticality, FILE * error_log_file);


/** \fn int get_month (char * month)
 * \brief Give the number of a month
 *
 * This function returns the number of a month
 * (January -> 1)
 * @param[in] month the name of the month
 */
int get_month (char * month); 


/** \fn char * read_criticality_get_log (char * sensor, char * log_line, FILE * log_file);
 * \brief Get a log from a log file
 *
 * This function extracts a log from a log file and returns it.
 * @param[in] sensor the sensor corresponding to the log file
 * @param[in] log_line the string to fill with the extracted log
 * @param[in] log_file the file from which the log must be extracted
 */
char * read_criticality_get_log(char * sensor, char * log_line, FILE * log_file);


#endif
#ifdef __cplusplus
}
#endif
