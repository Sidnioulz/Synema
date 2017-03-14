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
 * \file types_of_attacks.c
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

int script_attacks (gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_affichage * data = (data_affichage *) f->priv;

	data->type = TYPE_ATTACKS;
	data->attack_type = ATTACKS;

	FILE * report;
	FILE * script;
	char * command_1, * command_2, * command_3;
	int nb = 0;
	ssize_t r;
	char * c1 = calloc (1000, sizeof(char));
	char * c2 = calloc (1000, sizeof(char));

	int tab[10] = {0};
	int errno = 0;
	size_t n;
	
	data->name_foreground_file = frame_get_absolute_tmp_path (f, "img.svg");
	command_1 = get_report_name(f);
	command_2 = frame_get_absolute_tmp_path (f, "attacks");
	
	report = fopen (command_1, "r");
	
	if (report == NULL) {
//		data->name_foreground_file = frame_get_absolute_data_path (f, "white.png");
		printf("the program can't open the wanted file %s\n", command_1);
		return 0;
	}

	script = fopen (command_2, "w+");
	
	if (script == NULL) {
//		data->name_foreground_file = frame_get_absolute_data_path (f, "white.png");
		printf("the program can't open the wanted file %s\n", command_2);
		return 0;
	}
	
	/* reading of datas */
	
	r = getline(&c1, &n, report);
	
	while (r > 0)
	{
		sscanf(c1, "%s", c2);
		if (c2 != NULL && strcasecmp (c2, "Attack:") == 0)
		{
			sscanf (c1, "%s%i", c2, &nb);
			switch (nb) {
				case 1:
					tab[0] ++;
					break;
				case 2:
					tab[1] ++;
					break;
				case 3:
					tab[2] ++;
					break;

				case 4:
					tab[3] ++;
					break;

				case 5:
					tab[4] ++;
					break;

				case 6:
					tab[5] ++;
					break;

				case 7:
					tab[6] ++;
					break;

				case 8:
					tab[7] ++;
					break;

				case 9:
					tab[8] ++;
					break;

				case 10:
					tab[9] ++;
					break;

				default:
					printf ("there are others attacks\n");
					break;
			}
		}
		else if (c2 == NULL)
		{
			printf("error : c2 = NULL\n");
		}
		
		r = getline(&c1, &n, report);
	}
	free (c1);
	
/*	if (r == 0 || r < 0)
	{
		printf ("Error : %s\n", strerror(errno));
	}
*/	
	fprintf (script, "\"COMMUNITY SIP TCP/IP \" %i\n", tab[0]);
	fprintf (script, "\"IIS UNICODE CODEPOINT ENCODING\" %i\n", tab[1]); 
	fprintf (script, "\"DNS SPOOF \" %i\n", tab[7]);
	fprintf (script, "\"DOUBLE DECODING ATTACK\" %i\n", tab[2]);
	fprintf (script, "\"WEB-CGI calendar access\" %i\n", tab[3]);
	fprintf (script, "\"WEB-PHP shoutbox.php access\" %i\n", tab[4]);
	fprintf (script, "\"WEB-MISC weblogic/tomcat \" %i\n", tab[8]);
	fprintf (script, "\"COMMUNITY SIP DNS \" %i\n", tab[5]);
	fprintf (script, "\"WEB-CGI wrap access\" %i\n", tab[6]); 
	fprintf (script, "\"WEB-MISC Invalid HTTP \" %i\n", tab[9]);
	
	free (c2);
	fclose (report);
	fclose (script);
	free (command_1);
	free (command_2);	
	
	command_3 = g_strdup_printf ("ploticus -dir %s -svg %s -o %s", frame_get_absolute_tmp_path (f, ""), frame_get_absolute_data_path (f, "script_noun_of_attacks"), frame_get_absolute_tmp_path (f, "img.svg"));
	
	system (command_3);
	
	g_free (command_3);
	refresh_content (f);
    
	return 1;
}
