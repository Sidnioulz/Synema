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
 * \file position.c
 * \brief Fake Snort plugin written by the twentieth group
 * \author Vincent Jaouen <vincent.jaouen@ensi-bourges.fr>
 * \author Bénédicte Augustin <benedicte.augustin@ensi-bourges.com>
 *
 * This file contains a fake Snort plugin for demonstration purpose.
 */

#include "position.h"
#include "file_manip.h"

map_position position (int name, int continent)
{
    map_position pos;
    
//*******************WORLD
if (continent == WORLD) {

    if(name == EUROPE)
    {
        pos.x = 16;
        pos.y = 59;
    }
    if(name == NORTH_AMERICA)
    {
        pos.x = 16;
        pos.y = 27;
    }
    if(name == SOUTH_AMERICA)
    {
        pos.x = 31;
        pos.y = 40;
    }
    if(name == ASIA)
    {
        pos.x = 15;
        pos.y = 79;
    }
    if(name == AFRICA)
    {
        pos.x = 26;
        pos.y = 61;
    }
    if(name == OCEANIA)
    {
        pos.x = 32;
        pos.y = 91;
    }
    if(name == MIDDLE_EAST)
    {
        pos.x = 21;
        pos.y = 67;
    }
}// END WORLD**************** 

//********************OCEANIA
else if (continent == OCEANIA) {

    if(name == AUSTRALIA)
    {
        pos.x = 20;
        pos.y = 22;
    }
    if(name == INDONESIA)
    {
        pos.x = 1;
        pos.y = 8;
    }
    if(name == NEW_GUINEA)
    {
        pos.x = 5;
        pos.y = 36;
    }
    if(name == NEW_ZEALAND)
    {
        pos.x = 32;
        pos.y = 52;
    }
}// END OCEANIA*********************

//****************************EUROPE
else if (continent == EUROPE) {

    if(name == PORTUGAL)
    {
        pos.x = 37;
        pos.y = 12;
    }
    if(name == ICELAND)
    {
        pos.x = 6;
        pos.y = 9;
    }
    if(name == IRELAND)
    {
        pos.x = 11;
        pos.y = 19;
    }
    if(name == UNITED_KINGDOM)
    {
        pos.x = 22;
        pos.y = 29;
    }
    if(name == SPAIN)
    {
        pos.x = 37;
        pos.y = 23;
    }
    if(name == FRANCE)
    {
        pos.x = 29;
        pos.y = 37;
    }
    if(name == BELGIUM)
    {
        pos.x = 24;
        pos.y = 41;
    }
    if(name == NETHERLAND)
    {
        pos.x = 22;
        pos.y = 42;
    }
    if(name == SWITZERLAND)
    {
        pos.x = 28;
        pos.y = 49;
    }
    if(name == ITALY)
    {
        pos.x = 32;
        pos.y = 60;
    }
    if(name == GERMANY)
    {
        pos.x = 24;
        pos.y = 51;
    }
    if(name == DENMARK)
    {
        pos.x = 17;
        pos.y = 49;
    }
    if(name == AUSTRIA)
    {
        pos.x = 26;
        pos.y = 62;
    }
    if(name == CZECH_REPUBLIC)
    {
        pos.x = 24;
        pos.y = 62;
    }
    if(name == POLAND)
    {
        pos.x = 21;
        pos.y = 68;
    }
    if(name == HUNGARY)
    {
        pos.x = 26;
        pos.y = 71;
    }
    if(name == BOSNIA)
    {
        pos.x = 30;
        pos.y = 69;
    }
    if(name == SERBIA)
    {
        pos.x = 30;
        pos.y = 75;
    }
    if(name == GREECE)
    {
        pos.x = 35;
        pos.y = 81;
    }
    if(name == BULGARIA)
    {
        pos.x = 30;
        pos.y = 83;
    }
    if(name == ROUMANIA)
    {
        pos.x = 27;
        pos.y = 82;
    }
    if(name == UKRAINE)
    {
        pos.x = 21;
        pos.y = 89;
    }
    if(name == BELARUS)
    {
        pos.x = 17;
        pos.y = 80;
    }
    if(name == LITHUANIA)
    {
        pos.x = 16;
        pos.y = 71;
    }
    if(name == LATVIA)
    {
        pos.x = 14;
        pos.y = 72;
    }
    if(name == ESTONIA)
    {
        pos.x = 12;
        pos.y = 71;
    }
    if(name == FINLAND)
    {
        pos.x = 6;
        pos.y = 67;
    }
    if(name == SWEDEN)
    {
        pos.x = 10;
        pos.y = 54;
    }
    if(name == NORWAY)
    {
        pos.x = 11;
        pos.y = 43;
    }
} // END EUROPE*********************

//SOUTH_AMERICA************************
else if(continent == SOUTH_AMERICA) {

    if(name == BRAZIL)
    {
        pos.x = 14;
        pos.y = 30;
    }
    if(name == URUGUAY)
    {
        pos.x = 26;
        pos.y = 29;
    }
    if(name == ARGENTINA)
    {
        pos.x = 7;
        pos.y = 21;
    }
    if(name == CHILE)
    {
        pos.x = 8;
        pos.y = 17;
    }
    if(name == PERU)
    {
        pos.x = 12;
        pos.y = 11;
    }
    if(name == BOLIVIA)
    {
        pos.x = 16;
        pos.y = 20;
    }
    if(name == PARAGUAY)
    {
        pos.x = 20;
        pos.y = 27;
    }
    if(name == ECUADOR)
    {
        pos.x = 6;
        pos.y = 7;
    }
    if(name == COLOMBIA)
    {
        pos.x = 3;
        pos.y = 11;
    }
    if(name == VENEZUELA)
    {
        pos.x = 1;
        pos.y = 9;
    }
} // END SOUTH_AMERICA**********************

// NORTH_AMERICA****************************
else if (continent == NORTH_AMERICA) {

    if(name == ALASKA)
    {
        pos.x = 7;
        pos.y = 20;
    }
    if(name == CANADA)
    {
        pos.x = 12;
        pos.y = 33;
    }
    if(name == USA)
    {
        pos.x = 22;
        pos.y = 33;
    }
    if(name == MEXICO)
    {
        pos.x = 31;
        pos.y = 31;
    }
    if(name == GREENLAND)
    {
        pos.x = 2;
        pos.y = 89;
    }
    if(name == CENTRAL_AMERICA)
    {
        pos.x = 8;
        pos.y = 41;
    }
} // END NORTH_AMERICA*********************

// AFRICA**********************************
else if (continent == AFRICA) {

    if(name == MAROCO)
    {
        pos.x = 1;
        pos.y = 19;
    }
    if(name == ALGERIA)
    {
        pos.x = 2;
        pos.y = 32;
    }
    if(name == LIBYA)
    {
        pos.x = 3;
        pos.y = 60;
    }
    if(name == EGYPT)
    {
        pos.x = 4;
        pos.y = 77;
    }
    if(name == TUNISIA)
    {
        pos.x = 90;
        pos.y = 90;
    }
    if(name == WESTERN_SAHARA)
    {
        pos.x = 4;
        pos.y = 10;
    }
    if(name == MAURITANIA)
    {
        pos.x = 7;
        pos.y = 15;
    }
    if(name == MALI)
    {
        pos.x = 9;
        pos.y = 28;
    }
    if(name == NIGER)
    {
        pos.x = 9;
        pos.y = 42;
    }
    if(name == CHAD)
    {
        pos.x = 10;
        pos.y = 60;
    }
    if(name == SUDAN)
    {
        pos.x = 9;
        pos.y = 80;
    }
    if(name == EPITREA)
    {
        pos.x = 10;
        pos.y = 93;
    }
    if(name == SENEGAL)
    {
        pos.x = 10;
        pos.y = 8;
    }
    if(name == GUINEA)
    {
        pos.x = 13;
        pos.y = 12;
    }
    if(name == SIERRA_LEONE)
    {
        pos.x = 14;
        pos.y = 11;
    }
    if(name == LIBERIA)
    {
        pos.x = 16;
        pos.y = 18;
    }
    if(name == IVORY_COAST)
    {
        pos.x = 15;
        pos.y = 21;
    }
    if(name == BURKINA_FASO)
    {
        pos.x = 12;
        pos.y = 29;
    }
    if(name == GHANA)
    {
        pos.x = 15;
        pos.y = 29;
    }
    if(name == NIGERIA)
    {
        pos.x = 14;
        pos.y = 41;
    }
    if(name == ETHIOPIA)
    {
        pos.x = 15;
        pos.y = 95;
    }
    if(name == CAMEROUN)
    {
        pos.x = 17;
        pos.y = 50;
    }
    if(name == CENTRAL_AFRICAN_REPUBLIC)
    {
        pos.x = 16;
        pos.y = 65;
    }
    if(name == GABON)
    {
        pos.x = 20;
        pos.y = 48;
    }
    if(name == CONGO)
    {
        pos.x = 20;
        pos.y = 53;
    }
    if(name == REPUBLIC_OF_CONGO)
    {
        pos.x = 21;
        pos.y = 69;
    }
    if(name == UGANDA)
    {
        pos.x = 18;
        pos.y = 88;
    }
    if(name == KENYA)
    {
        pos.x = 20;
        pos.y = 91;
    }
    if(name == SOMALIA)
    {
        pos.x = 17;
        pos.y = 115;
    }
    if(name == ANGOLA)
    {
        pos.x = 28;
        pos.y = 59;
    }
    if(name == TANZANIA)
    {
        pos.x = 24;
        pos.y = 90;
    }
    if(name == ZAMBIA)
    {
        pos.x = 28;
        pos.y = 70;
    }
    if(name == MOZAMBIC)
    {
        pos.x = 29;
        pos.y = 93;
    }
    if(name == ZIMBABWE)
    {
        pos.x = 32;
        pos.y = 79;
    }
    if(name == NAMIBIA)
    {
        pos.x = 33;
        pos.y = 58;
    }
    if(name == BOTSWANA)
    {
        pos.x = 33;
        pos.y = 69;
    }
    if(name == SOUTH_AFRICA)
    {
        pos.x = 38;
        pos.y = 65;
    }
    if(name == REPUBLIC_OF_MADAGASCAR)
    {
        pos.x = 33;
        pos.y = 112;
    }
}// END AFRICA***************************

// ASIA**********************************
else if (continent == ASIA) {

    if(name == INDIA)
    {
        pos.x = 24;
        pos.y = 50;
    }
    if(name == CHINA)
    {
        pos.x = 19;
        pos.y = 62;
    }
    if(name == RUSSIA)
    {
        pos.x = 6;
        pos.y = 57;
    }
    if(name == KAZAKHSTAN)
    {
        pos.x = 12;
        pos.y = 38;
    }
    if(name == MONGOLIA)
    {
        pos.x = 12;
        pos.y = 65;
    }
    if(name == JAPAN)
    {
        pos.x = 17;
        pos.y = 89;
    }
    if(name == PAKISTAN)
    {
        pos.x = 21;
        pos.y = 45;
    }
    if(name == AFGHANISTAN)
    {
        pos.x = 19;
        pos.y = 44;
    }
    if(name == THAILAND)
    {
        pos.x = 28;
        pos.y = 68;
    }
    if(name == KOREA)
    {
        pos.x = 17;
        pos.y = 81;
    }
    if(name == MYANMAR)
    {
        pos.x = 25;
        pos.y = 65;
    }
    if(name == TURKMENISTAN)
    {
        pos.x = 16;
        pos.y = 21;
    }
    if(name == TURKEY)
    {
        pos.x = 16;
        pos.y = 8;
    }
    if(name == SYRIA)
    {
        pos.x = 18;
        pos.y = 10;
    }
    if(name == IRAQ)
    {
        pos.x = 19;
        pos.y = 12;
    }
    if(name == SAUDI_ARABIA)
    {
        pos.x = 23;
        pos.y = 14;
    }
    if(name == YEMEN)
    {
        pos.x = 28;
        pos.y = 15;
    }
    if(name == OMAN)
    {
        pos.x = 25;
        pos.y = 21;
    }
    if(name == IRAN)
    {
        pos.x = 19;
        pos.y = 21;
    }
}
    return pos;
}
