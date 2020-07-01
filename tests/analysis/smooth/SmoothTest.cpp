/***************************************************************************
    File                 : SmoothTest.cpp
    Project              : LabPlot
    Description          : Tests for data smoothing
    --------------------------------------------------------------------
    Copyright            : (C) 2020 Stefan Gerlach (stefan.gerlach@uni.kn)
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

#include "SmoothTest.h"
#include "backend/core/column/Column.h"
#include "backend/worksheet/plots/cartesian/XYSmoothCurve.h"

extern "C" {
#include "backend/nsl/nsl_smooth.h"
}

//##############################################################################

void SmoothTest::testPercentile() {
	// data
	QVector<int> xData{1,2,3,4,5,6,7,8,9,10};
	QVector<double> yData{47.7,44.,43.,44.96,45.,43.73,38.,47.1,44.,38.3};
	// p=0.5, d=5	(from LibreOffice)
	QVector<double> result50_5{47.7,44,44.96,44,43.73,44.96,44,43.73,44,38.3};

	//data source columns
	Column xDataColumn("x", AbstractColumn::ColumnMode::Integer);
	xDataColumn.replaceInteger(0, xData);

	Column yDataColumn("y", AbstractColumn::ColumnMode::Numeric);
	yDataColumn.replaceValues(0, yData);

	XYSmoothCurve smoothCurve("smooth");
	smoothCurve.setXDataColumn(&xDataColumn);
	smoothCurve.setYDataColumn(&yDataColumn);

	//prepare the smooth
	XYSmoothCurve::SmoothData smoothData = smoothCurve.smoothData();
	smoothData.type = nsl_smooth_type_percentile;
	// default
	//smoothData.points = 5;
	//smoothData.percentile = 0.5;
	smoothCurve.setSmoothData(smoothData);

	//perform the smooth
	smoothCurve.recalculate();
	const XYSmoothCurve::SmoothResult& smoothResult = smoothCurve.smoothResult();

	//check the results
	QCOMPARE(smoothResult.available, true);
	QCOMPARE(smoothResult.valid, true);

	const auto* resultXDataColumn{smoothCurve.xColumn()};
	const auto* resultYDataColumn{smoothCurve.yColumn()};

	const int np{resultXDataColumn->rowCount()};
	QCOMPARE(np, 10);

	for (int i = 0; i < np; i++) {
		QCOMPARE(resultXDataColumn->valueAt(i), (double)i + 1);
		QCOMPARE(resultYDataColumn->valueAt(i), result50_5.at(i));
	}
}

QTEST_MAIN(SmoothTest)
