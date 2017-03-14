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
 * \file criticality.c
 * \brief Correlation plugin
 * \author Aline BOUSQUET <aline.bousquet@ensi-bourges.fr>
 *
 * This file contains the source code for the SELinux plugin.
 */
#define _GNU_SOURCE

#include "criticality.h"

int find_pattern (char * sensor, char * log, criticality_struct * criticality, FILE * report_file, FILE * error_log_file)
{
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;

	char 		pattern[256];
	char 		time_pattern[256];
	char 		string[256];
	char 		rule_name[256];
	char 		regex[256];
	char 		regex_value[256];
	char 		regex_value_tmp[256];
	int 		date_found 			= 0;

	FILE 		* regex_file 			= fopen (REGEX_FILE, "r");
	if (regex_file == NULL) { 
		perror("Error openning the regex file\n");
		return -1;
	}

	sprintf (time_pattern, "[0-9]* %s ::time::(.*)", sensor);	
	sprintf (pattern, "([0-9]*) (%s) ::regex::(.*)", sensor);
	
	// For each regex in regex file
	while(fgets (string, 256, regex_file) != NULL) {
		// If date hasn't been found yet
		if (date_found == 0) {
			// Is the regex a time regex?
			re = pcre_compile(time_pattern, 0, &error, &erroffset, NULL);  
			if (re == NULL) {
				if (error_log_file != NULL)
					  fprintf(error_log_file, "find_pattern(): PCRE compilation failed at offset %d: %s\n", erroffset, error);
				  return -1;
			}
			rc = pcre_exec(re, NULL, string, (int)strlen(string), 0, 0, ovector, OVECCOUNT);    
			if (rc >0) {
				// If regex is a time regex, extract date
				sprintf(regex, "%.*s", ovector[3] - ovector[2], string + ovector[2]);
				date_found = log_date_handler(sensor, log, regex, criticality, error_log_file);	
			}
		}

		// Extract the regex from regex line
		re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
		if (re == NULL) {
			if (error_log_file != NULL)
				fprintf(error_log_file, "find_pattern(): PCRE compilation failed at offset %d: %s\n", erroffset, error);
			return -1;
		}
		rc = pcre_exec(re, NULL, string, (int)strlen(string), 0, 0, ovector, OVECCOUNT);    
		if (rc > 0) {
			sprintf(rule_name, "%.*s", ovector[3] - ovector[2], string + ovector[2]);
			sprintf(regex, "%.*s", ovector[7] - ovector[6], string + ovector[6]);
			criticality->regex_rule_number = atoi (rule_name);
			// Try to match the log with the regex 
			re = pcre_compile(regex, 0, &error, &erroffset, NULL);  
			if (re == NULL) {
				if (error_log_file != NULL)
					  fprintf(error_log_file, "PCRE compilation failed at offset %d: %s\n", erroffset, error);
				  return -1;
			}
			rc = pcre_exec(re, NULL, log, (int)strlen(log), 0, 0, ovector, OVECCOUNT);    
			if (rc > 0) { 
				// If the match was successful, look for the log criticality 
				sprintf(regex_value, "%.*s", ovector[3] - ovector[2], log + ovector[2]);
				int i;

				if ( (strcmp(sensor, SNORT) == 0) && (strcmp (regex_value, "Priority:") == 0) ) {
					sprintf(regex_value, "%.*s", ovector[5] - ovector[4], log + ovector[4]);
					// If the criticality is given in the log
					criticality->value = atoi (regex_value);
					criticality->value_rule_number = 0;
				}
				else {
					// Read criticality in value file
					for (i = 2; i < rc; i++) {
						sprintf(regex_value_tmp, "::%.*s", ovector[2*i+1] - ovector[2*i], log + ovector[2*i]);
						strcat(regex_value, regex_value_tmp);
					}
					read_criticality ( criticality, regex_value, error_log_file);
				}
				if (report_file != NULL && criticality->value != -1 && criticality->criticality_time->tm_year != 1) {
					// Save the log infos
					save_criticality (sensor, criticality, report_file );
				}
			}
		}
	}
	pcre_free(re);
	fclose (regex_file);

	if (criticality->value != -1)
		return 0;
	else 
		return 1;
}


int read_criticality (criticality_struct * criticality, char * value, FILE * error_log_file) 
{
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;

	char 		pattern[256];
	char 		string[256];
	char 		criticality_name[10];
	char 		rule_name[256];

	FILE 		* fp 			= fopen (VALUE_FILE, "r");

	if (fp == NULL) { 
		perror("Error openning the file\n");
		return -1;
	}
	// Extract criticality from the value rule
	sprintf (pattern, "([0-9]*) (%i) (%s) ([0-9]*)", criticality->regex_rule_number, value);
	while(fgets (string, 256, fp) != NULL) {
		re = pcre_compile(pattern, 0, &error, &erroffset, NULL);  
		if (re == NULL) {
			if (error_log_file != NULL)
				fprintf(error_log_file, "read_criticality(): PCRE compilation failed at offset %d: %s\n", erroffset, error);
			return -1;
		}
		rc = pcre_exec(re, NULL, string, (int)strlen(string), 0, 0, ovector, OVECCOUNT);    
		if (rc > 0) {
			sprintf(criticality_name, "%.*s\n", ovector[9] - ovector[8], string + ovector[8]);
			sprintf(rule_name, "%.*s", ovector[3] - ovector[2], string + ovector[2]);
			criticality->value = atoi (criticality_name);
			criticality->value_rule_number = atoi (rule_name);
		}
	}
	
	pcre_free (re);
	fclose (fp);
	
	return 0;
}


int save_criticality (char * sensor, criticality_struct * criticality, FILE * report_file )
{
	if (criticality->criticality_time->tm_year < 1900) {
		criticality->criticality_time->tm_year = criticality->criticality_time->tm_year + 1900;
		criticality->criticality_time->tm_mon = criticality->criticality_time->tm_mon + 1;
	}

	char * day = NULL, * month = NULL, * hour = NULL, * min = NULL, * sec = NULL;
	if (criticality->criticality_time->tm_mday < 10)
		day = g_strdup_printf("0%i", criticality->criticality_time->tm_mday);
	else 
		day = g_strdup_printf("%i", criticality->criticality_time->tm_mday);

	if (criticality->criticality_time->tm_mon < 10)
		month = g_strdup_printf("0%i", criticality->criticality_time->tm_mon);
	else 
		month = g_strdup_printf("%i", criticality->criticality_time->tm_mon);

	if (criticality->criticality_time->tm_hour < 10)
		hour = g_strdup_printf("0%i", criticality->criticality_time->tm_hour);
	else 
		hour = g_strdup_printf("%i", criticality->criticality_time->tm_hour);

	if (criticality->criticality_time->tm_min < 10)
		min = g_strdup_printf("0%i", criticality->criticality_time->tm_min);
	else 
		min = g_strdup_printf("%i", criticality->criticality_time->tm_min);

	if (criticality->criticality_time->tm_sec < 10)
		sec = g_strdup_printf("0%i", criticality->criticality_time->tm_sec);
	else 
		sec = g_strdup_printf("%i", criticality->criticality_time->tm_sec);
	fprintf (report_file, "%s/%s/%i %s:%s:%s %s %i %i %i\n", 
			day, 
			month, 
			criticality->criticality_time->tm_year, 
			hour, 
			min, 
			sec, 
			sensor,
			criticality->regex_rule_number, 
			criticality->value_rule_number, 
			criticality->value);
	
	g_free (day);
	g_free (month);
	g_free (hour);
	g_free (min);
	g_free (sec);
	
			
	return 0;
}


int log_date_handler(char * sensor, char * log, char * regex, criticality_struct * criticality, FILE * error_log_file)
{
	pcre 		* re;
	const char 	* error;
	int 		erroffset;
	int 		ovector[OVECCOUNT];
	int 		rc;

	date_struct	date_tmp;
	// Try to match the date pattern with the log 
	re = pcre_compile(regex, 0, &error, &erroffset, NULL);  
	if (re == NULL) {
		if (error_log_file != NULL)
			fprintf(error_log_file, "log_date_handler(): PCRE compilation failed at offset %d: %s\n", erroffset, error);
		return -1;
	}
	rc = pcre_exec(re, NULL, log, (int)strlen(log), 0, 0, ovector, OVECCOUNT);   
	if (rc > 0) { 
		// If the match was successful, extract date and save it in a struct tm
		if ( (strcmp (sensor, OSIRIS) == 0)) {
			sprintf(date_tmp.Year, "%.*s", ovector[15] - ovector[14], log + ovector[14]);				
			sprintf(date_tmp.Month, "%.*s", ovector[5] - ovector[4], log + ovector[4]);
			sprintf(date_tmp.Day, "%.*s", ovector[7] - ovector[6], log + ovector[6]);
			sprintf(date_tmp.Hour, "%.*s", ovector[9] - ovector[8], log + ovector[8]);
			sprintf(date_tmp.Min, "%.*s", ovector[11] - ovector[10], log + ovector[10]);
			sprintf(date_tmp.Sec, "%.*s", ovector[13] - ovector[12], log + ovector[12]);

			criticality->criticality_time->tm_year = atoi (date_tmp.Year);
			criticality->criticality_time->tm_mon = get_month (date_tmp.Month);
			criticality->criticality_time->tm_mday = atoi (date_tmp.Day);
			criticality->criticality_time->tm_hour = atoi (date_tmp.Hour);
			criticality->criticality_time->tm_min = atoi (date_tmp.Min);
			criticality->criticality_time->tm_sec = atoi (date_tmp.Sec);
		}
		else if (strcmp (sensor, SELINUX) == 0) {
			char time_sec[256];
			sprintf(time_sec, "%.*s", ovector[3] - ovector[2], log + ovector[2]);
			int time_sec_value = atoi (time_sec);

			criticality->criticality_time = gmtime((time_t*)&time_sec_value);
			criticality->criticality_time->tm_year += 1900;
			criticality->criticality_time->tm_mon += 1;
		}
		else if (strcmp (sensor, SNORT) == 0) {
			sprintf(date_tmp.Month, "%.*s", ovector[3] - ovector[2], log + ovector[2]);
			sprintf(date_tmp.Day, "%.*s", ovector[5] - ovector[4], log + ovector[4]);
			sprintf(date_tmp.Hour, "%.*s", ovector[7] - ovector[6], log + ovector[6]);
			sprintf(date_tmp.Min, "%.*s", ovector[9] - ovector[8], log + ovector[8]);
			sprintf(date_tmp.Sec, "%.*s", ovector[11] - ovector[10], log + ovector[10]);

			time_t current_time = time(NULL);
			criticality->criticality_time = gmtime(&current_time);
			criticality->criticality_time->tm_year += 1900;

			criticality->criticality_time->tm_mon = atoi (date_tmp.Month);
			criticality->criticality_time->tm_mday = atoi (date_tmp.Day);
			criticality->criticality_time->tm_hour = atoi (date_tmp.Hour);
			criticality->criticality_time->tm_min = atoi (date_tmp.Min);
			criticality->criticality_time->tm_sec = atoi (date_tmp.Sec);
		}
		else if (strcmp (sensor, SYSLOG) == 0) {
			sprintf(date_tmp.Month, "%.*s", ovector[3] - ovector[2], log + ovector[2]);
			sprintf(date_tmp.Day, "%.*s", ovector[5] - ovector[4], log + ovector[4]);
			sprintf(date_tmp.Hour, "%.*s", ovector[7] - ovector[6], log + ovector[6]);
			sprintf(date_tmp.Min, "%.*s", ovector[9] - ovector[8], log + ovector[8]);
			sprintf(date_tmp.Sec, "%.*s", ovector[11] - ovector[10], log + ovector[10]);

			time_t current_time = time(NULL);
			criticality->criticality_time = gmtime(&current_time);
			criticality->criticality_time->tm_year += 1900;

			criticality->criticality_time->tm_mon = get_month (date_tmp.Month);
			criticality->criticality_time->tm_mday = atoi (date_tmp.Day);
			criticality->criticality_time->tm_hour = atoi (date_tmp.Hour);
			criticality->criticality_time->tm_min = atoi (date_tmp.Min);
			criticality->criticality_time->tm_sec = atoi (date_tmp.Sec);
		}
		pcre_free(re);
	
		return 1;
	}
	
		
	pcre_free(re);
	
	return 0;
}

int get_month (char * month) 
{
	if ( strstr(month, "Jan") != NULL)
		return 1;
	else if ( strstr(month, "Feb") != NULL)
		return 2;
	else if ( strstr(month, "Mar") != NULL)
		return 3;
	else if ( strstr(month, "Apr") != NULL)
		return 4;
	else if ( strstr(month, "May") != NULL)
		return 5;
	else if ( strstr(month, "Jun") != NULL)
		return 6;
	else if ( strstr(month, "Jul") != NULL)
		return 7;
	else if ( strstr(month, "Aug") != NULL)
		return 8;
	else if ( strstr(month, "Sep") != NULL)
		return 9;
	else if ( strstr(month, "Oct") != NULL)
		return 10;
	else if ( strstr(month, "Nov") != NULL)
		return 11;
	else if ( strstr(month, "Dec") != NULL)
		return 12;
	else
		return 1;
}


char * read_criticality_get_log(char * sensor, char * log_line, FILE * log_file)
{
	sprintf(log_line, "%s", "\0");
	
	if (strcmp(sensor, SNORT) == 0) { 
		char 	tmp[256];
		while ( (fgets(tmp, 256, log_file) != NULL) && (strcmp(tmp, "\n") != 0) ) {
			strcat (log_line, tmp);
		}
	}
	else 
		log_line = fgets (log_line, 256, log_file);

	if (log_line != NULL && strcmp(log_line, "\0") != 0)
		return log_line; 
	else 
		return NULL;
}


