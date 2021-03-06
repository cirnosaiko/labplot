/***************************************************************************
    File                 : CorrelationTest.h
    Project              : LabPlot
    Description          : Tests for data correlation
    --------------------------------------------------------------------
    Copyright            : (C) 2018 Stefan Gerlach (stefan.gerlach@uni.kn)
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
#ifndef CORRELATIONTEST_H
#define CORRELATIONTEST_H

#include <../AnalysisTest.h>

class CorrelationTest : public AnalysisTest {
	Q_OBJECT

private slots:
	// linear tests
	void testLinear();
	void testLinear2();
	void testLinear_noX();
	void testLinear_swapped();

	// circular tests
	void testCircular();
	void testCircular2();

	// norm
	void testLinear_biased();
	void testLinear2_biased();
	void testLinear_unbiased();
	void testLinear2_unbiased();
	void testLinear_coeff();
	void testLinear2_coeff();
	void testCircular_coeff();
	void testCircular2_coeff();

	// sampling interval
	void testLinear_samplingInterval();
	void testLinear2_samplingInterval();
	void testCircular_samplingInterval();
	void testCircular2_samplingInterval();

	void testPerformance();
};
#endif
