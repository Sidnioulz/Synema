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
 * \file priority.c
 * \brief Fake Snort plugin written by the twentieth group
 * \author Vincent Jaouen <vincent.jaouen@ensi-bourges.fr>
 * \author Bénédicte Augustin <benedicte.augustin@ensi-bourges.com>
 *
 * This file contains a fake Snort plugin for demonstration purpose.
 */
 
# define _GNU_SOURCE
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "frame-utilities.h"
# include "file_manip.h"
# include "plugin.h"
# include "fonctions.h"

int script_priorite (gpointer user_data)
{
printf("test\n\n");
	frame_t *f = (frame_t *) user_data;
	data_affichage *data = (data_affichage *) f->priv;

	data->type = TYPE_ATTACKS;
	data->attack_type = PRIORITY;

	FILE * report;
	FILE * script;
	char * command_1, * command_2, * command_3;
	int nb = 0;
	ssize_t r;
	char * c1 = calloc (1000, sizeof(char));
	char * c2 = calloc (1000, sizeof(char));
	int little = 0, normal = 0, big = 0, very_big = 0;
	int total = 0;	
	int errno = 0;
	size_t n;
	
	data->name_foreground_file = frame_get_absolute_tmp_path (f, "img.svg");
	command_1 = get_report_name(f);
	command_2 =frame_get_absolute_tmp_path (f, "priority");
	
	report = fopen(command_1, "r");

	if (report == NULL) {
//		data->name_foreground_file = frame_get_absolute_data_path (f, "white.png");
        	printf("the program can't open the wanted file %s\n", command_1);
        	return 0;
	}

	script = fopen(command_2, "w+");

	if (script == NULL) {
//        	data->name_foreground_file = frame_get_absolute_data_path (f, "white.png");
        	printf("the program can't open the wanted file %s\n", command_2);
        	return 0;
	}   

	/* reading of datas and calculation of the number of priority*/

	r = getline(&c1, &n, report);
	
	while (r > 0)
	{
		sscanf(c1, "%s", c2);
		if (c2 != NULL && strcasecmp(c2,"Priority:") == 0)
		{
			sscanf(c1, "%s%i", c2, &nb);
			switch (nb) {
				case 1:
					very_big++;
					break;
				case 2:
					big++;
					break;
				case 3:
					normal++;
					break;
				case 4:
					little++;
					break;
				default:
              			        printf ("There are others priority for the different attacks\n");
					break;
			}
		}
		else if (c2 == NULL)
		{
			printf ("error : c2 = NULL\n");
		}
		
		r = getline(&c1, &n, report);
	}
	free (c1);
	
/*	if (r == 0 || r < 0)
	{
		printf ("Error : %s\n", strerror(errno));
	}
*/
	/* calculation and composition of datas in a report */
	total = very_big + big + normal + little;
	very_big = (int)( ((float)very_big / (float)total) *100);
	big = (int)( ((float)big / (float)total) *100);
	normal = (int)( ((float)normal / (float)total) *100);
	little = (int)( ((float)little / (float)total) *100);

	fprintf (script, "\"Emergency\"\t.%i\n", very_big);
	fprintf (script, "\"Critical\"\t.%i\n", big);
	fprintf (script, "\"Error\"\t.%i\n", normal);
	fprintf (script, "\"Warning\"\t.%i\n", little);
	
	free (c2);
	fclose (report);
	fclose (script);
	free (command_1);
	free (command_2);
	
	command_3 = g_strdup_printf ("ploticus -dir %s -svg %s -o %s", frame_get_absolute_tmp_path (f, ""), frame_get_absolute_data_path (f, "script_priority"), frame_get_absolute_tmp_path(f, "img.svg"));
	system (command_3);
	
	g_free (command_3);
	refresh_content (f);

	return 1;
}
