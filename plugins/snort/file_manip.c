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
 * \file file_manip.c
 * \brief Fake Snort plugin written by the twentieth group
 * \author Vincent Jaouen <vincent.jaouen@ensi-bourges.fr>
 * \author Bénédicte Augustin <benedicte.augustin@ensi-bourges.com>
 *
 * This file contains a fake Snort plugin for demonstration purpose.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_manip.h"
#include "position.h"
#include "frame-utilities.h"



char * get_report_name (frame_t * f)
{
	data_affichage *data = (data_affichage *) f->priv;

    char * name_report;
    int type = f->display_period.type;
    int value = f->display_period.value;


    if(data->type == TYPE_GEOLOCALISATION) {

        if (type == WEEK && value == 2)
            name_report = strdup("rapportXIV");
    
        else if (type == DAY && value == 1)
            name_report = strdup("rapport1DAY");

        else if (type == HOUR && value == 2)
            name_report = strdup("rapport2H");

        else if (type == LIVE)
            name_report = strdup("rapportlive");

        else name_report = strdup("rapportXIV");   
    }

    else if (data->type == TYPE_ATTACKS) {

        if (type == WEEK && value == 2)
            name_report = strdup("rapport_a_XIV");
    
        else if (type == DAY && value == 1)
            name_report = strdup("rapport_a_1day");

        else if (type == HOUR && value == 2)
            name_report = strdup("rapport_a_2hours");

        else if (type == LIVE)
            name_report = strdup("rapport_a_live");

        else name_report = strdup("rapport_a_XIV");   
    }

    return frame_get_absolute_reports_path(f, name_report, NULL);
}
        



int lines_nb_file (char * name_file)
{
    FILE * var_file = fopen(name_file, "r");

    if(name_file == NULL) {
        printf("the program can't open the wanted file %s\n", name_file);
        return 0;
    }    

    int n=0, carac;
    carac=fgetc(var_file);
    while(carac!=EOF)
    {
        carac=fgetc(var_file);
        if(carac=='\n') n++;
    }
    fclose(var_file);
    return n;
}



int * read_report (int continent, int countries_number, char * name_report)
{
    FILE * report = fopen(name_report, "r");

    if (report == NULL) {
        printf("the program can't open the wanted file %s\n", name_report);
        return 0;
    }    
    
    int i, lines_nb = lines_nb_file(name_report), attacks_nb = 0, continent_attacks;
    float attacks_percent = 0, continent_percent = 0;

    int * countries_percents_tab = (int *) calloc (countries_number, sizeof(int)); // table declaration, which memorize values

    char * continent_name = (char *) calloc (40, sizeof(char));
    char * country_name = (char *) calloc (40, sizeof(char));
    
    for (i=0 ; i<lines_nb ; i++) {
        if(i<=4)
            fscanf(report, "%s %d %f", continent_name, &continent_attacks, &continent_percent);
        else
            fscanf(report, "%s %f %s %d %f", continent_name, &continent_percent, country_name, &attacks_nb, &attacks_percent);

        if (continent == WORLD) {

            if(strcmp("Europe", continent_name)==0)
                countries_percents_tab[EUROPE] = continent_percent;

            else if(strcmp("Asia", continent_name)==0)
                countries_percents_tab[ASIA] = continent_percent;

            else if(strcmp("Oceania", continent_name)==0)
                countries_percents_tab[OCEANIA] = continent_percent;

            else if(strcmp("Africa", continent_name)==0)
                countries_percents_tab[AFRICA] = continent_percent;

            else if(strcmp("North_America", continent_name)==0)
                countries_percents_tab[NORTH_AMERICA] = continent_percent;

            else if(strcmp("South_America", continent_name)==0)
                countries_percents_tab[SOUTH_AMERICA] = continent_percent;

            else if(strcmp("Middle_East", continent_name)==0)
                countries_percents_tab[MIDDLE_EAST] = continent_percent;
        }
    
        else if (continent == ASIA) {

            if(strcmp("Afghanistan", country_name)==0)
                countries_percents_tab[AFGHANISTAN] = attacks_percent;

            else if(strcmp("China", country_name)==0)
                countries_percents_tab[CHINA] = attacks_percent;

            else if(strcmp("India", country_name)==0)
                countries_percents_tab[INDIA] = attacks_percent;

            else if(strcmp("Japan", country_name)==0)
                countries_percents_tab[JAPAN] = attacks_percent;

            else if(strcmp("Kazakhstan", country_name)==0)
                countries_percents_tab[KAZAKHSTAN] = attacks_percent;

            else if(strcmp("Korea", country_name)==0)
                countries_percents_tab[KOREA] = attacks_percent;

            else if(strcmp("Mongolia", country_name)==0)
                countries_percents_tab[MONGOLIA] = attacks_percent;

            else if(strcmp("Birmania", country_name)==0)
                countries_percents_tab[BIRMANIA] = attacks_percent;

            else if(strcmp("Pakistan", country_name)==0)
                countries_percents_tab[PAKISTAN] = attacks_percent;

            else if(strcmp("Russia", country_name)==0)
                countries_percents_tab[RUSSIA] = attacks_percent;

            else if(strcmp("Thailand", country_name)==0)
                countries_percents_tab[THAILAND] = attacks_percent;

            else if(strcmp("Turkmenistan", country_name)==0)
                countries_percents_tab[TURKMENISTAN] = attacks_percent;

            else if(strcmp("Turkey", country_name)==0)
                countries_percents_tab[TURKEY] = attacks_percent;

            else if(strcmp("Syria", country_name)==0)
                countries_percents_tab[SYRIA] = attacks_percent;

            else if(strcmp("Saudi_Arabia", country_name)==0)
                countries_percents_tab[SAUDI_ARABIA] = attacks_percent;

            else if(strcmp("Yemen", country_name)==0)
                countries_percents_tab[YEMEN] = attacks_percent;

            else if(strcmp("Oman", country_name)==0)
                countries_percents_tab[OMAN] = attacks_percent;

            else if(strcmp("Iran", country_name)==0)
                countries_percents_tab[IRAN] = attacks_percent;

            else if(strcmp("Iraq", country_name)==0)
                countries_percents_tab[IRAQ] = attacks_percent;
        }
        
        else if (continent == SOUTH_AMERICA) {
            
            if(strcmp("Brazil", country_name)==0)
                countries_percents_tab[BRAZIL] = attacks_percent;

            else if(strcmp("Uruguay", country_name)==0)
                countries_percents_tab[URUGUAY] = attacks_percent;

            else if(strcmp("Argentina", country_name)==0)
                countries_percents_tab[ARGENTINA] = attacks_percent;

            else if(strcmp("Chile", country_name)==0)
                countries_percents_tab[CHILE] = attacks_percent;

            else if(strcmp("Peru", country_name)==0)
                countries_percents_tab[PERU] = attacks_percent;

            else if(strcmp("Bolivia", country_name)==0)
                countries_percents_tab[BOLIVIA] = attacks_percent;

            else if(strcmp("Paraguay", country_name)==0)
                countries_percents_tab[PARAGUAY] = attacks_percent;

            else if(strcmp("Ecuador", country_name)==0)
                countries_percents_tab[ECUADOR] = attacks_percent;

            else if(strcmp("Colombia", country_name)==0)
                countries_percents_tab[COLOMBIA] = attacks_percent;

            else if(strcmp("Venezuela", country_name)==0)
                countries_percents_tab[VENEZUELA] = attacks_percent;
        }

        else if (continent == EUROPE) {

            if(strcmp("Portugal", country_name)==0)
                countries_percents_tab[PORTUGAL] = attacks_percent;

            else if(strcmp("Spain", country_name)==0)
                countries_percents_tab[SPAIN] = attacks_percent;

            else if(strcmp("France", country_name)==0)
                countries_percents_tab[FRANCE] = attacks_percent;

            else if(strcmp("Italy", country_name)==0)
                countries_percents_tab[ITALY] = attacks_percent;

            else if(strcmp("Germany", country_name)==0)
                countries_percents_tab[GERMANY] = attacks_percent;

            else if(strcmp("United_Kingdom", country_name)==0)
                countries_percents_tab[UNITED_KINGDOM] = attacks_percent;

            else if(strcmp("Ireland", country_name)==0)
                countries_percents_tab[IRELAND] = attacks_percent;

            else if(strcmp("Iceland", country_name)==0)
                countries_percents_tab[ICELAND] = attacks_percent;

            else if(strcmp("Bosnia", country_name)==0)
                countries_percents_tab[BOSNIA] = attacks_percent;

            else if(strcmp("Netherland", country_name)==0)
                countries_percents_tab[NETHERLAND] = attacks_percent;

            else if(strcmp("Sweden", country_name)==0)
                countries_percents_tab[SWEDEN] = attacks_percent;

            else if(strcmp("Norway", country_name)==0)
                countries_percents_tab[NORWAY] = attacks_percent;

            else if(strcmp("Finland", country_name)==0)
                countries_percents_tab[FINLAND] = attacks_percent;

            else if(strcmp("Austria", country_name)==0)
                countries_percents_tab[AUSTRIA] = attacks_percent;

            else if(strcmp("Czech_Republic", country_name)==0)
                countries_percents_tab[CZECH_REPUBLIC] = attacks_percent;

            else if(strcmp("Slovania", country_name)==0)
                countries_percents_tab[SLOVANIA] = attacks_percent;

            else if(strcmp("Hungary", country_name)==0)
                countries_percents_tab[HUNGARY] = attacks_percent;

            else if(strcmp("Poland", country_name)==0)
                countries_percents_tab[POLAND] = attacks_percent;

            else if(strcmp("Denmark", country_name)==0)
                countries_percents_tab[DENMARK] = attacks_percent;

            else if(strcmp("Bulgaria", country_name)==0)
                countries_percents_tab[BULGARIA] = attacks_percent;

            else if(strcmp("Greece", country_name)==0)
                countries_percents_tab[GREECE] = attacks_percent;

            else if(strcmp("Romania", country_name)==0)
                countries_percents_tab[ROMANIA] = attacks_percent;

            else if(strcmp("Ukraine", country_name)==0)
                countries_percents_tab[UKRAINE] = attacks_percent;

            else if(strcmp("Latvia", country_name)==0)
                countries_percents_tab[LATVIA] = attacks_percent;

            else if(strcmp("Estonia", country_name)==0)
                countries_percents_tab[ESTONIA] = attacks_percent;

            else if(strcmp("Lithuania", country_name)==0)
                countries_percents_tab[LITHUANIA] = attacks_percent;

            else if(strcmp("Belarus", country_name)==0)
                countries_percents_tab[BELARUS] = attacks_percent;

            else if(strcmp("Belgium", country_name)==0)
                countries_percents_tab[BELGIUM] = attacks_percent;

            else if(strcmp("Switzerland", country_name)==0)
                countries_percents_tab[SWITZERLAND] = attacks_percent;
        }
        else if(continent == NORTH_AMERICA) {
            
            if(strcmp("Alaska", country_name)==0)
                countries_percents_tab[ALASKA] = attacks_percent;

            else if(strcmp("Canada", country_name)==0)
                countries_percents_tab[CANADA] = attacks_percent;

            else if(strcmp("USA", country_name)==0)
                countries_percents_tab[USA] = attacks_percent;

            else if(strcmp("Mexico", country_name)==0)
                countries_percents_tab[MEXICO] = attacks_percent;

            else if(strcmp("Greenland", country_name)==0)
                countries_percents_tab[GREENLAND] = attacks_percent;

            else if(strcmp("Central_America", country_name)==0)
                countries_percents_tab[CENTRAL_AMERICA] = attacks_percent;

        }
        else if (continent == OCEANIA) {

            if(strcmp("Australia", country_name)==0)
                countries_percents_tab[AUSTRALIA] = attacks_percent;

            else if(strcmp("Indonesia", country_name)==0)
                countries_percents_tab[INDONESIA] = attacks_percent;

            else if(strcmp("New_Guinea", country_name)==0)
                countries_percents_tab[NEW_GUINEA] = attacks_percent;

            else if(strcmp("New_Zealand", country_name)==0)
                countries_percents_tab[NEW_ZEALAND] = attacks_percent;

        }
        else if (continent == AFRICA) {
            if(strcmp("Maroco", country_name)==0)
                countries_percents_tab[MAROCO] = attacks_percent;

            else if(strcmp("Algeria", country_name)==0)
                countries_percents_tab[ALGERIA] = attacks_percent;

            else if(strcmp("Libya", country_name)==0)
                countries_percents_tab[LIBYA] = attacks_percent;

            else if(strcmp("Egypt", country_name)==0)
                countries_percents_tab[EGYPT] = attacks_percent;

            else if(strcmp("Tunisia", country_name)==0)
                countries_percents_tab[TUNISIA] = attacks_percent;

            else if(strcmp("Western_Sahara", country_name)==0)
                countries_percents_tab[WESTERN_SAHARA] = attacks_percent;

            else if(strcmp("Mauritania", country_name)==0)
                countries_percents_tab[MAURITANIA] = attacks_percent;

            else if(strcmp("Mali", country_name)==0)
                countries_percents_tab[MALI] = attacks_percent;

            else if(strcmp("Niger", country_name)==0)
                countries_percents_tab[NIGER] = attacks_percent;

            else if(strcmp("Chad", country_name)==0)
                countries_percents_tab[CHAD] = attacks_percent;

            else if(strcmp("Sudan", country_name)==0)
                countries_percents_tab[SUDAN] = attacks_percent;

            else if(strcmp("Epitrea", country_name)==0)
                countries_percents_tab[EPITREA] = attacks_percent;

            else if(strcmp("Senegal", country_name)==0)
                countries_percents_tab[SENEGAL] = attacks_percent;

            else if(strcmp("Guinea", country_name)==0)
                countries_percents_tab[GUINEA] = attacks_percent;

            else if(strcmp("Sierra_Leone", country_name)==0)
                countries_percents_tab[SIERRA_LEONE] = attacks_percent;

            else if(strcmp("Liberia", country_name)==0)
                countries_percents_tab[LIBERIA] = attacks_percent;

            else if(strcmp("Ivory_Coast", country_name)==0)
                countries_percents_tab[IVORY_COAST] = attacks_percent;

            else if(strcmp("Burkina_Faso", country_name)==0)
                countries_percents_tab[BURKINA_FASO] = attacks_percent;

            else if(strcmp("Ghana", country_name)==0)
                countries_percents_tab[GHANA] = attacks_percent;

            else if(strcmp("Nigeria", country_name)==0)
                countries_percents_tab[NIGERIA] = attacks_percent;

            else if(strcmp("Ethiopia", country_name)==0)
                countries_percents_tab[ETHIOPIA] = attacks_percent;

            else if(strcmp("Cameroun", country_name)==0)
                countries_percents_tab[CAMEROUN] = attacks_percent;

            else if(strcmp("Central_African_Republic", country_name)==0)
                countries_percents_tab[CENTRAL_AFRICAN_REPUBLIC] = attacks_percent;

            else if(strcmp("Gabon", country_name)==0)
                countries_percents_tab[GABON] = attacks_percent;

            else if(strcmp("Congo", country_name)==0)
                countries_percents_tab[CONGO] = attacks_percent;

            else if(strcmp("Republic_Of_Congo", country_name)==0)
                countries_percents_tab[REPUBLIC_OF_CONGO] = attacks_percent;

            else if(strcmp("Uganda", country_name)==0)
                countries_percents_tab[UGANDA] = attacks_percent;

            else if(strcmp("Kenya", country_name)==0)
                countries_percents_tab[KENYA] = attacks_percent;

            else if(strcmp("Somalia", country_name)==0)
                countries_percents_tab[SOMALIA] = attacks_percent;

            else if(strcmp("Angola", country_name)==0)
                countries_percents_tab[ANGOLA] = attacks_percent;

            else if(strcmp("Tanzania", country_name)==0)
                countries_percents_tab[TANZANIA] = attacks_percent;

            else if(strcmp("Zambia", country_name)==0)
                countries_percents_tab[ZAMBIA] = attacks_percent;

            else if(strcmp("Mozambic", country_name)==0)
                countries_percents_tab[MOZAMBIC] = attacks_percent;

            else if(strcmp("Zimbabwe", country_name)==0)
                countries_percents_tab[ZIMBABWE] = attacks_percent;

            else if(strcmp("Namibia", country_name)==0)
                countries_percents_tab[NAMIBIA] = attacks_percent;

            else if(strcmp("Botswana", country_name)==0)
                countries_percents_tab[BOTSWANA] = attacks_percent;

            else if(strcmp("South_Africa", country_name)==0)
                countries_percents_tab[SOUTH_AFRICA] = attacks_percent;

            else if(strcmp("Republic_Of_Madagascar", country_name)==0)
                countries_percents_tab[REPUBLIC_OF_MADAGASCAR] = attacks_percent;

        }
    } // the end of this loop

    fclose(report);

    return countries_percents_tab;
}





int create_ploticus_image (gpointer user_data)
{
	frame_t *f = (frame_t *) user_data;
	data_affichage * data = (data_affichage *) f->priv;
    
	data->name_report_file = get_report_name (f);

	char * data_report_path = strdup (frame_get_absolute_tmp_path(f, "data_report"));

    FILE * data_report = fopen(data_report_path, "w");

    if(data_report == NULL) {
        data->name_foreground_file = frame_get_absolute_data_path (f, "white.png");
        printf("the program can't open the wanted file %s\n", data_report_path);
        return 0;
    }

    fprintf(data_report, "x y\n");

    FILE * report = fopen(data->name_report_file, "r");

    if (report == NULL) {
        data->name_foreground_file = frame_get_absolute_data_path (f, "white.png");
        printf("the program can't open the wanted file %s\n", data->name_report_file);
        return 0;
    }

    char * command = g_strdup_printf ("ploticus -dir %s -png %s -o %s", frame_get_absolute_tmp_path (f, ""), frame_get_absolute_data_path (f, "script_monde"), data->name_foreground_file);

    int countries_number = 0, i = 0, u = 0;

    switch (data->continent){
        case WORLD:
            countries_number = 8;
            break;
        case ASIA:
            countries_number = 21;
            break;
        case NORTH_AMERICA:
            countries_number = 5;
            break;
        case SOUTH_AMERICA:
            countries_number = 9;
            break;
        case EUROPE:
            countries_number = 40;
            break;
        case OCEANIA:
            countries_number = 3;
            break;
        case MIDDLE_EAST:
            countries_number = 8;
            break;
        case AFRICA:
            countries_number = 32;
            break;
        default :
            break;
    }

    int * countries_percents_tab = read_report (data->continent, countries_number, data->name_report_file);

    map_position posit;

    for (i=0 ; i < countries_number ; i++) {                  // the program enter in the loop for each countries
        posit = position (i, data->continent);                // it returns the position of the country
        for (u=0 ; u < countries_percents_tab[i] ; u++)       // it writes in a file the position as often as percents
               fprintf(data_report, "%i %i\n", posit.x, posit.y);
    }

    fclose(data_report);
    fclose(report);

    system(command);

    free(data_report_path);
    free(countries_percents_tab);
    g_free(command);

    return 1;
}
