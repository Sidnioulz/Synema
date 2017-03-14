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
 * \file file_manip.h
 * \brief Fake Snort plugin written by the twentieth group
 * \author Vincent Jaouen <vincent.jaouen@ensi-bourges.fr>
 * \author Bénédicte Augustin <benedicte.augustin@ensi-bourges.com>
 *
 * This file contains a fake Snort plugin for demonstration purpose.
 */

#define TYPE_ATTACKS 0
#define TYPE_GEOLOCALISATION 1
#include "frame-utilities.h"

enum {
    WORLD = 0,
    AFRICA,
    ASIA,
    EUROPE,
    MIDDLE_EAST,
    NORTH_AMERICA,
    OCEANIA,
    SOUTH_AMERICA
} continents;


enum {
    AFGHANISTAN = 0,
    CHINA,
    INDIA,
    JAPAN,
    KAZAKHSTAN,
    KOREA,
    MONGOLIA,
    BIRMANIA,
    PAKISTAN,
    RUSSIA,
    THAILAND,
    TURKMENISTAN,
    TURKEY,
    SYRIA,
    IRAQ,
    SAUDI_ARABIA,
    YEMEN,
    OMAN,
    MYANMAR,
    IRAN
} asia_countries;


enum {
    ARGENTINA = 0,
    BOLIVIA,
    BRAZIL,
    CHILE,
    PERU,
    COLOMBIA,
    ECUADOR,
    PARAGUAY,
    URUGUAY,
    VENEZUELA
} south_america_countries;


enum {
    AUSTRALIA = 0,
    NEW_ZEALAND,
    NEW_GUINEA,
    INDONESIA
} oceania_countries;


enum {
    ALASKA = 0,
    CANADA,
    USA,
    MEXICO,
    GREENLAND,
    CENTRAL_AMERICA
} north_america_countries;


enum {
    PORTUGAL = 0,
    SPAIN,
    FRANCE,
    ITALY,
    GERMANY,
    UNITED_KINGDOM,
    IRELAND,
    ICELAND,
    NETHERLAND,
    SWEDEN,
    NORWAY,
    FINLAND,
    AUSTRIA,
    CZECH_REPUBLIC,
    SLOVANIA,
    HUNGARY,
    POLAND,
    DENMARK,
    BULGARIA,
    GREECE,
    ROMANIA,
    UKRAINE,
    ROUMANIA,
    SERBIA,
    LATVIA,
    ESTONIA,
    BOSNIA,
    LITHUANIA,
    BELARUS,
    BELGIUM,
    SWITZERLAND
} europe_countries;


enum {
    MAROCO = 0,
    ALGERIA,
    LIBYA,
    EGYPT,
    TUNISIA,
    WESTERN_SAHARA,
    MAURITANIA,
    MALI,
    NIGER,
    EPITREA,
    CHAD,
    SUDAN,
    ETHIOPIA,
    SENEGAL,
    GUINEA,
    SIERRA_LEONE,
    LIBERIA,
    IVORY_COAST,
    BURKINA_FASO,
    GHANA,
    NIGERIA,
    ERITREA,
    CAMEROUN,
    CENTRAL_AFRICAN_REPUBLIC,
    GABON,
    CONGO,
    REPUBLIC_OF_CONGO,
    UGANDA,
    KENYA,
    SOMALIA,
    ANGOLA,
    TANZANIA,
    ZAMBIA,
    MOZAMBIC,
    ZIMBABWE,
    NAMIBIA,
    BOTSWANA,
    SOUTH_AFRICA,
    REPUBLIC_OF_MADAGASCAR
} africa_countries;


typedef struct __data_affichage {
    int type;
    int continent;
    int attack_type;
    char * name_background_file;
    char * name_foreground_file;
    char * name_report_file;
    int frame_size;
} data_affichage;


char * get_report_name (frame_t *);
int lines_nb_file (char *);
int * read_report (int, int, char*);
int create_ploticus_image (gpointer);
