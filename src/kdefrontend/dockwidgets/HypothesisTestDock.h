/***************************************************************************
    File                 : HypothesisTestDock.h
    Project              : LabPlot
    Description          : widget for hypothesis testing properties
    --------------------------------------------------------------------
    Copyright            : (C) 2019 Devanshu Agarwal(agarwaldevanshu8@gmail.com)

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

#ifndef HYPOTHESISTESTDOCK_H
#define HYPOTHESISTESTDOCK_H

#include "backend/hypothesis_test/HypothesisTest.h"
#include "ui_hypothesistestdock.h"
#include <QSqlDatabase>

class AbstractAspect;
class AspectTreeModel;
class HypothesisTest;
class TreeViewComboBox;
class KConfig;
class QScrollArea;
class QStandardItemModel;
class QStandardItem;

class HypothesisTestDock : public QWidget {
    Q_OBJECT

public:
    struct Test {
        enum Type {
            NoneType  = 0,
            TTest = 1 << 0,
            ZTest = 1 << 1,
            Anova = 1 << 2
        };
        enum SubType {
            NoneSubType = 0,
            TwoSampleIndependent    = 1 << 0,
            TwoSamplePaired         = 1 << 1,
            OneSample               = 1 << 2,
            OneWay                  = 1 << 3,
            TwoWay                  = 1 << 4
        };
        Type type = NoneType;
        SubType subtype = NoneSubType;
    };

    explicit HypothesisTestDock(QWidget*);
    void setHypothesisTest(HypothesisTest*);

private slots:
    void onRbH1OneTail1Toggled(bool checked);
    void onRbH1OneTail2Toggled(bool checked);
    void onRbH1TwoTailToggled(bool checked);

private slots:

private:
    Ui::HypothesisTestDock ui;
    bool m_initializing{false};
    TreeViewComboBox* cbSpreadsheet{nullptr};
    HypothesisTest* m_hypothesisTest{nullptr};
    AspectTreeModel* m_aspectTreeModel{nullptr};
    QSqlDatabase m_db;
    QString m_configPath;
    double population_mean{0};
    double significance_level{0.05};
    //        void load();
    //        void loadConfig(KConfig&);
    void setModelIndexFromAspect(TreeViewComboBox*, const AbstractAspect*);
    //        void readConnections();
    //        void updateFields();
    //        bool fieldSelected(const QString&);
    Test m_test;
    QScrollArea* scroll_dock;

    void countPartitions(Column *column, int &np, int &total_rows);
    void setColumnsComboBoxModel(Spreadsheet* spreadsheet);
    void setColumnsComboBoxView();
    bool nonEmptySelectedColumns();

    QStringList only_values_cols;
    QStringList two_categorical_cols;
    QStringList more_than_two_categorical_cols;

private slots:
    //SLOTs for changes triggered in PivotTableDock
    //        void nameChanged();
    //        void commentChanged();
    void dataSourceTypeChanged(int);
    void showTestType();
    void showHypothesisTest();
    void doHypothesisTest();
    void performLeveneTest();
    void spreadsheetChanged(const QModelIndex&);
    void changeCbCol2Label();
    void col1IndexChanged(int index);
    //        void connectionChanged();
    //        void tableChanged();
    //        void showDatabaseManager();

    //        //SLOTs for changes triggered in PivotTable
    //        void pivotTableDescriptionChanged(const AbstractAspect*);

    //        void addRow();
    //        void removeRow();
    //        void addColumn();
    //        void removeColumn();

    //        //save/load template
    //        void loadConfigFromTemplate(KConfig&);
    //        void saveConfigAsTemplate(KConfig&);

signals:
    //        void info(const QString&);
};

#endif // PIVOTTABLEDOCK_H