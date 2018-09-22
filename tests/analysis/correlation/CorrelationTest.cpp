/***************************************************************************
    File                 : CorrelationTest.cpp
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

#include "CorrelationTest.h"
#include "backend/core/column/Column.h"
#include "backend/worksheet/plots/cartesian/XYCorrelationCurve.h"

//##############################################################################

void CorrelationTest::testLinear() {
	// data
	QVector<int> xData = {1,2,3,4};
	QVector<double> yData = {1.,2.,3.,4.};
	QVector<double> y2Data = {0,1.,.5};

	//data source columns
	Column xDataColumn("x", AbstractColumn::Integer);
	xDataColumn.replaceInteger(0, xData);

	Column yDataColumn("y", AbstractColumn::Numeric);
	yDataColumn.replaceValues(0, yData);

	Column y2DataColumn("y2", AbstractColumn::Numeric);
	y2DataColumn.replaceValues(0, y2Data);

	XYCorrelationCurve correlationCurve("correlation");
	correlationCurve.setXDataColumn(&xDataColumn);
	correlationCurve.setYDataColumn(&yDataColumn);
	correlationCurve.setY2DataColumn(&y2DataColumn);

	//prepare the correlation
	XYCorrelationCurve::CorrelationData correlationData = correlationCurve.correlationData();
	correlationCurve.setCorrelationData(correlationData);

	//perform the correlation
	correlationCurve.recalculate();
	const XYCorrelationCurve::CorrelationResult& correlationResult = correlationCurve.correlationResult();

	//check the results
	QCOMPARE(correlationResult.available, true);
	QCOMPARE(correlationResult.valid, true);

	const AbstractColumn* resultXDataColumn = correlationCurve.xColumn();
	const AbstractColumn* resultYDataColumn = correlationCurve.yColumn();

	const int np = resultXDataColumn->rowCount();
	QCOMPARE(np, 7);

	for (int i = 0; i < np; i++)
		QCOMPARE(resultXDataColumn->valueAt(i), (double)i + 1);

	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(0));
	FuzzyCompare(resultYDataColumn->valueAt(0), 0., 1.e-15);
	QCOMPARE(resultYDataColumn->valueAt(1), 0.5);
	QCOMPARE(resultYDataColumn->valueAt(2), 2.);
	QCOMPARE(resultYDataColumn->valueAt(3), 3.5);
	QCOMPARE(resultYDataColumn->valueAt(4), 5.);
	QCOMPARE(resultYDataColumn->valueAt(5), 4.);
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(6));
	FuzzyCompare(resultYDataColumn->valueAt(6), 0., 2.e-15);
}

void CorrelationTest::testLinear2() {
	// data
	QVector<int> xData = {1, 2, 3};
	QVector<double> yData = {1.,2.,3.};
	QVector<double> y2Data = {0,1.,.5};

	//data source columns
	Column xDataColumn("x", AbstractColumn::Integer);
	xDataColumn.replaceInteger(0, xData);

	Column yDataColumn("y", AbstractColumn::Numeric);
	yDataColumn.replaceValues(0, yData);

	Column y2DataColumn("y2", AbstractColumn::Numeric);
	y2DataColumn.replaceValues(0, y2Data);

	XYCorrelationCurve correlationCurve("correlation");
	correlationCurve.setXDataColumn(&xDataColumn);
	correlationCurve.setYDataColumn(&yDataColumn);
	correlationCurve.setY2DataColumn(&y2DataColumn);

	//prepare the correlation
	XYCorrelationCurve::CorrelationData correlationData = correlationCurve.correlationData();
	correlationCurve.setCorrelationData(correlationData);

	//perform the correlation
	correlationCurve.recalculate();
	const XYCorrelationCurve::CorrelationResult& correlationResult = correlationCurve.correlationResult();

	//check the results
	QCOMPARE(correlationResult.available, true);
	QCOMPARE(correlationResult.valid, true);

	const AbstractColumn* resultXDataColumn = correlationCurve.xColumn();
	const AbstractColumn* resultYDataColumn = correlationCurve.yColumn();

	const int np = resultXDataColumn->rowCount();
	QCOMPARE(np, 5);

	for (int i = 0; i < np; i++)
		QCOMPARE(resultXDataColumn->valueAt(i), (double)i + 1);

	QCOMPARE(resultYDataColumn->valueAt(0), 0.5);
	QCOMPARE(resultYDataColumn->valueAt(1), 2.);
	QCOMPARE(resultYDataColumn->valueAt(2), 3.5);
	QCOMPARE(resultYDataColumn->valueAt(3), 3.);
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(4));
	FuzzyCompare(resultYDataColumn->valueAt(4), 0., 1.e-15);
}

void CorrelationTest::testLinear_noX() {
	// data
	QVector<double> yData = {1.,2.,3.,4.};
	QVector<double> y2Data = {0,1.,.5};

	//data source columns
	Column yDataColumn("y", AbstractColumn::Numeric);
	yDataColumn.replaceValues(0, yData);

	Column y2DataColumn("y2", AbstractColumn::Numeric);
	y2DataColumn.replaceValues(0, y2Data);

	XYCorrelationCurve correlationCurve("correlation");
	correlationCurve.setYDataColumn(&yDataColumn);
	correlationCurve.setY2DataColumn(&y2DataColumn);

	//prepare the correlation
	XYCorrelationCurve::CorrelationData correlationData = correlationCurve.correlationData();
	correlationCurve.setCorrelationData(correlationData);

	//perform the correlation
	correlationCurve.recalculate();
	const XYCorrelationCurve::CorrelationResult& correlationResult = correlationCurve.correlationResult();

	//check the results
	QCOMPARE(correlationResult.available, true);
	QCOMPARE(correlationResult.valid, true);

	const AbstractColumn* resultXDataColumn = correlationCurve.xColumn();
	const AbstractColumn* resultYDataColumn = correlationCurve.yColumn();

	const int np = resultXDataColumn->rowCount();
	QCOMPARE(np, 7);

	for (int i = 0; i < np; i++)
		QCOMPARE(resultXDataColumn->valueAt(i), (double)(i-np/2));

	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(0));
	FuzzyCompare(resultYDataColumn->valueAt(0), 0., 1.e-15);
	QCOMPARE(resultYDataColumn->valueAt(1), 0.5);
	QCOMPARE(resultYDataColumn->valueAt(2), 2.);
	QCOMPARE(resultYDataColumn->valueAt(3), 3.5);
	QCOMPARE(resultYDataColumn->valueAt(4), 5.);
	QCOMPARE(resultYDataColumn->valueAt(5), 4.);
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(6));
	FuzzyCompare(resultYDataColumn->valueAt(6), 0., 2.e-15);
}

void CorrelationTest::testLinear_swapped() {
	// data
	QVector<int> xData = {1,2,3,4};
	QVector<double> yData = {0,1.,.5};
	QVector<double> y2Data = {1.,2.,3.,4.};

	//data source columns
	Column xDataColumn("x", AbstractColumn::Integer);
	xDataColumn.replaceInteger(0, xData);

	Column yDataColumn("y", AbstractColumn::Numeric);
	yDataColumn.replaceValues(0, yData);

	Column y2DataColumn("y2", AbstractColumn::Numeric);
	y2DataColumn.replaceValues(0, y2Data);

	XYCorrelationCurve correlationCurve("correlation");
	correlationCurve.setXDataColumn(&xDataColumn);
	correlationCurve.setYDataColumn(&yDataColumn);
	correlationCurve.setY2DataColumn(&y2DataColumn);

	//prepare the correlation
	XYCorrelationCurve::CorrelationData correlationData = correlationCurve.correlationData();
	correlationCurve.setCorrelationData(correlationData);

	//perform the correlation
	correlationCurve.recalculate();
	const XYCorrelationCurve::CorrelationResult& correlationResult = correlationCurve.correlationResult();

	//check the results
	QCOMPARE(correlationResult.available, true);
	QCOMPARE(correlationResult.valid, true);

	const AbstractColumn* resultXDataColumn = correlationCurve.xColumn();
	const AbstractColumn* resultYDataColumn = correlationCurve.yColumn();

	const int np = resultXDataColumn->rowCount();
	QCOMPARE(np, 7);

	for (int i = 0; i < np; i++)
		QCOMPARE(resultXDataColumn->valueAt(i), (double)i + 1);

	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(0));
	FuzzyCompare(resultYDataColumn->valueAt(0), 0., 2.e-15);
	QCOMPARE(resultYDataColumn->valueAt(1), 4.);
	QCOMPARE(resultYDataColumn->valueAt(2), 5.);
	QCOMPARE(resultYDataColumn->valueAt(3), 3.5);
	QCOMPARE(resultYDataColumn->valueAt(4), 2.);
	QCOMPARE(resultYDataColumn->valueAt(5), 0.5);
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(6));
	FuzzyCompare(resultYDataColumn->valueAt(6), 0., 1.e-15);
}

/* norm tests */

void CorrelationTest::testLinear_biased() {
	// data
	QVector<double> yData = {1.,2.,3.,4.};
	QVector<double> y2Data = {0,1.,.5};

	//data source columns
	Column yDataColumn("y", AbstractColumn::Numeric);
	yDataColumn.replaceValues(0, yData);

	Column y2DataColumn("y2", AbstractColumn::Numeric);
	y2DataColumn.replaceValues(0, y2Data);

	XYCorrelationCurve correlationCurve("correlation");
	correlationCurve.setYDataColumn(&yDataColumn);
	correlationCurve.setY2DataColumn(&y2DataColumn);

	//prepare the correlation
	XYCorrelationCurve::CorrelationData correlationData = correlationCurve.correlationData();
	correlationData.normalize = nsl_corr_norm_biased;
	correlationCurve.setCorrelationData(correlationData);

	//perform the correlation
	correlationCurve.recalculate();
	const XYCorrelationCurve::CorrelationResult& correlationResult = correlationCurve.correlationResult();

	//check the results
	QCOMPARE(correlationResult.available, true);
	QCOMPARE(correlationResult.valid, true);

	const AbstractColumn* resultXDataColumn = correlationCurve.xColumn();
	const AbstractColumn* resultYDataColumn = correlationCurve.yColumn();

	const int np = resultXDataColumn->rowCount();
	QCOMPARE(np, 7);

	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(0));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(1));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(2));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(3));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(4));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(5));
	FuzzyCompare(resultYDataColumn->valueAt(0), 0., 1.e-15);
	QCOMPARE(resultYDataColumn->valueAt(1), 0.125);
	QCOMPARE(resultYDataColumn->valueAt(2), .5);
	QCOMPARE(resultYDataColumn->valueAt(3), 3.5/4.);
	QCOMPARE(resultYDataColumn->valueAt(4), 1.25);
	QCOMPARE(resultYDataColumn->valueAt(5), 1.);
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(6));
	FuzzyCompare(resultYDataColumn->valueAt(6), 0., 2.e-15);
}

void CorrelationTest::testLinear2_biased() {
	// data
	QVector<double> yData = {1.,2.,3.};
	QVector<double> y2Data = {0,1.,.5};

	//data source columns
	Column yDataColumn("y", AbstractColumn::Numeric);
	yDataColumn.replaceValues(0, yData);

	Column y2DataColumn("y2", AbstractColumn::Numeric);
	y2DataColumn.replaceValues(0, y2Data);

	XYCorrelationCurve correlationCurve("correlation");
	correlationCurve.setYDataColumn(&yDataColumn);
	correlationCurve.setY2DataColumn(&y2DataColumn);

	//prepare the correlation
	XYCorrelationCurve::CorrelationData correlationData = correlationCurve.correlationData();
	correlationData.normalize = nsl_corr_norm_biased;
	correlationCurve.setCorrelationData(correlationData);

	//perform the correlation
	correlationCurve.recalculate();
	const XYCorrelationCurve::CorrelationResult& correlationResult = correlationCurve.correlationResult();

	//check the results
	QCOMPARE(correlationResult.available, true);
	QCOMPARE(correlationResult.valid, true);

	const AbstractColumn* resultXDataColumn = correlationCurve.xColumn();
	const AbstractColumn* resultYDataColumn = correlationCurve.yColumn();

	const int np = resultXDataColumn->rowCount();
	QCOMPARE(np, 5);

	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(0));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(1));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(2));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(3));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(4));
	QCOMPARE(resultYDataColumn->valueAt(0), 1./6.);
	QCOMPARE(resultYDataColumn->valueAt(1), 2./3.);
	QCOMPARE(resultYDataColumn->valueAt(2), 7./6.);
	QCOMPARE(resultYDataColumn->valueAt(3), 1.);
	FuzzyCompare(resultYDataColumn->valueAt(4), 0., 1.e-15);
}

void CorrelationTest::testLinear_unbiased() {
	// data
	QVector<double> yData = {1.,2.,3.,4.};
	QVector<double> y2Data = {0,1.,.5};

	//data source columns
	Column yDataColumn("y", AbstractColumn::Numeric);
	yDataColumn.replaceValues(0, yData);

	Column y2DataColumn("y2", AbstractColumn::Numeric);
	y2DataColumn.replaceValues(0, y2Data);

	XYCorrelationCurve correlationCurve("correlation");
	correlationCurve.setYDataColumn(&yDataColumn);
	correlationCurve.setY2DataColumn(&y2DataColumn);

	//prepare the correlation
	XYCorrelationCurve::CorrelationData correlationData = correlationCurve.correlationData();
	correlationData.normalize = nsl_corr_norm_unbiased;
	correlationCurve.setCorrelationData(correlationData);

	//perform the correlation
	correlationCurve.recalculate();
	const XYCorrelationCurve::CorrelationResult& correlationResult = correlationCurve.correlationResult();

	//check the results
	QCOMPARE(correlationResult.available, true);
	QCOMPARE(correlationResult.valid, true);

	const AbstractColumn* resultXDataColumn = correlationCurve.xColumn();
	const AbstractColumn* resultYDataColumn = correlationCurve.yColumn();

	const int np = resultXDataColumn->rowCount();
	QCOMPARE(np, 7);

	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(0));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(1));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(2));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(3));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(4));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(5));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(6));
	FuzzyCompare(resultYDataColumn->valueAt(0), 0., 1.e-15);
	QCOMPARE(resultYDataColumn->valueAt(1), 0.25);
	QCOMPARE(resultYDataColumn->valueAt(2), 2./3.);
	QCOMPARE(resultYDataColumn->valueAt(3), 3.5/4.);
	QCOMPARE(resultYDataColumn->valueAt(4), 5./3.);
	QCOMPARE(resultYDataColumn->valueAt(5), 2.);
	FuzzyCompare(resultYDataColumn->valueAt(6), 0., 2.e-15);
}

void CorrelationTest::testLinear2_unbiased() {
	// data
	QVector<double> yData = {1.,2.,3.};
	QVector<double> y2Data = {0,1.,.5};

	//data source columns
	Column yDataColumn("y", AbstractColumn::Numeric);
	yDataColumn.replaceValues(0, yData);

	Column y2DataColumn("y2", AbstractColumn::Numeric);
	y2DataColumn.replaceValues(0, y2Data);

	XYCorrelationCurve correlationCurve("correlation");
	correlationCurve.setYDataColumn(&yDataColumn);
	correlationCurve.setY2DataColumn(&y2DataColumn);

	//prepare the correlation
	XYCorrelationCurve::CorrelationData correlationData = correlationCurve.correlationData();
	correlationData.normalize = nsl_corr_norm_unbiased;
	correlationCurve.setCorrelationData(correlationData);

	//perform the correlation
	correlationCurve.recalculate();
	const XYCorrelationCurve::CorrelationResult& correlationResult = correlationCurve.correlationResult();

	//check the results
	QCOMPARE(correlationResult.available, true);
	QCOMPARE(correlationResult.valid, true);

	const AbstractColumn* resultXDataColumn = correlationCurve.xColumn();
	const AbstractColumn* resultYDataColumn = correlationCurve.yColumn();

	const int np = resultXDataColumn->rowCount();
	QCOMPARE(np, 5);

	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(0));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(1));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(2));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(3));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(4));
	QCOMPARE(resultYDataColumn->valueAt(0), 0.5);
	QCOMPARE(resultYDataColumn->valueAt(1), 1.);
	QCOMPARE(resultYDataColumn->valueAt(2), 7./6.);
	QCOMPARE(resultYDataColumn->valueAt(3), 1.5);
	FuzzyCompare(resultYDataColumn->valueAt(4), 0., 1.e-15);
}

void CorrelationTest::testLinear_coeff() {
	// data
	QVector<double> yData = {1.,2.,3.,4.};
	QVector<double> y2Data = {0,1.,.5};

	//data source columns
	Column yDataColumn("y", AbstractColumn::Numeric);
	yDataColumn.replaceValues(0, yData);

	Column y2DataColumn("y2", AbstractColumn::Numeric);
	y2DataColumn.replaceValues(0, y2Data);

	XYCorrelationCurve correlationCurve("correlation");
	correlationCurve.setYDataColumn(&yDataColumn);
	correlationCurve.setY2DataColumn(&y2DataColumn);

	//prepare the correlation
	XYCorrelationCurve::CorrelationData correlationData = correlationCurve.correlationData();
	correlationData.normalize = nsl_corr_norm_coeff;
	correlationCurve.setCorrelationData(correlationData);

	//perform the correlation
	correlationCurve.recalculate();
	const XYCorrelationCurve::CorrelationResult& correlationResult = correlationCurve.correlationResult();

	//check the results
	QCOMPARE(correlationResult.available, true);
	QCOMPARE(correlationResult.valid, true);

	const AbstractColumn* resultXDataColumn = correlationCurve.xColumn();
	const AbstractColumn* resultYDataColumn = correlationCurve.yColumn();

	const int np = resultXDataColumn->rowCount();
	QCOMPARE(np, 7);

	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(0));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(1));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(2));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(3));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(4));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(5));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(6));
	FuzzyCompare(resultYDataColumn->valueAt(0), 0., 1.e-15);
	double norm = sqrt(30) * sqrt(1.25);
	QCOMPARE(resultYDataColumn->valueAt(1), 0.5/norm);
	QCOMPARE(resultYDataColumn->valueAt(2), 2./norm);
	QCOMPARE(resultYDataColumn->valueAt(3), 3.5/norm);
	QCOMPARE(resultYDataColumn->valueAt(4), 5./norm);
	QCOMPARE(resultYDataColumn->valueAt(5), 4./norm);
	FuzzyCompare(resultYDataColumn->valueAt(6), 0., 2.e-15);
}

void CorrelationTest::testLinear2_coeff() {
	// data
	QVector<double> yData = {1.,2.,3.};
	QVector<double> y2Data = {0,1.,.5};

	//data source columns
	Column yDataColumn("y", AbstractColumn::Numeric);
	yDataColumn.replaceValues(0, yData);

	Column y2DataColumn("y2", AbstractColumn::Numeric);
	y2DataColumn.replaceValues(0, y2Data);

	XYCorrelationCurve correlationCurve("correlation");
	correlationCurve.setYDataColumn(&yDataColumn);
	correlationCurve.setY2DataColumn(&y2DataColumn);

	//prepare the correlation
	XYCorrelationCurve::CorrelationData correlationData = correlationCurve.correlationData();
	correlationData.normalize = nsl_corr_norm_coeff;
	correlationCurve.setCorrelationData(correlationData);

	//perform the correlation
	correlationCurve.recalculate();
	const XYCorrelationCurve::CorrelationResult& correlationResult = correlationCurve.correlationResult();

	//check the results
	QCOMPARE(correlationResult.available, true);
	QCOMPARE(correlationResult.valid, true);

	const AbstractColumn* resultXDataColumn = correlationCurve.xColumn();
	const AbstractColumn* resultYDataColumn = correlationCurve.yColumn();

	const int np = resultXDataColumn->rowCount();
	QCOMPARE(np, 5);

	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(0));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(1));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(2));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(3));
	DEBUG(std::setprecision(15) << resultYDataColumn->valueAt(4));
	double norm = sqrt(14.)*sqrt(1.25);
	QCOMPARE(resultYDataColumn->valueAt(0), .5/norm);
	QCOMPARE(resultYDataColumn->valueAt(1), 2./norm);
	QCOMPARE(resultYDataColumn->valueAt(2), 3.5/norm);
	QCOMPARE(resultYDataColumn->valueAt(3), 3./norm);
	FuzzyCompare(resultYDataColumn->valueAt(4), 0., 1.e-15);
}

void CorrelationTest::testPerformance() {
	// data
	QVector<double> yData, y2Data;
#ifdef HAVE_FFTW3
	const int N = 5e6;
#else	// GSL is much slower
	const int N = 8.5e4;
#endif
	for (int i = 0;  i < N; i++) {
		yData.append(i % 100);
		y2Data.append(i % 10);
	}

	//data source columns
	Column yDataColumn("y", AbstractColumn::Numeric);
	yDataColumn.replaceValues(0, yData);

	Column y2DataColumn("y2", AbstractColumn::Numeric);
	y2DataColumn.replaceValues(0, y2Data);

	XYCorrelationCurve correlationCurve("correlation");
	correlationCurve.setYDataColumn(&yDataColumn);
	correlationCurve.setY2DataColumn(&y2DataColumn);

	//prepare and perform the correlation
	XYCorrelationCurve::CorrelationData correlationData = correlationCurve.correlationData();
	QBENCHMARK {
		// triggers recalculate()
		correlationCurve.setCorrelationData(correlationData);
	}

	//check the results
	const XYCorrelationCurve::CorrelationResult& correlationResult = correlationCurve.correlationResult();

	QCOMPARE(correlationResult.available, true);
	QCOMPARE(correlationResult.valid, true);

	const AbstractColumn* resultXDataColumn = correlationCurve.xColumn();

	const int np = resultXDataColumn->rowCount();
	QCOMPARE(np, 2*N - 1);
}

QTEST_MAIN(CorrelationTest)
