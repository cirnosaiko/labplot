/***************************************************************************
    File                 : nsl_math.h
    Project              : LabPlot
    Description          : NSL math functions
    --------------------------------------------------------------------
    Copyright            : (C) 2018-2020 by Stefan Gerlach (stefan.gerlach@uni.kn)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#ifndef NSL_MATH_H
#define NSL_MATH_H

#define M_PI_180 (M_PI/180.)
#define M_180_PI (180./M_PI)

/* returns decimal places of signed value
* 0.1 -> 1, 0.06 -> 2, 23 -> -1, 100 -> -2
*/
int nsl_math_decimal_places(double value);

/* return decimal places of signed value rounded to one digit
* 0.1 -> 1, 0.006 -> 2, 0.8 -> 0, 12 -> -1, 520 -> -3
*/
int nsl_math_rounded_decimals(double value);

/* nsl_math_rounded_decimals() but max 'max'
 */
int nsl_math_rounded_decimals_max(double value, int max);

/* round double value to n decimal places */
double nsl_math_round_places(double value, unsigned int n); 

/* round double value to precision p */
double nsl_math_round_precision(double value, unsigned int p);

#endif /* NSL_MATH_H */
