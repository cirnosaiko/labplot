/***************************************************************************
	File                 : ImportDatasetWidget.h
	Project              : LabPlot
	Description          : import online dataset widget
	--------------------------------------------------------------------
	Copyright            : (C) 2019 Kovacs Ferencz (kferike98@gmail.com)

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


#ifndef IMPORTDATASETWIDGET_H
#define IMPORTDATASETWIDGET_H

#include "ui_importdatasetwidget.h"
#include "QWidget"
#include "QMap"


class QCompleter;
class DatasetHandler;
class DatasetModel;

class ImportDatasetWidget : public QWidget {
	Q_OBJECT

public:
	explicit ImportDatasetWidget(QWidget* parent);
	~ImportDatasetWidget() override;
	QString getSelectedDataset() const;
	void loadDatasetToProcess(DatasetHandler* datasetHandler);
	QString locateCategoryJsonFile() const;
	const QMap< QString, QMap<QString, QMap<QString, QVector<QString>>>>& getDatasetsMap();
	void setCollection(const QString&);
	void setCategory(const QString&);
	void setSubcategory(const QString&);
	void setDataset(const QString&);
	void processTest(const QString& category, const QString& subcategory, const QString& dataset, DatasetHandler* datasetHandler);
	const QString getJsonDir();

private:    
	Ui::ImportDatasetWidget ui;
	QMap< QString, QMap<QString, QMap<QString, QVector<QString>>>> m_datasetsMap;
	QString m_selectedSubcategory;
	QCompleter* m_categoryCompleter;
	QCompleter* m_datasetCompleter;
	QString m_jsonDir;
	bool m_loadingCategories;
	QString m_selectedCategory;
	QString m_selectedCollection;
	DatasetModel* m_datasetModel{nullptr};
	bool m_showDetails{false};

	QString validCollectionName(const QString& collection);
	void downloadCollectionsFile();
	void downloadCollectionFile(const QString&);
	void uploadCategoryFile();
	void uploadDatasetFile(const QString&);
	void updateDatasetCompleter();
	void updateCategoryCompleter();
	void loadDatasetCategoriesFromJson();
	void listDatasetsForSubcategory(QTreeWidgetItem* item);
	void restoreSelectedSubcategory(const QString& collection);
	void highlightLocalMetadataFiles();
	QJsonObject loadDatasetObject();
	void updateDetails();
	void showDetails(bool show);

private slots:
	void updateCategoryTree(const QString&);
	void scrollToCategoryTreeItem(const QString& rootName);
	void scrollToDatasetListItem(const QString& rootName);
	void showDatasetMetadataManager();
	void refreshCategories();
	void clearCache();
	void restoreBackup();

signals:
	void datasetSelected();
	void datasetDoubleClicked();

};


#endif // IMPORTDATASETWIDGET_H
