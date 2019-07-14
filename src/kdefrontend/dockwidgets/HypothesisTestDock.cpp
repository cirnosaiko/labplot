﻿/***************************************************************************
    File                 : HypothesisTestDock.cpp
    Project              : LabPlot
    Description          : widget for hypothesis test properties
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

#include "HypothesisTestDock.h"
#include "backend/core/AspectTreeModel.h"
#include "backend/core/AbstractAspect.h"
#include "backend/core/Project.h"
#include "backend/spreadsheet/Spreadsheet.h"
#include "commonfrontend/widgets/TreeViewComboBox.h"
#include "kdefrontend/datasources/DatabaseManagerDialog.h"
#include "kdefrontend/datasources/DatabaseManagerWidget.h"
//#include "kdefrontend/pivot/hypothesisTestView.h"
#include "kdefrontend/TemplateHandler.h"

#include <QDir>
#include <QSqlError>
#include <QScrollArea>

#include <KConfig>
#include <KConfigGroup>
#include <KMessageBox>
#include <QDebug>

#include <QStandardItemModel>
#include <QAbstractItemModel>
/*!
  \class HypothesisTestDock
  \brief Provides a dock (widget) for hypothesis testing:
  \ingroup kdefrontend
*/

//TOOD: Make this dock widget scrollable and automatic resizeable for different screens.

HypothesisTestDock::HypothesisTestDock(QWidget* parent) : QWidget(parent) {
    ui.setupUi(this);

    ui.cbDataSourceType->addItem(i18n("Spreadsheet"));
    ui.cbDataSourceType->addItem(i18n("Database"));

    cbSpreadsheet = new TreeViewComboBox;
    ui.gridLayout->addWidget(cbSpreadsheet, 5, 4, 1, 3);

    ui.bDatabaseManager->setIcon(QIcon::fromTheme("network-server-database"));
    ui.bDatabaseManager->setToolTip(i18n("Manage connections"));
    m_configPath = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).constFirst() +  "sql_connections";

    // adding item to tests and testtype combo box;

    ui.cbTest->addItem( i18n("T Test"), Test::Type::TTest);
    ui.cbTest->addItem( i18n("Z Test"), Test::Type::ZTest);
    ui.cbTest->addItem( i18n("ANOVA"), Test::Type::Anova);

    // making all test blocks invisible at starting.
    ui.pbLeveneTest->hide();
    ui.lCategorical->hide();
    ui.chbCategorical->hide();
    ui.lCol1->hide();
    ui.cbCol1->hide();
    ui.lCol2->hide();
    ui.cbCol2->hide();
    ui.lEqualVariance->hide();
    ui.chbEqualVariance->hide();
    ui.chbEqualVariance->setChecked(true);
    ui.pbPerformTest->setEnabled(false);
    ui.rbH1OneTail2->hide();
    ui.rbH1OneTail1->hide();
    ui.rbH1TwoTail->hide();
    ui.rbH0OneTail1->hide();
    ui.rbH0OneTail2->hide();
    ui.rbH0TwoTail->hide();
    ui.lH0->hide();
    ui.lH1->hide();

    QString mu = UTF8_QSTRING("μ");
    QString mu0 = UTF8_QSTRING("μₒ");

    // radio button for null and alternate hypothesis
    // for alternative hypothesis (h1)
    // one_tail_1 is mu > mu0; one_tail_2 is mu < mu0; two_tail = mu != mu0;

    ui.rbH1OneTail1->setText( i18n("%1 %2 %3", mu, UTF8_QSTRING(">"), mu0));
    ui.rbH1OneTail2->setText( i18n("%1 %2 %3", mu, UTF8_QSTRING("<"), mu0));
    ui.rbH1TwoTail->setText( i18n("%1 %2 %3", mu, UTF8_QSTRING("≠"), mu0));

    ui.rbH0OneTail1->setText( i18n("%1 %2 %3",mu, UTF8_QSTRING("≤"), mu0));
    ui.rbH0OneTail2->setText( i18n("%1 %2 %3", mu, UTF8_QSTRING("≥"), mu0));
    ui.rbH0TwoTail->setText( i18n("%1 %2 %3", mu, UTF8_QSTRING("="), mu0));

    ui.rbH0TwoTail->setEnabled(false);
    ui.rbH0OneTail1->setEnabled(false);
    ui.rbH0OneTail2->setEnabled(false);


    // setting muo and alpha buttons
    ui.lMuo->setText( i18n("%1", mu0));
    ui.lAlpha->setText( i18n("%1", UTF8_QSTRING("α")));
    ui.leMuo->setText( i18n("%1", population_mean));
    ui.leAlpha->setText( i18n("%1", significance_level));

    ui.lMuo->hide();
    ui.lAlpha->hide();
    ui.leMuo->hide();
    ui.leAlpha->hide();

    ui.pbPerformTest->setIcon(QIcon::fromTheme("run-build"));

    ui.leMuo->setText( i18n("%1", population_mean));
    ui.leAlpha->setText( i18n("%1", significance_level));


    //    readConnections();

    //    auto* style = ui.bAddRow->style();
    //    ui.bAddRow->setIcon(style->standardIcon(QStyle::SP_ArrowRight));
    //    ui.bAddRow->setToolTip(i18n("Add the selected field to rows"));
    //    ui.bRemoveRow->setIcon(style->standardIcon(QStyle::SP_ArrowLeft));
    //    ui.bRemoveRow->setToolTip(i18n("Remove the selected field from rows"));

    //    ui.bAddColumn->setIcon(style->standardIcon(QStyle::SP_ArrowRight));
    //    ui.bAddColumn->setToolTip(i18n("Add the selected field to columns"));
    //    ui.bRemoveColumn->setIcon(style->standardIcon(QStyle::SP_ArrowLeft));
    //    ui.bRemoveColumn->setToolTip(i18n("Remove the selected field from columns"));

    //    //add/remove buttons only enabled if something was selected
    //    ui.bAddRow->setEnabled(false);
    //    ui.bRemoveRow->setEnabled(false);
    //    ui.bAddColumn->setEnabled(false);
    //    ui.bRemoveColumn->setEnabled(false);

    //    connect(ui.leName, &QLineEdit::textChanged, this, &HypothesisTestDock::nameChanged);
    //    connect(ui.leComment, &QLineEdit::textChanged, this, &HypothesisTestDock::commentChanged);
    connect(ui.cbDataSourceType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &HypothesisTestDock::dataSourceTypeChanged);

    connect(cbSpreadsheet, &TreeViewComboBox::currentModelIndexChanged, this, &HypothesisTestDock::spreadsheetChanged);
    //    connect(ui.cbConnection, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
    //            this, &HypothesisTestDock::connectionChanged);
    //    connect(ui.cbTable, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
    //            this, &HypothesisTestDock::tableChanged);
    //    connect(ui.bDatabaseManager, &QPushButton::clicked, this, &HypothesisTestDock::showDatabaseManager);

    //    connect(ui.bAddRow,  &QPushButton::clicked, this, &HypothesisTestDock::addRow);
    //    connect(ui.bRemoveRow, &QPushButton::clicked, this,&HypothesisTestDock::removeRow);
    //    connect(ui.bAddColumn,  &QPushButton::clicked, this, &HypothesisTestDock::addColumn);
    //    connect(ui.bRemoveColumn, &QPushButton::clicked, this,&HypothesisTestDock::removeColumn);

    //      connect(ui.cbCol1, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &HypothesisTestDock::doTTest);
    //      connect(ui.cbCol2, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &HypothesisTestDock::doTTest);

    //    connect(ui.lwFields, &QListWidget::itemSelectionChanged, this, [=]() {
    //        bool enabled = !ui.lwFields->selectedItems().isEmpty();
    //        ui.bAddRow->setEnabled(enabled);
    //        ui.bAddColumn->setEnabled(enabled);
    //    });

    //    connect(ui.lwRows, &QListWidget::doubleClicked, this,&HypothesisTestDock::removeRow);
    //    connect(ui.lwRows, &QListWidget::itemSelectionChanged, this, [=]() {
    //        ui.bRemoveRow->setEnabled(!ui.lwRows->selectedItems().isEmpty());
    //    });

    //    connect(ui.lwColumns, &QListWidget::doubleClicked, this,&HypothesisTestDock::removeColumn);
    //    connect(ui.lwColumns, &QListWidget::itemSelectionChanged, this, [=]() {
    //        ui.bRemoveColumn->setEnabled(!ui.lwColumns->selectedItems().isEmpty());
    //    });

    connect(ui.cbTest, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &HypothesisTestDock::showTestType);
    connect(ui.cbTestType, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &HypothesisTestDock::showHypothesisTest);
    //    connect(ui.cbTest, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &HypothesisTestDock::showHypothesisTest);
    //    connect(ui.cbTestType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &HypothesisTestDock::showHypothesisTest);
    connect(ui.pbPerformTest, &QPushButton::clicked, this, &HypothesisTestDock::doHypothesisTest);
    connect(ui.pbLeveneTest, &QPushButton::clicked, this, &HypothesisTestDock::performLeveneTest);


    //connecting null hypothesis and alternate hypothesis radio button
    connect(ui.rbH1OneTail1, &QRadioButton::toggled, this, &HypothesisTestDock::onRbH1OneTail1Toggled);
    connect(ui.rbH1OneTail2, &QRadioButton::toggled, this, &HypothesisTestDock::onRbH1OneTail2Toggled);
    connect(ui.rbH1TwoTail, &QRadioButton::toggled, this, &HypothesisTestDock::onRbH1TwoTailToggled);

    connect(ui.cbCol1, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &HypothesisTestDock::col1IndexChanged);
    connect(ui.chbCategorical, &QCheckBox::stateChanged, this, &HypothesisTestDock::changeCbCol2Label);
}

void HypothesisTestDock::setHypothesisTest(HypothesisTest* HypothesisTest) {
    m_initializing = true;
    m_hypothesisTest = HypothesisTest;

    m_aspectTreeModel = new AspectTreeModel(m_hypothesisTest->project());

    QList<const char*> list;
    list << "Folder" << "Workbook" << "Spreadsheet" << "LiveDataSource";
    cbSpreadsheet->setTopLevelClasses(list);

    list.clear();
    list << "Spreadsheet" << "LiveDataSource";
    m_aspectTreeModel->setSelectableAspects(list);

    cbSpreadsheet->setModel(m_aspectTreeModel);

    //show the properties
    ui.leName->setText(m_hypothesisTest->name());
    ui.leComment->setText(m_hypothesisTest->comment());
    ui.cbDataSourceType->setCurrentIndex(m_hypothesisTest->dataSourceType());
    if (m_hypothesisTest->dataSourceType() == HypothesisTest::DataSourceSpreadsheet)
        setModelIndexFromAspect(cbSpreadsheet, m_hypothesisTest->dataSourceSpreadsheet());
    //    else
    //        ui.cbConnection->setCurrentIndex(ui.cbConnection->findText(m_hypothesisTest->dataSourceConnection()));

    setColumnsComboBoxModel(m_hypothesisTest->dataSourceSpreadsheet());

    this->dataSourceTypeChanged(ui.cbDataSourceType->currentIndex());

    //setting rows and columns in combo box;

    //undo functions
    connect(m_hypothesisTest, SIGNAL(aspectDescriptionChanged(const AbstractAspect*)), this, SLOT(hypothesisTestDescriptionChanged(const AbstractAspect*)));
    //TODO:

    m_initializing = false;
}

void HypothesisTestDock::showTestType() {
    m_test.type = Test::Type(ui.cbTest->currentData().toInt());

    ui.cbTestType->clear();
    if (m_test.type & (Test::Type::TTest | Test::Type::ZTest)) {
        ui.cbTestType->addItem( i18n("Two Sample Independent"), Test::SubType::TwoSampleIndependent);
        ui.cbTestType->addItem( i18n("Two Sample Paired"), Test::SubType::TwoSamplePaired);
        ui.cbTestType->addItem( i18n("One Sample"), Test::SubType::OneSample);
    }
    else if (m_test.type & Test::Type::Anova) {
        ui.cbTestType->addItem( i18n("One Way"), Test::SubType::OneWay);
        ui.cbTestType->addItem( i18n("Two Way"), Test::SubType::TwoWay);
    }

    showHypothesisTest();
}

void HypothesisTestDock::showHypothesisTest() {
    m_test.subtype = Test::SubType(ui.cbTestType->currentData().toInt());

    ui.lCol1->show();
    ui.cbCol1->show();

    ui.lCol2->setVisible(m_test.subtype & (~Test::SubType::OneSample));
    ui.cbCol2->setVisible(m_test.subtype & (~Test::SubType::OneSample));

    ui.lEqualVariance->setVisible( (m_test.type & Test::Type::TTest) &
                                   (m_test.subtype & Test::SubType::TwoSampleIndependent));
    ui.chbEqualVariance->setVisible( (m_test.type & Test::Type::TTest) &
                                     (m_test.subtype & Test::SubType::TwoSampleIndependent));

    ui.lCategorical->setVisible( (m_test.type & Test::Type::TTest) &
                                 (m_test.subtype & Test::SubType::TwoSampleIndependent));
    ui.chbCategorical->setVisible( (m_test.type & Test::Type::TTest) &
                                   (m_test.subtype & Test::SubType::TwoSampleIndependent));

    ui.pbLeveneTest->setVisible( (m_test.type & Test::Type::Anova) |
                                 ( (m_test.type & Test::Type::TTest) &
                                   (m_test.subtype & Test::SubType::TwoSampleIndependent)));

    ui.chbEqualVariance->setChecked(true);

    ui.lH1->setVisible( (m_test.type & ~Test::Type::Anova));
    ui.rbH1OneTail1->setVisible( (m_test.type & ~Test::Type::Anova));
    ui.rbH1OneTail2->setVisible( (m_test.type & ~Test::Type::Anova));
    ui.rbH1TwoTail->setVisible( (m_test.type & ~Test::Type::Anova));

    ui.lH0->setVisible( (m_test.type & ~Test::Type::Anova));
    ui.rbH0OneTail1->setVisible( (m_test.type & ~Test::Type::Anova));
    ui.rbH0OneTail2->setVisible( (m_test.type & ~Test::Type::Anova));
    ui.rbH0TwoTail->setVisible( (m_test.type & ~Test::Type::Anova));

    ui.rbH1TwoTail->setChecked(true);

    ui.lMuo->setVisible( (m_test.subtype & Test::SubType::OneSample));
    ui.leMuo->setVisible( (m_test.subtype & Test::SubType::OneSample));

    ui.lAlpha->show();
    ui.leAlpha->show();

    setColumnsComboBoxView();

    ui.pbPerformTest->setEnabled(nonEmptySelectedColumns());
    ui.pbLeveneTest->setEnabled(nonEmptySelectedColumns());
}

void HypothesisTestDock::doHypothesisTest()  {
    m_hypothesisTest->setPopulationMean(ui.leMuo->text());
    m_hypothesisTest->setSignificanceLevel(ui.leAlpha->text());

    QStringList cols;
    switch (m_test.type) {
    case Test::Type::TTest: {
        cols << ui.cbCol1->currentText() << ui.cbCol2->currentText();
        m_hypothesisTest->setColumns(cols);

        switch (m_test.subtype) {
        case Test::SubType::TwoSampleIndependent:
            m_hypothesisTest->performTwoSampleIndependentTTest(ui.chbCategorical->isChecked(), ui.chbEqualVariance->isChecked());
            break;
        case Test::SubType::TwoSamplePaired:
            m_hypothesisTest->performTwoSamplePairedTTest();
            break;
        case Test::SubType::OneSample: {
            cols << ui.cbCol1->currentText();
            m_hypothesisTest->setColumns(cols);
            m_hypothesisTest->performOneSampleTTest();
            break;
        }
        case Test::SubType::NoneSubType:
            break;
        case Test::SubType::OneWay:
            break;
        case Test::SubType::TwoWay:
            break;
        }
        break;
    }
    case Test::Type::ZTest: {
        cols << ui.cbCol1->currentText() << ui.cbCol2->currentText();
        m_hypothesisTest->setColumns(cols);

        switch (m_test.subtype) {
        case Test::SubType::TwoSampleIndependent:
            m_hypothesisTest->performTwoSampleIndependentZTest();
            break;
        case Test::SubType::TwoSamplePaired:
            m_hypothesisTest->performTwoSamplePairedZTest();
            break;
        case Test::SubType::OneSample: {
            cols << ui.cbCol1->currentText();
            m_hypothesisTest->setColumns(cols);
            m_hypothesisTest->performOneSampleZTest();
            break;
        }
        case Test::SubType::OneWay:
            break;
        case Test::SubType::TwoWay:
            break;
        case Test::SubType::NoneSubType:
            break;
        }

        break;
    }
    case Test::Type::Anova: {
        cols << ui.cbCol1->currentText() << ui.cbCol2->currentText();
        m_hypothesisTest->setColumns(cols);

        switch (m_test.subtype) {
        case Test::SubType::OneWay:
            m_hypothesisTest->performOneWayAnova();
            break;
        case Test::SubType::TwoWay:
            break;
        case Test::SubType::NoneSubType:
            break;
        case Test::SubType::TwoSampleIndependent:
            break;
        case Test::SubType::TwoSamplePaired:
            break;
        case Test::SubType::OneSample:
            break;
        }

        break;
    }
    case Test::Type::NoneType:
        break;
    }
}

void HypothesisTestDock::performLeveneTest()  {
    QStringList cols;
    cols << ui.cbCol1->currentText() << ui.cbCol2->currentText();
    m_hypothesisTest->setColumns(cols);
    m_hypothesisTest->setSignificanceLevel(ui.leAlpha->text());

    m_hypothesisTest->performLeveneTest(ui.chbCategorical->isChecked());
}

void HypothesisTestDock::setModelIndexFromAspect(TreeViewComboBox* cb, const AbstractAspect* aspect) {
    if (aspect)
        cb->setCurrentModelIndex(m_aspectTreeModel->modelIndexOfAspect(aspect));
    else
        cb->setCurrentModelIndex(QModelIndex());
}

///*!
//    shows the database manager where the connections are created and edited.
//    The selected connection is selected in the connection combo box in this widget.
//**/
//void HypothesisTestDock::showDatabaseManager() {
//    DatabaseManagerDialog* dlg = new DatabaseManagerDialog(this, ui.cbConnection->currentText());

//    if (dlg->exec() == QDialog::Accepted) {
//        //re-read the available connections to be in sync with the changes in DatabaseManager
//        m_initializing = true;
//        ui.cbConnection->clear();
//        readConnections();

//        //select the connection the user has selected in DatabaseManager
//        const QString& conn = dlg->connection();
//        ui.cbConnection->setCurrentIndex(ui.cbConnection->findText(conn));
//        m_initializing = false;

//        connectionChanged();
//    }

//    delete dlg;
//}

///*!
//    loads all available saved connections
//*/
//void HypothesisTestDock::readConnections() {
//    DEBUG("ImportSQLDatabaseWidget: reading available connections");
//    KConfig config(m_configPath, KConfig::SimpleConfig);
//    for (const auto& name : config.groupList())
//        ui.cbConnection->addItem(name);
//}

///*!
// * adds the selected field to the rows
// */
//void HypothesisTestDock::addRow() {
//    QString field = ui.lwFields->currentItem()->text();
//    ui.lwRows->addItem(field);
//    ui.lwFields->takeItem(ui.lwFields->currentRow());
//    m_hypothesisTest->addToRows(field);
//}

///*!
// * removes the selected field from the rows
// */
//void HypothesisTestDock::removeRow() {
//    const QString& field = ui.lwRows->currentItem()->text();
//    ui.lwRows->takeItem(ui.lwRows->currentRow());
//    m_hypothesisTest->removeFromRows(field);
//    updateFields();
//}

///*!
// * adds the selected field to the columns
// */
//void HypothesisTestDock::addColumn() {
//    QString field = ui.lwFields->currentItem()->text();
//    ui.lwColumns->addItem(field);
//    ui.lwFields->takeItem(ui.lwFields->currentRow());
//    m_hypothesisTest->addToColumns(field);
//}

///*!
// * removes the selected field from the columns
// */
//void HypothesisTestDock::removeColumn() {
//    const QString& field = ui.lwColumns->currentItem()->text();
//    ui.lwColumns->takeItem(ui.lwColumns->currentRow());
//    m_hypothesisTest->removeFromColumns(field);
//    updateFields();
//}

///*!
// * re-populates the content of the "Fields" list widget by adding the non-selected fields only.
// * called when a selected field is removed from rows or columns.
// */
//void HypothesisTestDock::updateFields() {
//    ui.lwFields->clear();
//    for (auto dimension : m_hypothesisTest->dimensions())
//        if (!fieldSelected(dimension))
//            ui.lwFields->addItem(new QListWidgetItem(QIcon::fromTheme("draw-text"), dimension));

//    for (auto measure : m_hypothesisTest->measures())
//        if (!fieldSelected(measure))
//            ui.lwFields->addItem(new QListWidgetItem(measure));
//}

///*!
// * return \c true if the field name \c field was selected among rows or columns,
// * return \c false otherwise.
// * */
//bool HypothesisTestDock::fieldSelected(const QString& field) {
//    for (int i = 0; i<ui.lwRows->count(); ++i)
//        if (ui.lwRows->item(i)->text() == field)
//            return true;

//    for (int i = 0; i<ui.lwColumns->count(); ++i)
//        if (ui.lwColumns->item(i)->text() == field)
//            return true;

//    return false;
//}

////*************************************************************
////****** SLOTs for changes triggered in HypothesisTestDock *******
////*************************************************************
//void HypothesisTestDock::nameChanged() {
//    if (m_initializing)
//        return;

//    m_hypothesisTest->setName(ui.leName->text());
//}

//void HypothesisTestDock::commentChanged() {
//    if (m_initializing)
//        return;

//    m_hypothesisTest->setComment(ui.leComment->text());
//}

void HypothesisTestDock::dataSourceTypeChanged(int index) {
    HypothesisTest::DataSourceType type = static_cast<HypothesisTest::DataSourceType>(index);
    bool showDatabase = (type == HypothesisTest::DataSourceDatabase);
    ui.lSpreadsheet->setVisible(!showDatabase);
    cbSpreadsheet->setVisible(!showDatabase);
    ui.lConnection->setVisible(showDatabase);
    ui.cbConnection->setVisible(showDatabase);
    ui.bDatabaseManager->setVisible(showDatabase);
    ui.lTable->setVisible(showDatabase);
    ui.cbTable->setVisible(showDatabase);

    if (m_initializing)
        return;

    m_hypothesisTest->setComment(ui.leComment->text());
}

void HypothesisTestDock::spreadsheetChanged(const QModelIndex& index) {
    auto* aspect = static_cast<AbstractAspect*>(index.internalPointer());
    Spreadsheet* spreadsheet = dynamic_cast<Spreadsheet*>(aspect);
    setColumnsComboBoxModel(spreadsheet);
    m_hypothesisTest->setDataSourceSpreadsheet(spreadsheet);
}

void HypothesisTestDock::changeCbCol2Label() {
    if ( (m_test.type & ~Test::Type::Anova) & (m_test.subtype & ~Test::SubType::TwoSampleIndependent)) {
        ui.lCol2->setText( i18n("Independent Var. 2"));
        return;
    }

    QString selected_text = ui.cbCol1->currentText();
    Column* col1 = m_hypothesisTest->dataSourceSpreadsheet()->column(selected_text);

    if (!ui.chbCategorical->isChecked() && (col1->columnMode() == AbstractColumn::Integer || col1->columnMode() == AbstractColumn::Numeric)) {
        ui.lCol2->setText( i18n("Independent Var. 2"));
        ui.chbCategorical->setChecked(false);
        ui.chbCategorical->setEnabled(true);
    } else {
        ui.lCol2->setText( i18n("Dependent Var. 1"));
        if (!ui.chbCategorical->isChecked())
            ui.chbCategorical->setEnabled(false);
        else
            ui.chbCategorical->setEnabled(true);
        ui.chbCategorical->setChecked(true);
    }
}

void HypothesisTestDock::col1IndexChanged(int index) {
    if (index < 0) return;
    changeCbCol2Label();
}


//void HypothesisTestDock::connectionChanged() {
//    if (ui.cbConnection->currentIndex() == -1) {
//        ui.lTable->hide();
//        ui.cbTable->hide();
//        return;
//    }

//    //clear the previously shown tables
//    ui.cbTable->clear();
//    ui.lTable->show();
//    ui.cbTable->show();

//    const QString& connection = ui.cbConnection->currentText();

//    //connection name was changed, determine the current connections settings
//    KConfig config(m_configPath, KConfig::SimpleConfig);
//    KConfigGroup group = config.group(connection);

//    //close and remove the previos connection, if available
//    if (m_db.isOpen()) {
//        m_db.close();
//        QSqlDatabase::removeDatabase(m_db.driverName());
//    }

//    //open the selected connection
//    QDEBUG("HypothesisTestDock: connecting to " + connection);
//    const QString& driver = group.readEntry("Driver");
//    m_db = QSqlDatabase::addDatabase(driver);

//    const QString& dbName = group.readEntry("DatabaseName");
//    if (DatabaseManagerWidget::isFileDB(driver)) {
//        if (!QFile::exists(dbName)) {
//            KMessageBox::error(this, i18n("Couldn't find the database file '%1'. Please check the connection settings.", dbName),
//                               appendRow     i18n("Connection Failed"));
//            return;
//        } else
//            m_db.setDatabaseName(dbName);
//    } else if (DatabaseManagerWidget::isODBC(driver)) {
//        if (group.readEntry("CustomConnectionEnabled", false))
//            m_db.setDatabaseName(group.readEntry("CustomConnectionString"));
//        else
//            m_db.setDatabaseName(dbName);
//    } else {
//        m_db.setDatabaseName(dbName);
//        m_db.setHostName( group.readEntry("HostName") );
//        m_db.setPort( group.readEntry("Port", 0) );
//        m_db.setUserName( group.readEntry("UserName") );
//        m_db.setPassword( group.readEntry("Password") );
//    }

//    WAIT_CURSOR;
//    if (!m_db.open()) {
//        RESET_CURSOR;
//        KMessageBox::error(this, i18n("Failed to connect to the database '%1'. Please check the connection settings.", ui.cbConnection->currentText()) +
//                                    QLatin1String("\n\n") + m_db.lastError().databaseText(),
//                                 i18n("Connection Failed"));
//        return;
//    }

//    //show all available database tables
//    if (m_db.tables().size()) {
//        for (auto table : m_db.tables())
//            ui.cbTable->addItem(QIcon::fromTheme("view-form-table"), table);
//        ui.cbTable->setCurrentIndex(0);
//    }

//    RESET_CURSOR;

//    if (m_initializing)
//        return;

//// 	m_hypothesisTest->setDataSourceConnection(connection);
//}

//void HypothesisTestDock::tableChanged() {
//    const QString& table = ui.cbTable->currentText();

//    //show all attributes of the selected table
//// 	for (const auto* col : spreadsheet->children<Column>()) {
//// 		QListWidgetItem* item = new QListWidgetItem(col->icon(), col->name());
//// 		ui.lwFields->addItem(item);
//// 	}

//    if (m_initializing)
//        return;

//// 	m_hypothesisTest->setDataSourceTable(table);
//}

////*************************************************************
////******** SLOTs for changes triggered in Matrix *********
////*************************************************************
//void HypothesisTestDock::hypothesisTestDescriptionChanged(const AbstractAspect* aspect) {
//    if (m_hypothesisTest != aspect)
//        return;

//    m_initializing = true;
//    if (aspect->name() != ui.leName->text())
//        ui.leName->setText(aspect->name());
//    else if (aspect->comment() != ui.leComment->text())
//        ui.leComment->setText(aspect->comment());

//    m_initializing = false;
//}

////*************************************************************
////******************** SETTINGS *******************************
////*************************************************************
//void HypothesisTestDock::load() {

//}

//void HypothesisTestDock::loadConfigFromTemplate(KConfig& config) {
//    Q_UNUSED(config);
//}

///*!
//    loads saved matrix properties from \c config.
// */
//void HypothesisTestDock::loadConfig(KConfig& config) {
//    Q_UNUSED(config);
//}

///*!
//    saves matrix properties to \c config.
// */
//void HypothesisTestDock::saveConfigAsTemplate(KConfig& config) {
//    Q_UNUSED(config);
//}

//TODO: Rather than inbuilt slots use own decided slots for checked rather than clicked

// for alternate hypothesis
// one_tail_1 is mu > mu0; one_tail_2 is mu < mu0; two_tail = mu != mu0;
void HypothesisTestDock::onRbH1OneTail1Toggled(bool checked) {
    if (!checked) return;
    ui.rbH0OneTail1->setChecked(true);
    m_hypothesisTest->setTailType(HypothesisTest::TailPositive);
}

void HypothesisTestDock::onRbH1OneTail2Toggled(bool checked) {
    if (!checked) return;
    ui.rbH0OneTail2->setChecked(true);
    m_hypothesisTest->setTailType(HypothesisTest::TailNegative);
}

void HypothesisTestDock::onRbH1TwoTailToggled(bool checked) {
    if (!checked) return;
    ui.rbH0TwoTail->setChecked(true);
    m_hypothesisTest->setTailType(HypothesisTest::TailTwo);
}


/**************************************Helper Functions********************************************/

void HypothesisTestDock::countPartitions(Column *column, int &np, int &total_rows) {
    total_rows = column->rowCount();
    np = 0;
    QString cell_value;
    QMap<QString, bool> discovered_categorical_var;

    AbstractColumn::ColumnMode original_col_mode = column->columnMode();
    column->setColumnMode(AbstractColumn::Text);

    for (int i = 0; i < total_rows; i++) {
        cell_value = column->textAt(i);

        if (cell_value.isEmpty()) {
            total_rows = i;
            break;
        }

        if (discovered_categorical_var[cell_value])
            continue;

        discovered_categorical_var[cell_value] = true;
        np++;
    }
    column->setColumnMode(original_col_mode);
}

void HypothesisTestDock::setColumnsComboBoxModel(Spreadsheet* spreadsheet) {
    only_values_cols.clear();
    two_categorical_cols.clear();
    more_than_two_categorical_cols.clear();

    for (auto* col : spreadsheet->children<Column>()) {
        if (col->columnMode() == AbstractColumn::Integer || col->columnMode() == AbstractColumn::Numeric)
            only_values_cols.append(col->name());
        else {
            int np = 0, n_rows = 0;
            countPartitions(col, np, n_rows);
            if (np <= 1)
                continue;
            else if (np == 2)
                two_categorical_cols.append(col->name());
            else
                more_than_two_categorical_cols.append(col->name());
        }
    }
    setColumnsComboBoxView();
    showHypothesisTest();
}

void HypothesisTestDock::setColumnsComboBoxView() {

    ui.cbCol1->clear();
    ui.cbCol2->clear();
    if (m_test.subtype & Test::SubType::TwoSampleIndependent) {
        ui.cbCol1->addItems(only_values_cols);
        ui.cbCol1->addItems(two_categorical_cols);

        ui.cbCol2->addItems(only_values_cols);
    } else if (m_test.subtype & Test::SubType::TwoSamplePaired) {
        ui.cbCol1->addItems(only_values_cols);
        ui.cbCol2->addItems(only_values_cols);
    } else if (m_test.subtype & Test::SubType::OneSample)
        ui.cbCol1->addItems(only_values_cols);
    else if (m_test.type & Test::Type::Anova) {
        ui.cbCol1->addItems(two_categorical_cols);
        ui.cbCol1->addItems(more_than_two_categorical_cols);
        ui.cbCol2->addItems(only_values_cols);
    }
}

bool HypothesisTestDock::nonEmptySelectedColumns() {
    if (ui.cbCol1->isVisible() && ui.cbCol1->count() < 1)
        return false;
    if (ui.cbCol2->isVisible() && ui.cbCol2->count() < 1)
        return false;
    return true;
}