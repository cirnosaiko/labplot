/***************************************************************************
    File                 : LabelWidget.cc
    Project              : LabPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2008-2020 Alexander Semke (alexander.semke@web.de)
    Copyright            : (C) 2012-2017 Stefan Gerlach (stefan.gerlach@uni-konstanz.de)
    Description          : label settings widget

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
#include "LabelWidget.h"
#include "backend/worksheet/Worksheet.h"
#include "backend/worksheet/plots/cartesian/Axis.h"
#include "backend/worksheet/plots/cartesian/CartesianPlot.h"
#include "backend/worksheet/plots/cartesian/CartesianPlotLegend.h"
#include "kdefrontend/GuiTools.h"
#include "kdefrontend/dockwidgets/BaseDock.h"
#include "tools/TeXRenderer.h"

#include <QMenu>
#include <QSettings>
#include <QSplitter>
#include <QTextDocumentFragment>
#include <QWidgetAction>

#include <KConfigGroup>
#include <KSharedConfig>
#include <KCharSelect>
#include <KLocalizedString>
#ifdef HAVE_KF5_SYNTAX_HIGHLIGHTING
#include <KF5/KSyntaxHighlighting/SyntaxHighlighter>
#include <KF5/KSyntaxHighlighting/Definition>
#include <KF5/KSyntaxHighlighting/Theme>
#endif

#include <cmath>

/*!
	\class LabelWidget
 	\brief Widget for editing the properties of a TextLabel object, mostly used in an appropriate dock widget.

	In order the properties of the label to be shown, \c loadConfig() has to be called with the corresponding KConfigGroup
	(settings for a label in *Plot, Axis etc. or for an independent label on the worksheet).

	\ingroup kdefrontend
 */
LabelWidget::LabelWidget(QWidget* parent) : QWidget(parent), m_dateTimeMenu(new QMenu(this)) {
	ui.setupUi(this);

	const KConfigGroup group = KSharedConfig::openConfig()->group(QLatin1String("Settings_General"));
	m_units = (BaseDock::Units)group.readEntry("Units", (int)BaseDock::Units::Metric);
	if (m_units == BaseDock::Units::Imperial)
		m_worksheetUnit = Worksheet::Unit::Inch;

	m_dateTimeMenu->setSeparatorsCollapsible(false); //we don't want the first separator to be removed

	ui.kcbFontColor->setColor(Qt::black); // default color

	//Icons
	ui.tbFontBold->setIcon( QIcon::fromTheme(QLatin1String("format-text-bold")) );
	ui.tbFontItalic->setIcon( QIcon::fromTheme(QLatin1String("format-text-italic")) );
	ui.tbFontUnderline->setIcon( QIcon::fromTheme(QLatin1String("format-text-underline")) );
	ui.tbFontStrikeOut->setIcon( QIcon::fromTheme(QLatin1String("format-text-strikethrough")) );
	ui.tbFontSuperScript->setIcon( QIcon::fromTheme(QLatin1String("format-text-superscript")) );
	ui.tbFontSubScript->setIcon( QIcon::fromTheme(QLatin1String("format-text-subscript")) );
	ui.tbSymbols->setIcon( QIcon::fromTheme(QLatin1String("labplot-format-text-symbol")) );
	ui.tbDateTime->setIcon( QIcon::fromTheme(QLatin1String("chronometer")) );
	ui.tbTexUsed->setIcon( QIcon::fromTheme(QLatin1String("labplot-TeX-logo")) );

	ui.tbFontBold->setToolTip(i18n("Bold"));
	ui.tbFontItalic->setToolTip(i18n("Italic"));
	ui.tbFontUnderline->setToolTip(i18n("Underline"));
	ui.tbFontStrikeOut->setToolTip(i18n("Strike Out"));
	ui.tbFontSuperScript->setToolTip(i18n("Super Script"));
	ui.tbFontSubScript->setToolTip(i18n("Sub-Script"));
	ui.tbSymbols->setToolTip(i18n("Insert Symbol"));
	ui.tbDateTime->setToolTip(i18n("Insert Date/Time"));
	ui.tbTexUsed->setToolTip(i18n("Switch to TeX mode"));

	//Positioning and alignment
	ui.cbPositionX->addItem(i18n("Left"));
	ui.cbPositionX->addItem(i18n("Center"));
	ui.cbPositionX->addItem(i18n("Right"));
	ui.cbPositionX->addItem(i18n("Custom"));

	ui.cbPositionY->addItem(i18n("Top"));
	ui.cbPositionY->addItem(i18n("Center"));
	ui.cbPositionY->addItem(i18n("Bottom"));
	ui.cbPositionY->addItem(i18n("Custom"));

	QString suffix;
	if (m_units == BaseDock::Units::Metric)
		suffix = QLatin1String(" cm");
	else
		suffix = QLatin1String(" in");

	ui.sbPositionX->setSuffix(suffix);
	ui.sbPositionY->setSuffix(suffix);

	ui.cbHorizontalAlignment->addItem(i18n("Left"));
	ui.cbHorizontalAlignment->addItem(i18n("Center"));
	ui.cbHorizontalAlignment->addItem(i18n("Right"));

	ui.cbVerticalAlignment->addItem(i18n("Top"));
	ui.cbVerticalAlignment->addItem(i18n("Center"));
	ui.cbVerticalAlignment->addItem(i18n("Bottom"));

	ui.cbBorderShape->addItem(i18n("No Border"));
	ui.cbBorderShape->addItem(i18n("Rectangle"));
	ui.cbBorderShape->addItem(i18n("Ellipse"));
	ui.cbBorderShape->addItem(i18n("Round sided rectangle"));
	ui.cbBorderShape->addItem(i18n("Round corner rectangle"));
	ui.cbBorderShape->addItem(i18n("Inwards round corner rectangle"));
	ui.cbBorderShape->addItem(i18n("Dented border rectangle"));
	ui.cbBorderShape->addItem(i18n("Cuboid"));
	ui.cbBorderShape->addItem(i18n("Up Pointing rectangle"));
	ui.cbBorderShape->addItem(i18n("Down Pointing rectangle"));
	ui.cbBorderShape->addItem(i18n("Left Pointing rectangle"));
	ui.cbBorderShape->addItem(i18n("Right Pointing rectangle"));

	ui.cbBorderStyle->addItem(i18n("No line"));
	ui.cbBorderStyle->addItem(i18n("Solid line"));
	ui.cbBorderStyle->addItem(i18n("Dash line"));
	ui.cbBorderStyle->addItem(i18n("Dot line"));
	ui.cbBorderStyle->addItem(i18n("Dash dot line"));
	ui.cbBorderStyle->addItem(i18n("Dash dot dot line"));

	ui.kcbBackgroundColor->setAlphaChannelEnabled(true);
	ui.kcbBackgroundColor->setColor(QColor(0,0,0, 0)); // transparent
	ui.kcbFontColor->setAlphaChannelEnabled(true);
	ui.kcbFontColor->setColor(QColor(255,255,255, 255)); // black
	ui.kcbBorderColor->setAlphaChannelEnabled(true);
	ui.kcbBorderColor->setColor(QColor(255,255,255, 255)); // black

	//check whether the used latex compiler is available.
	//Following logic is implemented (s.a. LabelWidget::teXUsedChanged()):
	//1. in case latex was used to generate the text label in the stored project
	//and no latex is available on the target system, latex button is toggled and
	//the user still can switch to the non-latex mode.
	//2. in case the label was in the non-latex mode and no latex is available,
	//deactivate the latex button so the user cannot switch to this mode.
	m_teXEnabled = TeXRenderer::enabled();

#ifdef HAVE_KF5_SYNTAX_HIGHLIGHTING
	m_highlighter = new KSyntaxHighlighting::SyntaxHighlighter(ui.teLabel->document());
	m_highlighter->setDefinition(m_repository.definitionForName(QLatin1String("LaTeX")));
	m_highlighter->setTheme(  (palette().color(QPalette::Base).lightness() < 128)
								? m_repository.defaultTheme(KSyntaxHighlighting::Repository::DarkTheme)
								: m_repository.defaultTheme(KSyntaxHighlighting::Repository::LightTheme) );
#endif

	//SLOTS
	// text properties
	connect(ui.tbTexUsed, &QToolButton::clicked, this, &LabelWidget::teXUsedChanged );
	connect(ui.teLabel, &ResizableTextEdit::textChanged, this, &LabelWidget::textChanged);
	connect(ui.teLabel, &ResizableTextEdit::currentCharFormatChanged, this, &LabelWidget::charFormatChanged);
	connect(ui.kcbFontColor, &KColorButton::changed, this, &LabelWidget::fontColorChanged);
	connect(ui.kcbBackgroundColor, &KColorButton::changed, this, &LabelWidget::backgroundColorChanged);
	connect(ui.tbFontBold, &QToolButton::clicked, this, &LabelWidget::fontBoldChanged);
	connect(ui.tbFontItalic, &QToolButton::clicked, this, &LabelWidget::fontItalicChanged);
	connect(ui.tbFontUnderline, &QToolButton::clicked, this, &LabelWidget::fontUnderlineChanged);
	connect(ui.tbFontStrikeOut, &QToolButton::clicked, this, &LabelWidget::fontStrikeOutChanged);
	connect(ui.tbFontSuperScript, &QToolButton::clicked, this, &LabelWidget::fontSuperScriptChanged);
	connect(ui.tbFontSubScript, &QToolButton::clicked, this, &LabelWidget::fontSubScriptChanged);
	connect(ui.tbSymbols, &QToolButton::clicked, this, &LabelWidget::charMenu);
	connect(ui.tbDateTime, &QToolButton::clicked, this, &LabelWidget::dateTimeMenu);
	connect(m_dateTimeMenu, &QMenu::triggered, this, &LabelWidget::insertDateTime );
	connect(ui.kfontRequester, &KFontRequester::fontSelected, this, &LabelWidget::fontChanged);
	connect(ui.kfontRequesterTeX, &KFontRequester::fontSelected, this, &LabelWidget::teXFontChanged);
	connect(ui.sbFontSize, QOverload<int>::of(&QSpinBox::valueChanged), this, &LabelWidget::fontSizeChanged);

	// geometry
	connect( ui.cbPositionX, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &LabelWidget::positionXChanged);
	connect( ui.cbPositionY, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &LabelWidget::positionYChanged);
	connect( ui.sbPositionX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LabelWidget::customPositionXChanged);
	connect( ui.sbPositionY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LabelWidget::customPositionYChanged);
	connect( ui.cbHorizontalAlignment, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &LabelWidget::horizontalAlignmentChanged);
	connect( ui.cbVerticalAlignment, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &LabelWidget::verticalAlignmentChanged);
	connect( ui.sbRotation, QOverload<int>::of(&QSpinBox::valueChanged), this, &LabelWidget::rotationChanged);
	connect( ui.sbOffsetX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LabelWidget::offsetXChanged);
	connect( ui.sbOffsetY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LabelWidget::offsetYChanged);

	connect( ui.chbVisible, &QCheckBox::clicked, this, &LabelWidget::visibilityChanged);

	//Border
	connect(ui.cbBorderShape, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LabelWidget::borderShapeChanged);
	connect(ui.cbBorderStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LabelWidget::borderStyleChanged);
	connect(ui.kcbBorderColor, &KColorButton::changed, this, &LabelWidget::borderColorChanged);
	connect(ui.sbBorderWidth, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LabelWidget::borderWidthChanged);
	connect(ui.sbBorderOpacity, QOverload<int>::of(&QSpinBox::valueChanged), this, &LabelWidget::borderOpacityChanged);

	//TODO: https://bugreports.qt.io/browse/QTBUG-25420
	ui.tbFontUnderline->hide();
	ui.tbFontStrikeOut->hide();
}

void LabelWidget::setLabels(QList<TextLabel*> labels) {
	m_labelsList = labels;
	m_label = labels.first();

	ui.lOffsetX->hide();
	ui.lOffsetY->hide();

	ui.sbOffsetX->hide();
	ui.sbOffsetY->hide();

	this->load();
	initConnections();
	updateBackground();
}

void LabelWidget::setAxes(QList<Axis*> axes) {
	m_labelsList.clear();
	for (auto* axis : axes) {
		m_labelsList.append(axis->title());
		connect(axis, &Axis::titleOffsetXChanged, this, &LabelWidget::labelOffsetxChanged);
		connect(axis, &Axis::titleOffsetYChanged, this, &LabelWidget::labelOffsetyChanged );
		connect(axis->title(), &TextLabel::rotationAngleChanged, this, &LabelWidget::labelRotationAngleChanged );
	}

	SET_NUMBER_LOCALE
	ui.sbPositionX->setLocale(numberLocale);
	ui.sbPositionY->setLocale(numberLocale);
	ui.sbOffsetX->setLocale(numberLocale);
	ui.sbOffsetY->setLocale(numberLocale);
	ui.sbBorderWidth->setLocale(numberLocale);

	m_axesList = axes;
	m_label = m_labelsList.first();

	this->load();
	initConnections();
	updateBackground();
}

/*!
 * this function keeps the background color of the TextEdit in LabelWidget in sync with
 * the background color of the parent aspect. This is to avoid the situations where the
 * text wouldn't be readable anymore if the foreground color is close or equal to the
 * background color of TextEdit - e.g., a label with white foreground color on a dark worksheet
 * and with white background color in TextEdit (desktop default color).
 *
 * Called if the background color of the parent aspect has changed.
 */
void LabelWidget::updateBackground() const {
	if (ui.tbTexUsed->isChecked())
		return;

	QColor color;
	AspectType type = m_label->parentAspect()->type();
	if (type == AspectType::Worksheet)
		color = static_cast<const Worksheet*>(m_label->parentAspect())->backgroundFirstColor();
	else if (type == AspectType::CartesianPlot)
		color = static_cast<CartesianPlot*>(m_label->parentAspect())->plotArea()->backgroundFirstColor();
	else if (type == AspectType::CartesianPlotLegend)
		color = static_cast<const CartesianPlotLegend*>(m_label->parentAspect())->backgroundFirstColor();
	if (type == AspectType::Axis)
		color = static_cast<CartesianPlot*>(m_label->parentAspect()->parentAspect())->plotArea()->backgroundFirstColor();

	QPalette p = ui.teLabel->palette();
	p.setColor(QPalette::Base, color);
	ui.teLabel->setPalette(p);
}

void LabelWidget::initConnections() const {
	connect(m_label, &TextLabel::textWrapperChanged, this, &LabelWidget::labelTextWrapperChanged);
	connect(m_label, &TextLabel::teXImageUpdated, this, &LabelWidget::labelTeXImageUpdated);
	connect(m_label, &TextLabel::teXFontChanged, this, &LabelWidget::labelTeXFontChanged);
	connect(m_label, &TextLabel::fontColorChanged, this, &LabelWidget::labelFontColorChanged);
	connect(m_label, &TextLabel::backgroundColorChanged, this, &LabelWidget::labelBackgroundColorChanged);
	connect(m_label, &TextLabel::positionChanged, this, &LabelWidget::labelPositionChanged);
	connect(m_label, &TextLabel::horizontalAlignmentChanged, this, &LabelWidget::labelHorizontalAlignmentChanged);
	connect(m_label, &TextLabel::verticalAlignmentChanged, this, &LabelWidget::labelVerticalAlignmentChanged);
	connect(m_label, &TextLabel::rotationAngleChanged, this, &LabelWidget::labelRotationAngleChanged);
	connect(m_label, &TextLabel::borderShapeChanged, this, &LabelWidget::labelBorderShapeChanged);
	connect(m_label, &TextLabel::borderPenChanged, this, &LabelWidget::labelBorderPenChanged);
	connect(m_label, &TextLabel::borderOpacityChanged, this, &LabelWidget::labelBorderOpacityChanged);
	connect(m_label, &TextLabel::visibleChanged, this, &LabelWidget::labelVisibleChanged);

	AspectType type = m_label->parentAspect()->type();
	if (type == AspectType::Worksheet) {
		auto* worksheet = static_cast<const Worksheet*>(m_label->parentAspect());
		connect(worksheet, &Worksheet::backgroundFirstColorChanged, this, &LabelWidget::updateBackground);
	}else if (type == AspectType::CartesianPlot) {
		auto* plotArea = static_cast<CartesianPlot*>(m_label->parentAspect())->plotArea();
		connect(plotArea, &PlotArea::backgroundFirstColorChanged, this, &LabelWidget::updateBackground);
	} else if (type == AspectType::CartesianPlotLegend) {
		auto* legend = static_cast<const CartesianPlotLegend*>(m_label->parentAspect());
		connect(legend, &CartesianPlotLegend::backgroundFirstColorChanged, this, &LabelWidget::updateBackground);
	} else if (type == AspectType::Axis) {
		auto* plotArea = static_cast<CartesianPlot*>(m_label->parentAspect()->parentAspect())->plotArea();
		connect(plotArea, &PlotArea::backgroundFirstColorChanged, this, &LabelWidget::updateBackground);
	}
}

/*!
 * enables/disables the "fixed label"-mode, used when displaying
 * the properties of axis' title label.
 * In this mode, in the "geometry"-part only the offset (offset to the axis)
 * and the rotation of the label are available.
 */
void LabelWidget::setFixedLabelMode(const bool b) {
	ui.lPositionX->setVisible(!b);
	ui.cbPositionX->setVisible(!b);
	ui.sbPositionX->setVisible(!b);
	ui.lPositionY->setVisible(!b);
	ui.cbPositionY->setVisible(!b);
	ui.sbPositionY->setVisible(!b);
	ui.lHorizontalAlignment->setVisible(!b);
	ui.cbHorizontalAlignment->setVisible(!b);
	ui.lVerticalAlignment->setVisible(!b);
	ui.cbVerticalAlignment->setVisible(!b);
	ui.lOffsetX->setVisible(b);
	ui.lOffsetY->setVisible(b);
	ui.sbOffsetX->setVisible(b);
	ui.sbOffsetY->setVisible(b);
}

/*!
 * enables/disables all geometry relevant widgets.
 * Used when displaying legend's title label.
 */
void LabelWidget::setGeometryAvailable(const bool b) {
	ui.lGeometry->setVisible(b);
	ui.lPositionX->setVisible(b);
	ui.cbPositionX->setVisible(b);
	ui.sbPositionX->setVisible(b);
	ui.lPositionY->setVisible(b);
	ui.cbPositionY->setVisible(b);
	ui.sbPositionY->setVisible(b);
	ui.lHorizontalAlignment->setVisible(b);
	ui.cbHorizontalAlignment->setVisible(b);
	ui.lVerticalAlignment->setVisible(b);
	ui.cbVerticalAlignment->setVisible(b);
	ui.lOffsetX->setVisible(b);
	ui.lOffsetY->setVisible(b);
	ui.sbOffsetX->setVisible(b);
	ui.sbOffsetY->setVisible(b);
	ui.lRotation->setVisible(b);
	ui.sbRotation->setVisible(b);
}

/*!
 * enables/disables all border relevant widgets.
 * Used when displaying legend's title label.
 */
void LabelWidget::setBorderAvailable(bool b) {
	ui.lBorder->setVisible(b);
	ui.lBorderShape->setVisible(b);
	ui.cbBorderShape->setVisible(b);
	ui.lBorderStyle->setVisible(b);
	ui.cbBorderStyle->setVisible(b);
	ui.lBorderColor->setVisible(b);
	ui.kcbBorderColor->setVisible(b);
	ui.lBorderWidth->setVisible(b);
	ui.sbBorderWidth->setVisible(b);
	ui.lBorderOpacity->setVisible(b);
	ui.sbBorderOpacity->setVisible(b);
}

void LabelWidget::updateUnits() {
	const KConfigGroup group = KSharedConfig::openConfig()->group(QLatin1String("Settings_General"));
	BaseDock::Units units = (BaseDock::Units)group.readEntry("Units", (int)BaseDock::Units::Metric);
	if (units == m_units)
		return;

	m_units = units;
	Lock lock(m_initializing);
	QString suffix;
	if (m_units == BaseDock::Units::Metric) {
		//convert from imperial to metric
		m_worksheetUnit = Worksheet::Unit::Centimeter;
		suffix = QLatin1String(" cm");
		ui.sbPositionX->setValue(ui.sbPositionX->value()*2.54);
		ui.sbPositionY->setValue(ui.sbPositionX->value()*2.54);
	} else {
		//convert from metric to imperial
		m_worksheetUnit = Worksheet::Unit::Inch;
		suffix = QLatin1String(" in");
		ui.sbPositionX->setValue(ui.sbPositionX->value()/2.54);
		ui.sbPositionY->setValue(ui.sbPositionY->value()/2.54);
	}

	ui.sbPositionX->setSuffix(suffix);
	ui.sbPositionY->setSuffix(suffix);
}

//**********************************************************
//****** SLOTs for changes triggered in LabelWidget ********
//**********************************************************

// text formatting slots

void LabelWidget::textChanged() {
	if (m_initializing)
		return;

	const Lock lock(m_initializing);

	if (ui.tbTexUsed->isChecked()) {
		QString text = ui.teLabel->toPlainText();
		TextLabel::TextWrapper wrapper(text, true);

		for (auto* label : m_labelsList)
			label->setText(wrapper);
	} else {
		//save an empty string instead of a html-string with empty body,
		//if no text available in QTextEdit
		QString text;
		if (!ui.teLabel->toPlainText().isEmpty()) {
			//if the current label text is empty, set the color first
			if (m_label->text().text.isEmpty()) {
				ui.teLabel->selectAll();
				ui.teLabel->setTextColor(m_label->fontColor());
			}

			text = ui.teLabel->toHtml();
		}

		TextLabel::TextWrapper wrapper(text, false, true);
		for (auto* label : m_labelsList)
			label->setText(wrapper);
	}

	//background color gets lost on every text change...
	updateBackground();
}

/*!
 * \brief LabelWidget::charFormatChanged
 * \param format
 * Used to update the colors, font,... in the color font widgets to show the style of the selected text
 */
void LabelWidget::charFormatChanged(const QTextCharFormat& format) {
	if (m_initializing)
		return;

	if (ui.tbTexUsed->isChecked())
		return;

	const Lock lock(m_initializing);

	// update button state
	ui.tbFontBold->setChecked(ui.teLabel->fontWeight() == QFont::Bold);
	ui.tbFontItalic->setChecked(ui.teLabel->fontItalic());
	ui.tbFontUnderline->setChecked(ui.teLabel->fontUnderline());
	ui.tbFontStrikeOut->setChecked(format.fontStrikeOut());
	ui.tbFontSuperScript->setChecked(format.verticalAlignment() == QTextCharFormat::AlignSuperScript);
	ui.tbFontSubScript->setChecked(format.verticalAlignment() == QTextCharFormat::AlignSubScript);

	//font and colors
	if (format.foreground().color().isValid())
		ui.kcbFontColor->setColor(format.foreground().color());
	else
		ui.kcbFontColor->setColor(m_label->fontColor());

	if (format.background().color().isValid())
		ui.kcbBackgroundColor->setColor(format.background().color());
	else
		ui.kcbBackgroundColor->setColor(m_label->backgroundColor());

	ui.kfontRequester->setFont(format.font());
}

void LabelWidget::teXUsedChanged(bool checked) {
	//hide text editing elements if TeX-option is used
	ui.tbFontBold->setVisible(!checked);
	ui.tbFontItalic->setVisible(!checked);

	//TODO: https://bugreports.qt.io/browse/QTBUG-25420
// 	ui.tbFontUnderline->setVisible(!checked);
// 	ui.tbFontStrikeOut->setVisible(!checked);

	ui.tbFontSubScript->setVisible(!checked);
	ui.tbFontSuperScript->setVisible(!checked);
	ui.tbSymbols->setVisible(!checked);

	ui.lFont->setVisible(!checked);
	ui.kfontRequester->setVisible(!checked);

	//TODO:
	//for normal text we need to hide the background color because of QTBUG-25420
	ui.kcbBackgroundColor->setVisible(checked);
	ui.lBackgroundColor->setVisible(checked);

	if (checked) {
		ui.tbTexUsed->setToolTip(i18n("Switch to TeX mode"));

		//reset all applied formattings when switching from html to tex mode
		QTextCursor cursor = ui.teLabel->textCursor();
		int position = cursor.position();
		ui.teLabel->selectAll();
		QTextCharFormat format;
		ui.teLabel->setCurrentCharFormat(format);
		cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, position);
		ui.teLabel->setTextCursor(cursor);

#ifdef HAVE_KF5_SYNTAX_HIGHLIGHTING
		m_highlighter->setDocument(ui.teLabel->document());
#endif
		KConfigGroup conf(KSharedConfig::openConfig(), QLatin1String("Settings_Worksheet"));
		QString engine = conf.readEntry(QLatin1String("LaTeXEngine"), "");
		if (engine == QLatin1String("xelatex") || engine == QLatin1String("lualatex")) {
			ui.lFontTeX->setVisible(true);
			ui.kfontRequesterTeX->setVisible(true);
			ui.lFontSize->setVisible(false);
			ui.sbFontSize->setVisible(false);
		} else {
			ui.lFontTeX->setVisible(false);
			ui.kfontRequesterTeX->setVisible(false);
			ui.lFontSize->setVisible(true);
			ui.sbFontSize->setVisible(true);
		}

		//update TeX colors
		ui.kcbFontColor->setColor(m_label->fontColor());
		ui.kcbBackgroundColor->setColor(m_label->backgroundColor());
	} else {
		ui.tbTexUsed->setToolTip(i18n("Switch to text mode"));

#ifdef HAVE_KF5_SYNTAX_HIGHLIGHTING
		m_highlighter->setDocument(nullptr);
#endif
		ui.lFontTeX->setVisible(false);
		ui.kfontRequesterTeX->setVisible(false);
		ui.lFontSize->setVisible(false);
		ui.sbFontSize->setVisible(false);

		//when switching to the text mode, set the background color to white just for the case the latex code provided by the user
		//in the TeX-mode is not valid and the background was set to red (s.a. LabelWidget::labelTeXImageUpdated())
		ui.teLabel->setStyleSheet(QString());
	}

	//no latex is available and the user switched to the text mode,
	//deactivate the button since it shouldn't be possible anymore to switch to the TeX-mode
	if (!m_teXEnabled && !checked) {
		ui.tbTexUsed->setEnabled(false);
		ui.tbTexUsed->setToolTip(i18n("LaTeX typesetting not possible. Please check the settings."));
	} else
		ui.tbTexUsed->setEnabled(true);

	if (m_initializing)
		return;

	QString text = checked ? ui.teLabel->toPlainText() : ui.teLabel->toHtml();
	TextLabel::TextWrapper wrapper(text, checked);
	for (auto* label : m_labelsList)
		label->setText(wrapper);
}

void LabelWidget::fontColorChanged(const QColor& color) {
	if (m_initializing)
		return;

	QTextCursor c = ui.teLabel->textCursor();
	if (c.selectedText().isEmpty())
		ui.teLabel->selectAll();
	ui.teLabel->setTextColor(color);
}

void LabelWidget::backgroundColorChanged(const QColor& color) {
	if (m_initializing)
		return;

	if (!m_teXEnabled && m_label->text().teXUsed) {
		QTextCursor c = ui.teLabel->textCursor();
		if (c.selectedText().isEmpty())
			ui.teLabel->selectAll();

		ui.teLabel->setTextBackgroundColor(color);
	} else {
		// Latex text does not support html code. For this the backgroundColor variable is used
		// Only single color background is supported
		for (auto* label : m_labelsList)
			label->setBackgroundColor(color);
	}
}

void LabelWidget::fontSizeChanged(int value) {
	if (m_initializing)
		return;

	QFont font = m_label->teXFont();
	font.setPointSize(value);
	for (auto* label : m_labelsList)
		label->setTeXFont(font);
}

void LabelWidget::fontBoldChanged(bool checked) {
	if (m_initializing)
		return;

	QTextCursor c = ui.teLabel->textCursor();
	if (c.selectedText().isEmpty())
		ui.teLabel->selectAll();

	if (checked)
		ui.teLabel->setFontWeight(QFont::Bold);
	else
		ui.teLabel->setFontWeight(QFont::Normal);
}

void LabelWidget::fontItalicChanged(bool checked) {
	if (m_initializing)
		return;

	QTextCursor c = ui.teLabel->textCursor();
	if (c.selectedText().isEmpty())
		ui.teLabel->selectAll();

	ui.teLabel->setFontItalic(checked);
}

void LabelWidget::fontUnderlineChanged(bool checked) {
	if (m_initializing)
		return;

	QTextCursor c = ui.teLabel->textCursor();
	if (c.selectedText().isEmpty())
		ui.teLabel->selectAll();

	ui.teLabel->setFontUnderline(checked);
}

void LabelWidget::fontStrikeOutChanged(bool checked) {
	if (m_initializing)
		return;

	QTextCursor c = ui.teLabel->textCursor();
	if (c.selectedText().isEmpty())
		ui.teLabel->selectAll();

	QTextCharFormat format = ui.teLabel->currentCharFormat();
	format.setFontStrikeOut(checked);
	ui.teLabel->setCurrentCharFormat(format);
}

void LabelWidget::fontSuperScriptChanged(bool checked) {
	if (m_initializing)
		return;

	QTextCursor c = ui.teLabel->textCursor();
	if (c.selectedText().isEmpty())
		ui.teLabel->selectAll();

	QTextCharFormat format = ui.teLabel->currentCharFormat();
	if (checked)
		format.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
	else
		format.setVerticalAlignment(QTextCharFormat::AlignNormal);

	ui.teLabel->setCurrentCharFormat(format);
}

void LabelWidget::fontSubScriptChanged(bool checked) {
	if (m_initializing)
		return;

	QTextCursor c = ui.teLabel->textCursor();
	if (c.selectedText().isEmpty())
		ui.teLabel->selectAll();

	QTextCharFormat format = ui.teLabel->currentCharFormat();
	if (checked)
		format.setVerticalAlignment(QTextCharFormat::AlignSubScript);
	else
		format.setVerticalAlignment(QTextCharFormat::AlignNormal);

	ui.teLabel->setCurrentCharFormat(format);
}

void LabelWidget::fontChanged(const QFont& font) {
	if (m_initializing)
		return;

	QTextCursor c = ui.teLabel->textCursor();
	if (c.selectedText().isEmpty())
		ui.teLabel->selectAll();

	// use format instead of using ui.teLabel->setFontFamily(font.family());
	// because this calls after every command textChanged() which is inefficient
	QTextCharFormat format;
	format.setFontFamily(font.family());
	format.setFontPointSize(font.pointSize());
	format.setFontItalic(font.italic());
	format.setFontWeight(font.weight());
	if (font.underline())
		format.setUnderlineStyle(QTextCharFormat::UnderlineStyle::SingleUnderline);
	if (font.strikeOut()) // anytime true. don't know why
		format.setFontStrikeOut(font.strikeOut());
	ui.teLabel->mergeCurrentCharFormat(format);
}

void LabelWidget::teXFontChanged(const QFont& font) {
	if (m_initializing)
		return;

	for (auto* label : m_labelsList)
		label->setTeXFont(font);
}

void LabelWidget::charMenu() {
	QMenu menu;
	KCharSelect selection(this, nullptr, KCharSelect::SearchLine | KCharSelect::CharacterTable | KCharSelect::BlockCombos | KCharSelect::HistoryButtons);
	QFont font = ui.teLabel->currentFont();
	// use the system default size, otherwise the symbols might be hard to read
	// if the current label font size is too small
	font.setPointSize(QFont().pointSize());
	selection.setCurrentFont(font);
	connect(&selection, &KCharSelect::charSelected, this, &LabelWidget::insertChar);
	connect(&selection, &KCharSelect::charSelected, &menu, &LabelWidget::close);

	auto* widgetAction = new QWidgetAction(this);
	widgetAction->setDefaultWidget(&selection);
	menu.addAction(widgetAction);

	QPoint pos(-menu.sizeHint().width()+ui.tbSymbols->width(),-menu.sizeHint().height());
	menu.exec(ui.tbSymbols->mapToGlobal(pos));
}

void LabelWidget::insertChar(QChar c) {
	ui.teLabel->insertPlainText(QString(c));
}

void LabelWidget::dateTimeMenu() {
	m_dateTimeMenu->clear();

	const QString configPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
	const QString configFile = configPath + QLatin1String("/klanguageoverridesrc");
	if (!QFile::exists(configFile)) {
		QDate date = QDate::currentDate();
		m_dateTimeMenu->addSeparator()->setText(i18n("Date"));
		m_dateTimeMenu->addAction( date.toString(Qt::TextDate) );
		m_dateTimeMenu->addAction( date.toString(Qt::ISODate) );
		m_dateTimeMenu->addAction( date.toString(Qt::SystemLocaleShortDate) );
		m_dateTimeMenu->addAction( date.toString(Qt::SystemLocaleLongDate) );
		m_dateTimeMenu->addAction( date.toString(Qt::RFC2822Date) );

		QDateTime time = QDateTime::currentDateTime();
		m_dateTimeMenu->addSeparator()->setText(i18n("Date and Time"));
		m_dateTimeMenu->addAction( time.toString(Qt::TextDate) );
		m_dateTimeMenu->addAction( time.toString(Qt::ISODate) );
		m_dateTimeMenu->addAction( time.toString(Qt::SystemLocaleShortDate) );
		m_dateTimeMenu->addAction( time.toString(Qt::SystemLocaleLongDate) );
		m_dateTimeMenu->addAction( time.toString(Qt::RFC2822Date) );
	} else {
		//application language was changed:
		//determine the currently used language and use QLocale::toString()
		//to get the strings translated into the currently used language
		//TODO: why not use QLocale() ?
		QSettings settings (configFile, QSettings::IniFormat);
		settings.beginGroup(QLatin1String("Language"));
		QByteArray languageCode;
		languageCode = settings.value(qAppName(), languageCode).toByteArray();
		QLocale locale(QString::fromLatin1(languageCode.data()));

		QDate date = QDate::currentDate();
		m_dateTimeMenu->addSeparator()->setText(i18n("Date"));
		m_dateTimeMenu->addAction( locale.toString(date, QLatin1String("ddd MMM d yyyy")) ); //Qt::TextDate
		m_dateTimeMenu->addAction( locale.toString(date, QLatin1String("yyyy-MM-dd")) ); //Qt::ISODate
		m_dateTimeMenu->addAction( locale.system().toString(date, QLocale::ShortFormat) ); //Qt::SystemLocaleShortDate
		//no LongFormat here since it would contain strings in system's language which (potentially) is not the current application language
		m_dateTimeMenu->addAction( locale.toString(date, QLatin1String("dd MMM yyyy")) ); //Qt::RFC2822Date

		QDateTime time = QDateTime::currentDateTime();
		m_dateTimeMenu->addSeparator()->setText(i18n("Date and Time"));
		m_dateTimeMenu->addAction( locale.toString(time, QLatin1String("ddd MMM d hh:mm:ss yyyy")) ); //Qt::TextDate
		m_dateTimeMenu->addAction( locale.toString(time, QLatin1String("yyyy-MM-ddTHH:mm:ss")) ); //Qt::ISODate
		m_dateTimeMenu->addAction( locale.system().toString(time, QLocale::ShortFormat) ); //Qt::SystemLocaleShortDate
		//no LongFormat here since it would contain strings in system's language which (potentially) is not the current application language

		//TODO: RFC2822 requires time zone but Qt QLocale::toString() seems to ignore TZD (time zone designator) completely,
		//which works correctly with QDateTime::toString()
		m_dateTimeMenu->addAction( locale.toString(time, QLatin1String("dd MMM yyyy hh:mm:ss")) ); //Qt::RFC2822Date
	}

	m_dateTimeMenu->exec( mapToGlobal(ui.tbDateTime->rect().bottomLeft()));
}

void LabelWidget::insertDateTime(QAction* action) {
	ui.teLabel->insertPlainText( action->text().remove('&') );
}

// geometry slots

/*!
    called when label's current horizontal position relative to its parent (left, center, right, custom ) is changed.
*/
void LabelWidget::positionXChanged(int index) {
	//Enable/disable the spinbox for the x- oordinates if the "custom position"-item is selected/deselected
	if (index == ui.cbPositionX->count()-1 )
		ui.sbPositionX->setEnabled(true);
	else
		ui.sbPositionX->setEnabled(false);

	if (m_initializing)
		return;

	auto horPos = TextLabel::HorizontalPosition(index);
	for (auto* label : m_labelsList) {
		auto position = label->position();
		position.horizontalPosition = horPos;
		label->setPosition(position);
	}
}

/*!
    called when label's current horizontal position relative to its parent (top, center, bottom, custom ) is changed.
*/
void LabelWidget::positionYChanged(int index) {
	//Enable/disable the spinbox for the y-coordinates if the "custom position"-item is selected/deselected
	if (index == ui.cbPositionY->count()-1 )
		ui.sbPositionY->setEnabled(true);
	else
		ui.sbPositionY->setEnabled(false);

	if (m_initializing)
		return;

	auto verPos = TextLabel::VerticalPosition(index);
	for (auto* label : m_labelsList) {
		auto position = label->position();
		position.verticalPosition = verPos;
		label->setPosition(position);
	}
}

void LabelWidget::customPositionXChanged(double value) {
	if (m_initializing)
		return;

	double x = Worksheet::convertToSceneUnits(value, m_worksheetUnit);
	for (auto* label : m_labelsList) {
		auto position = label->position();
		position.point.setX(x);
		label->setPosition(position);
	}
}

void LabelWidget::customPositionYChanged(double value) {
	if (m_initializing)
		return;

	double y = Worksheet::convertToSceneUnits(value, m_worksheetUnit);
	for (auto* label : m_labelsList) {
		auto position = label->position();
		position.point.setY(y);
		label->setPosition(position);
	}
}

void LabelWidget::horizontalAlignmentChanged(int index) {
	if (m_initializing)
		return;

	for (auto* label : m_labelsList)
		label->setHorizontalAlignment(TextLabel::HorizontalAlignment(index));
}

void LabelWidget::verticalAlignmentChanged(int index) {
	if (m_initializing)
		return;

	for (auto* label : m_labelsList)
		label->setVerticalAlignment(TextLabel::VerticalAlignment(index));
}

void LabelWidget::rotationChanged(int value) {
	if (m_initializing)
		return;

	for (auto* label : m_labelsList)
		label->setRotationAngle(value);
}

void LabelWidget::offsetXChanged(double value) {
	if (m_initializing)
		return;

	for (auto* axis : m_axesList)
		axis->setTitleOffsetX( Worksheet::convertToSceneUnits(value, Worksheet::Unit::Point) );
}

void LabelWidget::offsetYChanged(double value) {
	if (m_initializing)
		return;

	for (auto* axis : m_axesList)
		axis->setTitleOffsetY( Worksheet::convertToSceneUnits(value, Worksheet::Unit::Point) );
}

void LabelWidget::visibilityChanged(bool state) {
	if (m_initializing)
		return;

	for (auto* label : m_labelsList)
		label->setVisible(state);
}

//border
void LabelWidget::borderShapeChanged(int index) {
	auto shape = (TextLabel::BorderShape)index;
	bool b = (shape != TextLabel::BorderShape::NoBorder);
	ui.lBorderStyle->setVisible(b);
	ui.cbBorderStyle->setVisible(b);
	ui.lBorderWidth->setVisible(b);
	ui.sbBorderWidth->setVisible(b);
	ui.lBorderColor->setVisible(b);
	ui.kcbBorderColor->setVisible(b);
	ui.lBorderOpacity->setVisible(b);
	ui.sbBorderOpacity->setVisible(b);

	if (m_initializing)
		return;

	for (auto* label : m_labelsList)
		label->setBorderShape(shape);
}

void LabelWidget::borderStyleChanged(int index) {
	if (m_initializing)
		return;

	auto penStyle = Qt::PenStyle(index);
	QPen pen;
	for (auto* label : m_labelsList) {
		pen = label->borderPen();
		pen.setStyle(penStyle);
		label->setBorderPen(pen);
	}
}

void LabelWidget::borderColorChanged(const QColor& color) {
	if (m_initializing)
		return;

	QPen pen;
	for (auto* label : m_labelsList) {
		pen = label->borderPen();
		pen.setColor(color);
		label->setBorderPen(pen);
	}

	m_initializing = true;
	GuiTools::updatePenStyles(ui.cbBorderStyle, color);
	m_initializing = false;
}

void LabelWidget::borderWidthChanged(double value) {
	if (m_initializing)
		return;

	QPen pen;
	for (auto* label : m_labelsList) {
		pen = label->borderPen();
		pen.setWidthF( Worksheet::convertToSceneUnits(value, Worksheet::Unit::Point) );
		label->setBorderPen(pen);
	}
}

void LabelWidget::borderOpacityChanged(int value) {
	if (m_initializing)
		return;

	qreal opacity = (float)value/100.;
	for (auto* label : m_labelsList)
		label->setBorderOpacity(opacity);
}

//*********************************************************
//****** SLOTs for changes triggered in TextLabel *********
//*********************************************************
void LabelWidget::labelTextWrapperChanged(const TextLabel::TextWrapper& text) {
	if (m_initializing)return;
	const Lock lock(m_initializing);

	//save and restore the current cursor position after changing the text
	QTextCursor cursor = ui.teLabel->textCursor();
	int position = cursor.position();
	if (text.teXUsed)
		ui.teLabel->setText(text.text);
	else
		ui.teLabel->setHtml(text.text);
	cursor.movePosition(QTextCursor::Start);
	cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, position);
	ui.teLabel->setTextCursor(cursor);

	ui.tbTexUsed->setChecked(text.teXUsed);
	this->teXUsedChanged(text.teXUsed);
}

/*!
 * \brief Highlights the text field red if wrong latex syntax was used (null image was produced)
 * or something else went wrong during rendering (\sa ExpressionTextEdit::validateExpression())
 */
void LabelWidget::labelTeXImageUpdated(bool valid) {
	if (!valid) {
		if (ui.teLabel->styleSheet().isEmpty())
			ui.teLabel->setStyleSheet(QLatin1String("QTextEdit{background: red;}"));
	} else
		ui.teLabel->setStyleSheet(QString());
}

void LabelWidget::labelTeXFontChanged(const QFont& font) {
	m_initializing = true;
	ui.kfontRequesterTeX->setFont(font);
	ui.sbFontSize->setValue(font.pointSize());
	m_initializing = false;
}

void LabelWidget::labelFontColorChanged(const QColor color) {
	// this function is only called when the theme is changed. Otherwise the color
	// is directly in the html text.
	// when the theme changes, the hole text should change color regardless of the color it has
	m_initializing = true;
	ui.kcbFontColor->setColor(color);
	ui.teLabel->selectAll();
	ui.teLabel->setTextColor(color);
	m_initializing = false;
}

void LabelWidget::labelPositionChanged(const TextLabel::PositionWrapper& position) {
	m_initializing = true;
	ui.sbPositionX->setValue( Worksheet::convertFromSceneUnits(position.point.x(), m_worksheetUnit) );
	ui.sbPositionY->setValue( Worksheet::convertFromSceneUnits(position.point.y(), m_worksheetUnit) );
	ui.cbPositionX->setCurrentIndex( static_cast<int>(position.horizontalPosition) );
	ui.cbPositionY->setCurrentIndex( static_cast<int>(position.verticalPosition) );
	m_initializing = false;
}

void LabelWidget::labelBackgroundColorChanged(const QColor color) {
	m_initializing = true;
	ui.kcbBackgroundColor->setColor(color);
	m_initializing = false;
}

void LabelWidget::labelHorizontalAlignmentChanged(TextLabel::HorizontalAlignment index) {
	m_initializing = true;
	ui.cbHorizontalAlignment->setCurrentIndex(static_cast<int>(index));
	m_initializing = false;
}

void LabelWidget::labelVerticalAlignmentChanged(TextLabel::VerticalAlignment index) {
	m_initializing = true;
	ui.cbVerticalAlignment->setCurrentIndex(static_cast<int>(index));
	m_initializing = false;
}

void LabelWidget::labelOffsetxChanged(qreal offset) {
	m_initializing = true;
	ui.sbOffsetX->setValue(Worksheet::convertFromSceneUnits(offset, Worksheet::Unit::Point));
	m_initializing = false;
}

void LabelWidget::labelOffsetyChanged(qreal offset) {
	m_initializing = true;
	ui.sbOffsetY->setValue(Worksheet::convertFromSceneUnits(offset, Worksheet::Unit::Point));
	m_initializing = false;
}

void LabelWidget::labelRotationAngleChanged(qreal angle) {
	m_initializing = true;
	ui.sbRotation->setValue(angle);
	m_initializing = false;
}

void LabelWidget::labelVisibleChanged(bool on) {
	m_initializing = true;
	ui.chbVisible->setChecked(on);
	m_initializing = false;
}

//border
void LabelWidget::labelBorderShapeChanged(TextLabel::BorderShape shape) {
	m_initializing = true;
	ui.cbBorderShape->setCurrentIndex(static_cast<int>(shape));
	m_initializing = false;
}

void LabelWidget::labelBorderPenChanged(const QPen& pen) {
	m_initializing = true;
	if (ui.cbBorderStyle->currentIndex() != pen.style())
		ui.cbBorderStyle->setCurrentIndex(pen.style());
	if (ui.kcbBorderColor->color() != pen.color())
		ui.kcbBorderColor->setColor(pen.color());
	if (ui.sbBorderWidth->value() != pen.widthF())
		ui.sbBorderWidth->setValue(Worksheet::convertFromSceneUnits(pen.widthF(), Worksheet::Unit::Point));
	m_initializing = false;
}

void LabelWidget::labelBorderOpacityChanged(float value) {
	m_initializing = true;
	float v = (float)value*100.;
	ui.sbBorderOpacity->setValue(v);
	m_initializing = false;
}

//**********************************************************
//******************** SETTINGS ****************************
//**********************************************************
void LabelWidget::load() {
	if (!m_label)
		return;

	m_initializing = true;

	ui.chbVisible->setChecked(m_label->isVisible());

	//Text/TeX
	ui.tbTexUsed->setChecked( (bool) m_label->text().teXUsed );
	if (m_label->text().teXUsed)
		ui.teLabel->setText(m_label->text().text);
	else {
		ui.teLabel->setHtml(m_label->text().text);
		ui.teLabel->selectAll(); // must be done to retrieve font
		ui.kfontRequester->setFont(ui.teLabel->currentFont());
	}

	// if the text is empty yet, user LabelWidget::fontColor(),
	//extract the color from the html formatted text otherwise
	 if (!m_label->text().text.isEmpty()) {
		QTextCharFormat format = ui.teLabel->currentCharFormat();
		ui.kcbFontColor->setColor(format.foreground().color());
		//ui.kcbBackgroundColor->setColor(format.background().color());
	 } else
		ui.kcbFontColor->setColor(m_label->fontColor());

	//used for latex text only
	ui.kcbBackgroundColor->setColor(m_label->backgroundColor());

	this->teXUsedChanged(m_label->text().teXUsed);
	ui.kfontRequesterTeX->setFont(m_label->teXFont());
	ui.sbFontSize->setValue( m_label->teXFont().pointSize() );

	//move the cursor to the end and set the focus to the text editor
	QTextCursor cursor = ui.teLabel->textCursor();
	cursor.movePosition(QTextCursor::End);
	ui.teLabel->setTextCursor(cursor);
	ui.teLabel->setFocus();

	// Geometry
	ui.cbPositionX->setCurrentIndex( (int)m_label->position().horizontalPosition );
	positionXChanged(ui.cbPositionX->currentIndex());
	ui.sbPositionX->setValue( Worksheet::convertFromSceneUnits(m_label->position().point.x(), m_worksheetUnit) );
	ui.cbPositionY->setCurrentIndex( (int)m_label->position().verticalPosition );
	positionYChanged(ui.cbPositionY->currentIndex());
	ui.sbPositionY->setValue( Worksheet::convertFromSceneUnits(m_label->position().point.y(), m_worksheetUnit) );

	if (!m_axesList.isEmpty()) {
		ui.sbOffsetX->setValue( Worksheet::convertFromSceneUnits(m_axesList.first()->titleOffsetX(), Worksheet::Unit::Point) );
		ui.sbOffsetY->setValue( Worksheet::convertFromSceneUnits(m_axesList.first()->titleOffsetY(), Worksheet::Unit::Point) );
	}
	ui.cbHorizontalAlignment->setCurrentIndex( (int) m_label->horizontalAlignment() );
	ui.cbVerticalAlignment->setCurrentIndex( (int) m_label->verticalAlignment() );
	ui.sbRotation->setValue( m_label->rotationAngle() );

	//Border
	ui.cbBorderShape->setCurrentIndex(static_cast<int>(m_label->borderShape()));
	borderShapeChanged(ui.cbBorderShape->currentIndex());
	ui.kcbBorderColor->setColor( m_label->borderPen().color() );
	ui.cbBorderStyle->setCurrentIndex( (int) m_label->borderPen().style() );
	ui.sbBorderWidth->setValue( Worksheet::convertFromSceneUnits(m_label->borderPen().widthF(), Worksheet::Unit::Point) );
	ui.sbBorderOpacity->setValue( round(m_label->borderOpacity()*100) );
	GuiTools::updatePenStyles(ui.cbBorderStyle, ui.kcbBorderColor->color());

	m_initializing = false;
}

void LabelWidget::loadConfig(KConfigGroup& group) {
	if (!m_label)
		return;

	m_initializing = true;

	//TeX
	ui.tbTexUsed->setChecked(group.readEntry("TeXUsed", (bool) m_label->text().teXUsed));
	this->teXUsedChanged(m_label->text().teXUsed);
	ui.sbFontSize->setValue( group.readEntry("TeXFontSize", m_label->teXFont().pointSize()) );
	ui.kfontRequesterTeX->setFont(group.readEntry("TeXFont", m_label->teXFont()));

	// Geometry
	ui.cbPositionX->setCurrentIndex( group.readEntry("PositionX", (int) m_label->position().horizontalPosition ) );
	ui.sbPositionX->setValue( Worksheet::convertFromSceneUnits(group.readEntry("PositionXValue", m_label->position().point.x()), m_worksheetUnit) );
	ui.cbPositionY->setCurrentIndex( group.readEntry("PositionY", (int) m_label->position().verticalPosition ) );
	ui.sbPositionY->setValue( Worksheet::convertFromSceneUnits(group.readEntry("PositionYValue", m_label->position().point.y()), m_worksheetUnit) );

	if (!m_axesList.isEmpty()) {
		ui.sbOffsetX->setValue( Worksheet::convertFromSceneUnits(group.readEntry("OffsetX", m_axesList.first()->titleOffsetX()), Worksheet::Unit::Point) );
		ui.sbOffsetY->setValue( Worksheet::convertFromSceneUnits(group.readEntry("OffsetY", m_axesList.first()->titleOffsetY()), Worksheet::Unit::Point) );
	}
	ui.cbHorizontalAlignment->setCurrentIndex( group.readEntry("HorizontalAlignment", (int) m_label->horizontalAlignment()) );
	ui.cbVerticalAlignment->setCurrentIndex( group.readEntry("VerticalAlignment", (int) m_label->verticalAlignment()) );
	ui.sbRotation->setValue( group.readEntry("Rotation", m_label->rotationAngle()) );

	//Border
	ui.cbBorderShape->setCurrentIndex(group.readEntry("BorderShape").toInt());
	ui.kcbBorderColor->setColor( group.readEntry("BorderColor", m_label->borderPen().color()) );
	ui.cbBorderStyle->setCurrentIndex( group.readEntry("BorderStyle", (int)m_label->borderPen().style()) );
	ui.sbBorderWidth->setValue( Worksheet::convertFromSceneUnits(group.readEntry("BorderWidth", m_label->borderPen().widthF()), Worksheet::Unit::Point) );
	ui.sbBorderOpacity->setValue( group.readEntry("BorderOpacity", m_label->borderOpacity())*100 );
	m_initializing = false;
}

void LabelWidget::saveConfig(KConfigGroup& group) {
	//TeX
	group.writeEntry("TeXUsed", ui.tbTexUsed->isChecked());
	group.writeEntry("TeXFontColor", ui.kcbFontColor->color());
	group.writeEntry("TeXBackgroundColor", ui.kcbBackgroundColor->color());
	group.writeEntry("TeXFont", ui.kfontRequesterTeX->font());

	// Geometry
	group.writeEntry("PositionX", ui.cbPositionX->currentIndex());
	group.writeEntry("PositionXValue", Worksheet::convertToSceneUnits(ui.sbPositionX->value(),m_worksheetUnit) );
	group.writeEntry("PositionY", ui.cbPositionY->currentIndex());
	group.writeEntry("PositionYValue",  Worksheet::convertToSceneUnits(ui.sbPositionY->value(),m_worksheetUnit) );

	if (!m_axesList.isEmpty()) {
		group.writeEntry("OffsetX",  Worksheet::convertToSceneUnits(ui.sbOffsetX->value(), Worksheet::Unit::Point) );
		group.writeEntry("OffsetY",  Worksheet::convertToSceneUnits(ui.sbOffsetY->value(), Worksheet::Unit::Point) );
	}
	group.writeEntry("HorizontalAlignment", ui.cbHorizontalAlignment->currentIndex());
	group.writeEntry("VerticalAlignment", ui.cbVerticalAlignment->currentIndex());
	group.writeEntry("Rotation", ui.sbRotation->value());

	//Border
	group.writeEntry("BorderShape", ui.cbBorderShape->currentIndex());
	group.writeEntry("BorderStyle", ui.cbBorderStyle->currentIndex());
	group.writeEntry("BorderColor", ui.kcbBorderColor->color());
	group.writeEntry("BorderWidth", Worksheet::convertToSceneUnits(ui.sbBorderWidth->value(), Worksheet::Unit::Point));
	group.writeEntry("BorderOpacity", ui.sbBorderOpacity->value()/100.0);
}
