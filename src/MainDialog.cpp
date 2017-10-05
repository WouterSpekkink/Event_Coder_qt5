/*
  Copyright 2017 Wouter Spekkink
  Authors : Wouter Spekkink <wouterspekkink@gmail.com>
  Website : http://www.wouterspekkink.org
  
  This file is part of the Event Coder Tool.
  DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
  Copyright 2017 Wouter Spekkink. All rights reserved.
  The Event Coder Tool is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  The Event Coder Tool is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with the Event Coder Tool.  If not, see <http://www.gnu.org/licenses/>.  
*/

/*
  ===NOTES===
  The main purpose of this class is to provide the user with a dialog, through which
  the user can interact with the program.
 */

#include <QtWidgets/QWidget>
#include "../include/MainDialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <sstream>

// Initializing some constants
const QString MainDialog::SEPDEFAULT = QString("-Select delimiter-");
const QString MainDialog::EMPTY = QString("");
const QString MainDialog::INCIDENT = QString("Incident");

// The constructor for this class
MainDialog::MainDialog(QWidget *parent) : QDialog(parent) {
  /* 
     DataInterface is a class that interfaces with the data that is supplied 
     by the user via an imported csv-file, or by loading a save file that the 
     user created earlier. The DataInterface also handles the creation of new files
     based on the activities of the user. The main dialog owns an instance of 
     DataInterface, and let's it do all the heavy lifting int erms of reading, 
     storing and writing data useful to the user.
  */
  dataInterface = new DataInterface();
  /*
    Logger is a class that only stores logs, and writes them to the disk as soon
    as the user closes the program.
   */
  logger = new Logger();
  fileName = "";
  sep = "";
  currentAttribute = "";
  currentAttributeFilter = "";
  currentValue = "";
  currentRelMemo = "";
  currentRelationshipFilter = "";
  currentLeftEventFilter = "";
  currentRightEventFilter = "";
  
  importFileLabel = new QLabel(tr("<h3>Import new data</h3>"));
  importFileLabel->setAlignment(Qt::AlignHCenter);
  selectFileButton = new QPushButton(tr("Select File"));
  selectSeparatorComboBox = new QComboBox();
  selectSeparatorComboBox->setEnabled(false); // Activated when a file is selected.
  initializeSeps();
  sep = SEPDEFAULT;
  importFileButton = new QPushButton(tr("Import data from file"));
  importFileButton->setEnabled(false); // Activated when a separator is selected.
  importCodesButton = new QPushButton(tr("Import codes"));
  saveLoadLabel = new QLabel(tr("<h3>Saving, loading and exit</h3>"));
  saveLoadLabel->setAlignment(Qt::AlignHCenter);
  saveSessionButton = new QPushButton(tr("Save current session"));

  loadSessionButton = new QPushButton(tr("Load previous session"));
  exitProgramButton = new QPushButton(tr("Exit program"));
  exportDataLabel = new QLabel(tr("<h3>Export options</h3>"));
  exportDataLabel->setAlignment(Qt::AlignHCenter);
  exportDataButton = new QPushButton(tr("Open export dialog"));

  previousEventButton = new QPushButton(tr("Previous incident"));
  nextEventButton = new QPushButton(tr("Next incident"));
  previousFlaggedButton = new QPushButton(tr("Previous flagged incident"));
  nextFlaggedButton = new QPushButton(tr("Next flagged incident"));
  indexIndicatorLabel = new QLabel(tr("Incident ( / )"));
  eventFlagLabel = new QLabel("");
  jumpToButton = new QPushButton(tr("Jump to"));
  flagButton = new QPushButton(tr("Toggle flag"));
  memoButton = new QPushButton(tr("Add / Edit memo"));
  leftEventField = new QPlainTextEdit();
  leftEventField->setReadOnly(true);
  rightEventField = new QPlainTextEdit();
  rightEventField->setReadOnly(true);
  previousLeftEventFieldButton = new QPushButton("<");
  nextLeftEventFieldButton = new QPushButton(">");
  selectLeftEventComboBox = new QComboBox();
  selectLeftEventComboBox->addItem("                                                  ");
  previousRightEventFieldButton = new QPushButton("<");
  nextRightEventFieldButton = new QPushButton(">");
  selectRightEventComboBox = new QComboBox();
  selectRightEventComboBox->addItem("                                                  ");

  leftEventFilterField = new QLineEdit();
  previousLeftEventFilterButton = new QPushButton("Prev.");
  nextLeftEventFilterButton = new QPushButton("Next");
  rightEventFilterField = new QLineEdit();
  previousRightEventFilterButton = new QPushButton("Prev.");
  nextRightEventFilterButton = new QPushButton("Next");
  
  eventAttributesLabel = new QLabel(tr("Incident attributes:"));
  valueLabel = new QLabel(tr("Value:"));
  valueField = new QLineEdit();
  valueField->setEnabled(false);
  valueField->installEventFilter(this);
  attributesPoolLabel = new QLabel(tr("Attribute pool:"));
  assignedAttributesListWidget = new QListWidget();
  assignedAttributesListWidget->setSortingEnabled(true);
  unassignedAttributesListWidget = new QListWidget();
  unassignedAttributesListWidget->setSortingEnabled(true);
  assignAttributeButton = new QPushButton("<<");
  unassignAttributeButton = new QPushButton(">>");
  attributesFilterLabel = new QLabel(tr("Filter:"));
  attributesFilterField = new QLineEdit();
  newAttributeButton = new QPushButton(tr("Create new attribute"));
  editAttributeButton = new QPushButton(tr("Edit highlighted attribute"));
  removeAttributesButton = new QPushButton(tr("Remove unused attributes"));

  previousAssignedAttributeButton = new QPushButton(tr("Previous"));
  previousAssignedAttributeButton->setEnabled(false);
  nextAssignedAttributeButton = new QPushButton(tr("Next"));
  nextAssignedAttributeButton->setEnabled(false);
  previousAssignedRelationshipButton = new QPushButton(tr("Previous"));
  previousAssignedRelationshipButton->setEnabled(false);
  nextAssignedRelationshipButton = new QPushButton(tr("Next"));
  nextAssignedRelationshipButton->setEnabled(false);

  eventRelationshipsLabel = new QLabel(tr("Indicated relationships:"));
  relationshipsPoolLabel = new QLabel(tr("Relationship pool:"));
  assignedRelationshipsListWidget = new QListWidget();
  assignedRelationshipsListWidget->setSortingEnabled(true);
  unassignedRelationshipsListWidget = new QListWidget();
  unassignedRelationshipsListWidget->setSortingEnabled(true);
  assignRelationshipButton = new QPushButton("<<");
  unassignRelationshipButton = new QPushButton(">>");
  relationshipsFilterLabel = new QLabel(tr("Filter"));
  relationshipsFilterField = new QLineEdit();
  newRelationshipButton = new QPushButton(tr("Create new relationship"));
  editRelationshipButton = new QPushButton(tr("Edit highlighted relationship"));
  removeRelationshipsButton = new QPushButton(tr("Remove unused relationships"));
  relMemoLabel = new QLabel(tr("Relationship memo:"));
  relMemoField = new QLineEdit();
  relMemoField->installEventFilter(this);

  // Most buttons should be deactivated at first.
  setWorkButtons(false);
  
  // Connecting the signals
  connect(selectFileButton, SIGNAL(clicked()), this, SLOT(getFile()));
  connect(importCodesButton, SIGNAL(clicked()), this, SLOT(importCodes()));
  connect(exitProgramButton, SIGNAL(clicked()), qApp, SLOT(quit()));
  connect(valueField, SIGNAL(textChanged(const QString &)), this, SLOT(setValue(const QString &)));
  connect(relMemoField, SIGNAL(textChanged(const QString &)), this, SLOT(setRelMemo(const QString &)));
  connect(newAttributeButton, SIGNAL(clicked()), this, SLOT(addAttribute()));
  connect(editAttributeButton, SIGNAL(clicked()), this, SLOT(editAttribute()));
  connect(removeAttributesButton, SIGNAL(clicked()), this, SLOT(removeAttributes()));
  connect(newRelationshipButton, SIGNAL(clicked()), this, SLOT(addRelationship()));
  connect(editRelationshipButton, SIGNAL(clicked()), this, SLOT(editRelationship()));
  connect(removeRelationshipsButton, SIGNAL(clicked()), this, SLOT(removeRelationships()));
  connect(selectSeparatorComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setSep(const QString &)));
  connect(importFileButton, SIGNAL(clicked()), this, SLOT(readNewData()));
  connect(previousEventButton, SIGNAL(clicked()), this, SLOT(previousEvent()));
  connect(nextEventButton, SIGNAL(clicked()), this, SLOT(nextEvent()));

  connect(leftEventFilterField, SIGNAL(textChanged(const QString &)), this, SLOT(setLeftEventFilter(const QString &)));
  connect(previousLeftEventFilterButton, SIGNAL(clicked()), this, SLOT(previousLeftEventFiltered()));
  connect(nextLeftEventFilterButton, SIGNAL(clicked()), this, SLOT(nextLeftEventFiltered()));
  connect(rightEventFilterField, SIGNAL(textChanged(const QString &)), this, SLOT(setRightEventFilter(const QString &)));
  connect(previousRightEventFilterButton, SIGNAL(clicked()), this, SLOT(previousRightEventFiltered()));
  connect(nextRightEventFilterButton, SIGNAL(clicked()), this, SLOT(nextRightEventFiltered()));
  
  connect(selectLeftEventComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setLeftEventField(const QString &)));
  connect(selectRightEventComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setRightEventField(const QString &)));
  connect(previousLeftEventFieldButton, SIGNAL(clicked()), this, SLOT(previousLeftEventField()));
  connect(nextLeftEventFieldButton, SIGNAL(clicked()), this, SLOT(nextLeftEventField()));
  connect(previousRightEventFieldButton, SIGNAL(clicked()), this, SLOT(previousRightEventField()));
  connect(nextRightEventFieldButton, SIGNAL(clicked()), this, SLOT(nextRightEventField()));
  connect(flagButton, SIGNAL(clicked()), this, SLOT(toggleFlag()));
  connect(previousFlaggedButton, SIGNAL(clicked()), this, SLOT(previousFlagged()));
  connect(nextFlaggedButton, SIGNAL(clicked()), this, SLOT(nextFlagged()));
  connect(jumpToButton, SIGNAL(clicked()), this, SLOT(jumpToIndex()));
  connect(saveSessionButton, SIGNAL(clicked()), this, SLOT(saveSession()));
  connect(loadSessionButton, SIGNAL(clicked()), this, SLOT(loadSession()));
  connect(exportDataButton, SIGNAL(clicked()), this, SLOT(exportData()));
  connect(dataInterface, SIGNAL(loadFinished(const QString &, const QString &)),
	  this, SLOT(processLoad(const QString &, const QString &)));
  connect(assignedAttributesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
	  this, SLOT(setCurrentAttributeAssigned(QListWidgetItem*)));
  connect(assignedAttributesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editAttribute()));
  connect(unassignedAttributesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
	  this, SLOT(setCurrentAttributeUnassigned(QListWidgetItem*)));
  connect(unassignedAttributesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editAttribute()));
  connect(assignAttributeButton, SIGNAL(clicked()), this, SLOT(assignAttribute()));
  connect(unassignAttributeButton, SIGNAL(clicked()), this, SLOT(unassignAttribute()));
  connect(previousAssignedAttributeButton, SIGNAL(clicked()), this, SLOT(jumpPreviousAttribute()));
  connect(nextAssignedAttributeButton, SIGNAL(clicked()), this, SLOT(jumpNextAttribute()));
  connect(attributesFilterField, SIGNAL(textChanged(const QString &)), this, SLOT(filterAttributes(const QString &)));
  connect(assignedRelationshipsListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(setCurrentRelationship(QListWidgetItem*)));
  connect(unassignedRelationshipsListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(setCurrentRelationship(QListWidgetItem*)));
  connect(assignedRelationshipsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editRelationship()));
  connect(unassignedRelationshipsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(editRelationship()));
  connect(assignRelationshipButton, SIGNAL(clicked()), this, SLOT(assignRelationship()));
  connect(previousAssignedRelationshipButton, SIGNAL(clicked()), this, SLOT(jumpPreviousRelationship()));
  connect(nextAssignedRelationshipButton, SIGNAL(clicked()), this, SLOT(jumpNextRelationship()));
  connect(unassignRelationshipButton, SIGNAL(clicked()), this, SLOT(unassignRelationship()));
  connect(relationshipsFilterField, SIGNAL(textChanged(const QString &)), this, SLOT(filterRelationships(const QString &)));
  connect(memoButton, SIGNAL(clicked()), this, SLOT(editMemo()));
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(finalBusiness()));
  
  // Creating the layout
  QPointer<QVBoxLayout> mainLayout = new QVBoxLayout;

  QPointer<QHBoxLayout> topMenuLayout = new QHBoxLayout;
  QPointer<QVBoxLayout> leftMenuLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> leftTopMenuLayout = new QHBoxLayout;
  leftTopMenuLayout->addWidget(selectFileButton);
  leftTopMenuLayout->addWidget(selectSeparatorComboBox);
  leftMenuLayout->addWidget(importFileLabel);
  leftMenuLayout->addLayout(leftTopMenuLayout);
  leftMenuLayout->addWidget(importFileButton);
  leftMenuLayout->addWidget(importCodesButton);
  leftMenuLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
  QPointer<QVBoxLayout> middleMenuLayout = new QVBoxLayout;
  middleMenuLayout->addWidget(saveLoadLabel);
  middleMenuLayout->addWidget(saveSessionButton);
  middleMenuLayout->addWidget(loadSessionButton);
  middleMenuLayout->addWidget(exitProgramButton);
  middleMenuLayout->setAlignment(Qt::AlignHCenter);
  QPointer<QVBoxLayout> rightMenuLayout = new QVBoxLayout;
  rightMenuLayout->addWidget(exportDataLabel);
  rightMenuLayout->addWidget(exportDataButton);
  rightMenuLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
  topMenuLayout->addLayout(leftMenuLayout);
  topMenuLayout->addLayout(middleMenuLayout);
  topMenuLayout->addLayout(rightMenuLayout);
  mainLayout->addLayout(topMenuLayout);

  QPointer<QFrame> topLine = new QFrame();
  topLine->setFrameShape(QFrame::HLine);
  mainLayout->addWidget(topLine);

  QPointer<QFrame> vertLeftLine = new QFrame();
  vertLeftLine->setFrameShape(QFrame::VLine);
  QPointer<QFrame> vertRightLine = new QFrame();
  vertRightLine->setFrameShape(QFrame::VLine);

  QPointer<QVBoxLayout> eventLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> topEventLayout = new QHBoxLayout;
  QPointer<QVBoxLayout> leftTopEventLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> topLeftTopEventLayout = new QHBoxLayout;
  topLeftTopEventLayout->addWidget(previousEventButton);
  topLeftTopEventLayout->addWidget(previousFlaggedButton);
  topLeftTopEventLayout->setAlignment(Qt::AlignHCenter);
  leftTopEventLayout->addLayout(topLeftTopEventLayout);
  QPointer<QHBoxLayout> bottomLeftTopEventLayout = new QHBoxLayout;
  bottomLeftTopEventLayout->addWidget(previousLeftEventFilterButton);
  bottomLeftTopEventLayout->addWidget(leftEventFilterField);
  bottomLeftTopEventLayout->addWidget(nextLeftEventFilterButton);
  leftTopEventLayout->addLayout(bottomLeftTopEventLayout);
  topEventLayout->addLayout(leftTopEventLayout);
  topEventLayout->addWidget(vertLeftLine);
  QPointer<QVBoxLayout> indexIndicatorLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> topIndexIndicatorLayout = new QHBoxLayout;
  topIndexIndicatorLayout->addWidget(indexIndicatorLabel);
  topIndexIndicatorLayout->addWidget(eventFlagLabel);
  indexIndicatorLayout->addLayout(topIndexIndicatorLayout);
  topIndexIndicatorLayout->setAlignment(Qt::AlignHCenter);
  indexIndicatorLabel->setAlignment(Qt::AlignHCenter);
  QPointer<QHBoxLayout> indexButtonsLayout = new QHBoxLayout;
  indexButtonsLayout->addWidget(jumpToButton);
  indexButtonsLayout->addWidget(flagButton);
  indexButtonsLayout->addWidget(memoButton);
  indexIndicatorLayout->addLayout(indexButtonsLayout);
  indexIndicatorLayout->setAlignment(Qt::AlignHCenter);
  topEventLayout->addLayout(indexIndicatorLayout);
  topEventLayout->addWidget(vertRightLine);
  QPointer<QVBoxLayout> rightTopEventLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> topRightTopEventLayout = new QHBoxLayout;
  topRightTopEventLayout->addWidget(nextFlaggedButton);
  topRightTopEventLayout->addWidget(nextEventButton);
  topRightTopEventLayout->setAlignment(Qt::AlignHCenter);
  rightTopEventLayout->addLayout(topRightTopEventLayout);
  QPointer<QHBoxLayout> bottomRightTopEventLayout = new QHBoxLayout;
  bottomRightTopEventLayout->addWidget(previousRightEventFilterButton);
  bottomRightTopEventLayout->addWidget(rightEventFilterField);
  bottomRightTopEventLayout->addWidget(nextRightEventFilterButton);
  rightTopEventLayout->addLayout(bottomRightTopEventLayout);
  topEventLayout->addLayout(rightTopEventLayout);
  eventLayout->addLayout(topEventLayout);
  QPointer<QHBoxLayout> middleEventLayout = new QHBoxLayout;
  middleEventLayout->addWidget(leftEventField);
  middleEventLayout->addWidget(rightEventField);
  eventLayout->addLayout(middleEventLayout);
  QPointer<QHBoxLayout> bottomEventLayout = new QHBoxLayout;
  QPointer<QHBoxLayout> leftBottomEventLayout = new QHBoxLayout;
  leftBottomEventLayout->addWidget(previousLeftEventFieldButton);
  leftBottomEventLayout->addWidget(selectLeftEventComboBox);
  leftBottomEventLayout->addWidget(nextLeftEventFieldButton);
  leftBottomEventLayout->setAlignment(Qt::AlignHCenter);
  bottomEventLayout->addLayout(leftBottomEventLayout);
  QPointer<QHBoxLayout> rightBottomEventLayout = new QHBoxLayout;
  rightBottomEventLayout->addWidget(previousRightEventFieldButton);
  rightBottomEventLayout->addWidget(selectRightEventComboBox);
  rightBottomEventLayout->addWidget(nextRightEventFieldButton);
  rightBottomEventLayout->setAlignment(Qt::AlignHCenter);
  bottomEventLayout->addLayout(rightBottomEventLayout);
  eventLayout->addLayout(bottomEventLayout);
  mainLayout->addLayout(eventLayout);

  QPointer<QFrame> middleLine = new QFrame();
  middleLine->setFrameShape(QFrame::HLine);
  mainLayout->addWidget(middleLine);

  QPointer<QHBoxLayout> attributesLayout = new QHBoxLayout;
  QPointer<QVBoxLayout> leftAttributesLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> leftTopAttributesLayout = new QHBoxLayout;
  leftTopAttributesLayout->addWidget(eventAttributesLabel);
  QPointer<QFrame> vertLine = new QFrame();
  vertLine->setFrameShape(QFrame::VLine);
  leftTopAttributesLayout->addWidget(vertLine);
  leftTopAttributesLayout->addWidget(valueLabel);
  leftTopAttributesLayout->addWidget(valueField);
  leftAttributesLayout->addLayout(leftTopAttributesLayout);
  leftAttributesLayout->addWidget(assignedAttributesListWidget);
  assignedAttributesListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  attributesLayout->addLayout(leftAttributesLayout);
  QPointer<QVBoxLayout> middleLeftAttributesLayout = new QVBoxLayout;
  middleLeftAttributesLayout->addWidget(assignAttributeButton);
  middleLeftAttributesLayout->addWidget(unassignAttributeButton);
  middleLeftAttributesLayout->addWidget(previousAssignedAttributeButton);
  middleLeftAttributesLayout->addWidget(nextAssignedAttributeButton);
  middleLeftAttributesLayout->setAlignment(Qt::AlignVCenter);
  attributesLayout->addLayout(middleLeftAttributesLayout);
  QPointer<QVBoxLayout> middleRightAttributesLayout = new QVBoxLayout;
  middleRightAttributesLayout->addWidget(attributesPoolLabel);
  middleRightAttributesLayout->addWidget(unassignedAttributesListWidget);
  unassignedAttributesListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  attributesLayout->addLayout(middleRightAttributesLayout);
  QPointer<QVBoxLayout> rightAttributesLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> attributesFilterLayout = new QHBoxLayout;
  attributesFilterLayout->addWidget(attributesFilterLabel);
  attributesFilterLayout->addWidget(attributesFilterField);
  rightAttributesLayout->addLayout(attributesFilterLayout);
  rightAttributesLayout->addWidget(newAttributeButton);
  rightAttributesLayout->addWidget(editAttributeButton);
  rightAttributesLayout->addWidget(removeAttributesButton);
  rightAttributesLayout->setAlignment(Qt::AlignVCenter);
  attributesLayout->addLayout(rightAttributesLayout);
  mainLayout->addLayout(attributesLayout);

  QPointer<QFrame> bottomLine = new QFrame();
  bottomLine->setFrameShape(QFrame::HLine);
  mainLayout->addWidget(bottomLine);
    
  QPointer<QHBoxLayout> relationshipsLayout = new QHBoxLayout;
  QPointer<QVBoxLayout> leftRelationshipsLayout = new QVBoxLayout;
  leftRelationshipsLayout->addWidget(eventRelationshipsLabel);
  leftRelationshipsLayout->addWidget(assignedRelationshipsListWidget);
  assignedRelationshipsListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  relationshipsLayout->addLayout(leftRelationshipsLayout);
  QPointer<QVBoxLayout> middleLeftRelationshipsLayout = new QVBoxLayout;
  middleLeftRelationshipsLayout->addWidget(assignRelationshipButton);
  middleLeftRelationshipsLayout->addWidget(unassignRelationshipButton);
  middleLeftRelationshipsLayout->addWidget(previousAssignedRelationshipButton);
  middleLeftRelationshipsLayout->addWidget(nextAssignedRelationshipButton);
  middleLeftRelationshipsLayout->setAlignment(Qt::AlignVCenter);
  relationshipsLayout->addLayout(middleLeftRelationshipsLayout);
  QPointer<QVBoxLayout> middleRightRelationshipsLayout = new QVBoxLayout;
  middleRightRelationshipsLayout->addWidget(relationshipsPoolLabel);
  middleRightRelationshipsLayout->addWidget(unassignedRelationshipsListWidget);
  unassignedRelationshipsListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  relationshipsLayout->addLayout(middleRightRelationshipsLayout);
  QPointer<QVBoxLayout> rightRelationshipsLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> relationshipsFilterLayout = new QHBoxLayout;
  relationshipsFilterLayout->addWidget(relationshipsFilterLabel);
  relationshipsFilterLayout->addWidget(relationshipsFilterField);
  rightRelationshipsLayout->addLayout(relationshipsFilterLayout);
  rightRelationshipsLayout->addWidget(newRelationshipButton);
  rightRelationshipsLayout->addWidget(editRelationshipButton);
  rightRelationshipsLayout->addWidget(removeRelationshipsButton);
  rightRelationshipsLayout->setAlignment(Qt::AlignVCenter);
  relationshipsLayout->addLayout(rightRelationshipsLayout);
  mainLayout->addLayout(relationshipsLayout);
  QPointer<QHBoxLayout> relMemoLayout = new QHBoxLayout;
  relMemoLayout->addWidget(relMemoLabel);
  relMemoLayout->addWidget(relMemoField);
  mainLayout->addLayout(relMemoLayout);

  setLayout(mainLayout);
  setWindowTitle(tr("Event Coder"));
  resize(1200, 800);
  // And that finishes the constructor.
}

void MainDialog::initializeSeps() {
  selectSeparatorComboBox->clear();
  selectSeparatorComboBox->addItem(SEPDEFAULT);
  selectSeparatorComboBox->addItem(",");
  selectSeparatorComboBox->addItem(";");
  selectSeparatorComboBox->addItem(":");
  selectSeparatorComboBox->addItem("|");
}

void MainDialog::getFile() {
  disableAttributeSelection();
  if (saveSessionButton->isEnabled() == true) { // Perhaps unclear, but this is only possible if a session is currently running.
    QPointer <QMessageBox> warningBox = new QMessageBox;
    warningBox->addButton(QMessageBox::Ok);
    warningBox->addButton(QMessageBox::Cancel);
    warningBox->setIcon(QMessageBox::Warning);
    warningBox->setText("WARNING: session in progress!");
    warningBox->setInformativeText("You will lose all progress in the current session if you load a new file without saving first.");
    if (warningBox->exec() == QMessageBox::Ok) {
      QString file = QFileDialog::getOpenFileName(this, tr("Open File"),"","Comma Delimited Files(*.csv)");
      if(!file.trimmed().isEmpty()) {
	fileName = file;
	initializeSeps();
	selectSeparatorComboBox->setEnabled(true); // We want the user to use this option only after selecting a file.
      } else {
	selectSeparatorComboBox->setEnabled(false);
      }
    }
  } else {
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"),"","Comma Delimited Files(*.csv)");
    if(!file.trimmed().isEmpty()) {
      fileName = file;
      initializeSeps();
      selectSeparatorComboBox->setEnabled(true); // We want the user to use this option only after selecting a file.
    } else {
      selectSeparatorComboBox->setEnabled(false);
    }
  }
}

/* This function sets the delimiter with which columns in the input file are delimited.
   The program cannot distinguish between columns and multi-value cells if the delimiters 
   for both are the same. Therefore, the options for the multi-value delimiters always
   exclude the column delimiter that is selected. The options for the multi-value 
   delimiter are reset each time the user sets another column delimiter.
*/

void MainDialog::setSep(const QString &selection) {
  disableAttributeSelection();  
  sep = selection;
  if (sep != SEPDEFAULT) {
    importFileButton->setEnabled(true);
  } else {
    importFileButton->setEnabled(false);
  }
}

void MainDialog::readNewData() {
  disableAttributeSelection();  
  eventIndex = 0;
  leftFieldIndex = 0;
  rightFieldIndex = 0;
  dataInterface->readFile(fileName, sep);
  QDateTime time = QDateTime::currentDateTime();
  QString timeText = time.toString(Qt::TextDate);
  QString newLog = timeText + " - " + "Imported new data from " + fileName;
  logger->addToLog(newLog);
  setWorkButtons(true);
  selectSeparatorComboBox->setEnabled(false);
  importFileButton->setEnabled(false);
  selectLeftEventComboBox->clear();
  selectRightEventComboBox->clear();
  std::vector<std::string>::iterator it;  
  for (it = dataInterface->header.begin(); it != dataInterface->header.end(); it++) {
    QString currentHeader = QString::fromUtf8(it->c_str());
    selectLeftEventComboBox->addItem(QString::fromUtf8(it->c_str()));
    selectRightEventComboBox->addItem(QString::fromUtf8(it->c_str()));
  }
  updateTexts();
  updateIndexIndicator();
}

void MainDialog::importCodes() {
  disableAttributeSelection();
  QPointer<QMessageBox> warningBox = new QMessageBox;
  warningBox->addButton(QMessageBox::Ok);
  warningBox->addButton(QMessageBox::Cancel);
  warningBox->setIcon(QMessageBox::Warning);
  warningBox->setText("WARNING: session in progress!");
  warningBox->setInformativeText("You will lose all progress in the current session if you update the source file without saving the current session first.");
  if (warningBox->exec() == QMessageBox::Ok) {
    QString QloadFile = QFileDialog::getOpenFileName(this, tr("Import Codes"),"", tr("sav files (*.sav)"));
    if (!QloadFile.trimmed().isEmpty()) {
      dataInterface->importCodes(QloadFile);
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "Updated codes from " + QloadFile;
      logger->addToLog(newLog);
    }
  }
  updateTexts();
  updateIndexIndicator();  
}

void MainDialog::saveSession() {
  QString QsaveFile = QFileDialog::getSaveFileName(this, tr("Save File"),"", tr("sav files (*.sav)"));
  if (!QsaveFile.trimmed().isEmpty()) {
    if(!QsaveFile.endsWith(".sav")) {
      QsaveFile.append(".sav");
    }
    dataInterface->writeSave(QsaveFile, eventIndex, sep);
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Saved session as " + QsaveFile + " with index (as seen by user): " +
      indexIndicatorLabel->text() + " and with index (as stored in machine): " + QString::number(eventIndex);
    logger->addToLog(newLog);
  }
}

void MainDialog::loadSession() {
  disableAttributeSelection();  
  if (saveSessionButton->isEnabled() == true) { 
    QPointer<QMessageBox> warningBox = new QMessageBox;
    warningBox->addButton(QMessageBox::Ok);
    warningBox->addButton(QMessageBox::Cancel);
    warningBox->setIcon(QMessageBox::Warning);
    warningBox->setText("WARNING: session in progress!");
    warningBox->setInformativeText("You will lose all progress in the current session if you load a session  without saving the current session first.");
    if (warningBox->exec() == QMessageBox::Ok) {
      QString QloadFile = QFileDialog::getOpenFileName(this, tr("Load File"),"", tr("sav files (*.sav)"));
      if (!QloadFile.trimmed().isEmpty()) {
	dataInterface->readSave(QloadFile);
	QDateTime time = QDateTime::currentDateTime();
	QString timeText = time.toString(Qt::TextDate);
	QString newLog = timeText + " - " + "Loaded session from " + QloadFile;
	logger->addToLog(newLog);
      }
    }
  } else {
   QString QloadFile = QFileDialog::getOpenFileName(this, tr("Load File"),"", tr("sav files (*.sav)"));
   if (!QloadFile.trimmed().isEmpty()) {
     dataInterface->readSave(QloadFile);
     QDateTime time = QDateTime::currentDateTime();
     QString timeText = time.toString(Qt::TextDate);
     QString newLog = timeText + " - " + "Loaded session from " + QloadFile;
     logger->addToLog(newLog);
   }
  }
}

void MainDialog::processLoad(const QString &index, const QString &separator) {
  int eI = index.toInt();
  eventIndex = eI;
  leftFieldIndex = 0;
  rightFieldIndex = 0;
  sep = separator;
  currentAttribute = "";
  currentAttributeFilter = "";
  attributesFilterField->blockSignals(true);
  attributesFilterField->setText("");
  attributesFilterField->blockSignals(false);
  currentRelationshipFilter = "";
  relationshipsFilterField->blockSignals(true);
  relationshipsFilterField->setText("");
  relationshipsFilterField->blockSignals(false);
  currentLeftEventFilter = "";
  leftEventFilterField->blockSignals(true);
  leftEventFilterField->setText("");
  leftEventFilterField->blockSignals(false);
  currentRightEventFilter = "";  
  rightEventFilterField->blockSignals(true);
  rightEventFilterField->setText("");
  rightEventFilterField->blockSignals(false);
  
  valueField->blockSignals(true);
  valueField->setText("");
  valueField->blockSignals(false);
  valueField->setEnabled(false);
  relMemoField->blockSignals(true);
  relMemoField->setText("");
  relMemoField->blockSignals(false);  
  
  setWorkButtons(true);
  selectSeparatorComboBox->setEnabled(false);
  importFileButton->setEnabled(false);
  selectLeftEventComboBox->clear();
  selectRightEventComboBox->clear();
  std::vector<std::string>::iterator it;  
  for (it = dataInterface->header.begin(); it != dataInterface->header.end(); it++) {
    QString currentHeader = QString::fromUtf8(it->c_str());
    selectLeftEventComboBox->addItem(QString::fromUtf8(it->c_str()));
    selectRightEventComboBox->addItem(QString::fromUtf8(it->c_str()));
  }
  updateTexts();
  updateIndexIndicator();
  QDateTime time = QDateTime::currentDateTime();
  QString timeText = time.toString(Qt::TextDate);
  QString newLog = timeText + " - " + "Data successfully loaded with index (as seen by user): " +
    indexIndicatorLabel->text() + " and with indexes (as stored in machine): " +
    QString::number(eventIndex);
  logger->addToLog(newLog);
}

void MainDialog::exportData() {
  disableAttributeSelection();  
  ExportDialog *exportDialog = new ExportDialog(this, dataInterface);
  exportDialog->exec();
  QDateTime time = QDateTime::currentDateTime();
  QString timeText = time.toString(Qt::TextDate);
  QString newLog = timeText + " - " + "Exported data to \"..\export\" folder";
  logger->addToLog(newLog);
  delete exportDialog;
}

void MainDialog::previousEvent() {
  disableAttributeSelection();
  if (eventIndex != 0) {
    eventIndex--;
  } else {
    eventIndex = dataInterface->rowData.size() - 1;
  }
  updateIndexIndicator();
  updateTexts();
  QDateTime time = QDateTime::currentDateTime();
  QString timeText = time.toString(Qt::TextDate);
  QString newLog = timeText + " - " + "New index (as seen by user): " +
    indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
    QString::number(eventIndex);
  logger->addToLog(newLog);
}

void MainDialog::nextEvent() {
  disableAttributeSelection();
  if (eventIndex != dataInterface->rowData.size() - 1) {
    eventIndex++;
  } else {
    eventIndex = 0;
  }
  updateIndexIndicator();
  updateTexts();
  QDateTime time = QDateTime::currentDateTime();
  QString timeText = time.toString(Qt::TextDate);
  QString newLog = timeText + " - " + "New index (as seen by user): " +
    indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
    QString::number(eventIndex);
  logger->addToLog(newLog);
}

void MainDialog::toggleFlag() {
  disableAttributeSelection();  
  std::vector<bool>::size_type currentRow = eventIndex;
  dataInterface->eventFlagIndex[currentRow] = !dataInterface->eventFlagIndex[currentRow];
  if (dataInterface->eventFlagIndex[currentRow] == true) {
    eventFlagLabel->setText("(!)");
    QDateTime time = QDateTime::currentDateTime(); 
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Flag set at index (as seen by user): " +
      indexIndicatorLabel->text() + ", and as stored in machine: " + QString::number(currentRow);
    logger->addToLog(newLog);
  } else if (dataInterface->eventFlagIndex[currentRow] == false) {
    eventFlagLabel->setText("");
    QDateTime time = QDateTime::currentDateTime(); 
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Flag removed at index (as seen by user): " +
      indexIndicatorLabel->text() + ", and as stored in machine: " + QString::number(currentRow);
    logger->addToLog(newLog);
  }
}

void MainDialog::previousFlagged() {
  disableAttributeSelection();
  std::vector<bool>::size_type currentRow = eventIndex;
  bool indexFound = false;
  for (std::vector<bool>::size_type i = currentRow; i != 0; i--) {
    if (dataInterface->eventFlagIndex[i] == true) {
      indexFound = true;
      eventIndex = i;
      if (i !=  currentRow) {
	break;
      }
    }
  }
  if ((!indexFound && dataInterface->eventFlagIndex[0] == true) ||
      (indexFound && eventIndex == currentRow && dataInterface->eventFlagIndex[0] == true)) {
    eventIndex = 0;
  }
  updateIndexIndicator();
  updateTexts();
  QDateTime time = QDateTime::currentDateTime();
  QString timeText = time.toString(Qt::TextDate);
  QString newLog = timeText + " - " + "New index (as seen by user): " +
    indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
    QString::number(eventIndex);
  logger->addToLog(newLog);
}

void MainDialog::nextFlagged() {
  disableAttributeSelection();
  std::vector<bool>::size_type currentRow = eventIndex;
  for (std::vector<bool>::size_type i = currentRow; i != dataInterface->eventFlagIndex.size(); i++) {
    if (dataInterface->eventFlagIndex[i] == true) {
      eventIndex = i;
      if (i != currentRow) {
	break;
      }
    }
  }
  updateIndexIndicator();
  updateTexts();
    QDateTime time = QDateTime::currentDateTime();
  QString timeText = time.toString(Qt::TextDate);
  QString newLog = timeText + " - " + "New index (as seen by user): " +
    indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
    QString::number(eventIndex);
  logger->addToLog(newLog);
}

void MainDialog::jumpToIndex() {
  disableAttributeSelection();
  IndexDialog *indexer = new IndexDialog(this, dataInterface->rowData.size());
  indexer->deleteLater();
  indexer->exec();
  std::string selectedIndex = indexer->getIndexText();
  unsigned int indexInt = 0;
  std::istringstream(selectedIndex) >> indexInt;
  delete indexer;
  if (indexInt > 0 && indexInt <= dataInterface->rowData.size()) {
    eventIndex = indexInt - 1;
  }
  updateIndexIndicator();
  updateTexts();
  QDateTime time = QDateTime::currentDateTime();
  QString timeText = time.toString(Qt::TextDate);
  QString newLog = timeText + " - " + "new index (as seen by user): " +
    indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
    QString::number(eventIndex);
  logger->addToLog(newLog);
}

void MainDialog::editMemo() {
  disableAttributeSelection();
  std::string currentMemo = "";
  bool found = false;
  std::stringstream ss;
  ss << eventIndex;
  std::string indexString = ss.str();
  std::vector <std::vector <std::string> >::size_type memoIndex = 0;
  for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->memos.size(); i++) {
    if (dataInterface->memos[i][0] == indexString) {
      currentMemo = dataInterface->memos[i][1];
      memoIndex = i;
      found = true;
      break;
    }
  }
  MemoDialog *memoDialog = new MemoDialog(this, eventIndex, currentMemo);
  memoDialog->deleteLater();
  memoDialog->exec();
  std::string memoText = memoDialog->getMemoText().toStdString();
  memoText.erase(std::remove(memoText.begin(), memoText.end(), ';'), memoText.end());
  memoText.erase(std::remove(memoText.begin(), memoText.end(), '|'), memoText.end());
  if (found) {
    if (memoText != "") {
      dataInterface->memos[memoIndex][1] = memoText;
    } else {
      dataInterface->memos.erase(dataInterface->memos.begin() + memoIndex);
    }
  } else {
    if (memoText != "") {
      std::vector<std::string> tempMemo;
      tempMemo.push_back(indexString);
      tempMemo.push_back(memoText);
      dataInterface->memos.push_back(tempMemo);
    }
  }
  delete memoDialog;
  QDateTime time = QDateTime::currentDateTime();
  QString timeText = time.toString(Qt::TextDate);
  QString newLog = timeText + " - " + "Edited memo for incident " + QString::number(eventIndex + 1);
  logger->addToLog(newLog);
}

void MainDialog::setLeftEventFilter(const QString &text) {
  currentLeftEventFilter = text;
}

void MainDialog::previousLeftEventFiltered() {
  disableAttributeSelection();
  if (currentLeftEventFilter != "") {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "attempting to jump to previous incident with string \"" +
      currentLeftEventFilter + "\" in column \"" +
      QString::fromStdString(dataInterface->header[leftFieldIndex]) + "\"";
    logger->addToLog(newLog);
    std::vector <std::vector <std::string> >::size_type index;
    bool cont = true;
    for (index = eventIndex; cont == true; index--) {
      if (index != eventIndex) {
	std::size_t found = (dataInterface->rowData[index][leftFieldIndex]).find(currentLeftEventFilter.toStdString());
	if (found != std::string::npos) {
	  eventIndex = index;
	  updateTexts();
	  updateIndexIndicator();
	  QDateTime time = QDateTime::currentDateTime();
	  QString timeText = time.toString(Qt::TextDate);
	  QString newLog = timeText + " - " + "new index (as seen by user): " +
	    indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
	    QString::number(eventIndex);
	  logger->addToLog(newLog);
	  return;
	}
      }
      if (index == 0) {
	cont = false;
      }
    }
  }
}

void MainDialog::nextLeftEventFiltered() {
  disableAttributeSelection();
  if (currentLeftEventFilter != "") {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "attempting to jump to next incident with string \"" +
      currentLeftEventFilter + "\" in column \"" +
      QString::fromStdString(dataInterface->header[leftFieldIndex]) + "\"";
    logger->addToLog(newLog);
    std::vector <std::vector <std::string> >::size_type index;
    for (index = eventIndex; index != dataInterface->rowData.size(); index++) {
      if (index != eventIndex) {
	std::size_t found = (dataInterface->rowData[index][leftFieldIndex]).find(currentLeftEventFilter.toStdString());
	if (found != std::string::npos) {
	  eventIndex = index;
	  updateTexts();
	  updateIndexIndicator();
	  QDateTime time = QDateTime::currentDateTime();
	  QString timeText = time.toString(Qt::TextDate);
	  QString newLog = timeText + " - " + "new index (as seen by user): " +
	    indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
	    QString::number(eventIndex);
	  logger->addToLog(newLog);
	  return;
	}
      }
    }
  }
}

void MainDialog::setRightEventFilter(const QString &text) {
  currentRightEventFilter = text;
}

void MainDialog::previousRightEventFiltered() {
  disableAttributeSelection();
  if (currentRightEventFilter != "") {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "attempting to jump to previous incident with string \"" +
      currentRightEventFilter + "\" in column \"" +
      QString::fromStdString(dataInterface->header[rightFieldIndex]) + "\"";
    logger->addToLog(newLog);
    std::vector <std::vector <std::string> >::size_type index;
    bool cont = true;
    for (index = eventIndex; cont == true; index--) {
      if (index != eventIndex) {
	std::size_t found = (dataInterface->rowData[index][rightFieldIndex]).find(currentRightEventFilter.toStdString());
	if (found != std::string::npos) {
	  eventIndex = index;
	  updateTexts();
	  updateIndexIndicator();
	  QDateTime time = QDateTime::currentDateTime();
	  QString timeText = time.toString(Qt::TextDate);
	  QString newLog = timeText + " - " + "new index (as seen by user): " +
	    indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
	    QString::number(eventIndex);
	  logger->addToLog(newLog);
	  return;
	}
      }
      if (index == 0) {
	cont = false;
      }
    }
  }
}

void MainDialog::nextRightEventFiltered() {
  disableAttributeSelection();
  if (currentRightEventFilter != "") {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "attempting to jump to next incident with string \"" +
      currentRightEventFilter + "\" in column \"" +
      QString::fromStdString(dataInterface->header[rightFieldIndex]) + "\"";
    logger->addToLog(newLog);
    std::vector <std::vector <std::string> >::size_type index;
    for (index = eventIndex; index != dataInterface->rowData.size(); index++) {
      if (index != eventIndex) {
	std::size_t found = (dataInterface->rowData[index][rightFieldIndex]).find(currentRightEventFilter.toStdString());
	if (found != std::string::npos) {
	  eventIndex = index;
	  updateTexts();
	  updateIndexIndicator();
	  QDateTime time = QDateTime::currentDateTime();
	  QString timeText = time.toString(Qt::TextDate);
	  QString newLog = timeText + " - " + "new index (as seen by user): " +
	    indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
	    QString::number(eventIndex);
	  logger->addToLog(newLog);
	  return;
	}
      }
    }
  }
}

void MainDialog::previousLeftEventField() {
  disableAttributeSelection();
  if (leftFieldIndex != 0) {
    leftFieldIndex--;
  } else {
    leftFieldIndex = dataInterface->header.size() - 1;
  }
  updateTexts();
}

void MainDialog::nextLeftEventField() {
  disableAttributeSelection();
  if (leftFieldIndex != dataInterface->header.size() - 1) {
    leftFieldIndex++;
  } else {
    leftFieldIndex = 0;
  }
  updateTexts();
}

void MainDialog::previousRightEventField() {
  disableAttributeSelection();
  if (rightFieldIndex != 0) {
    rightFieldIndex--;
  } else {
    rightFieldIndex = dataInterface->header.size() - 1;
  }
  updateTexts();
}

void MainDialog::nextRightEventField() {
  disableAttributeSelection();
  if (rightFieldIndex != dataInterface->header.size() - 1) {
    rightFieldIndex++;
  } else {
    rightFieldIndex = 0;
  }
  updateTexts();
}

void MainDialog::setLeftEventField(const QString &selection) {
  disableAttributeSelection();
  std::string field = selection.toStdString();
  for (std::vector <std::string>::size_type i = 0; i != dataInterface->header.size(); i++) {
    if (dataInterface->header[i] == field) {
      leftFieldIndex = i;
    }
  }
  if (saveSessionButton->isEnabled()) {
    updateTexts();
  }
}

void MainDialog::setRightEventField(const QString &selection) {
  disableAttributeSelection();
  std::string field = selection.toStdString();
  for (std::vector <std::string>::size_type i = 0; i != dataInterface->header.size(); i++) {
    if (dataInterface->header[i] == field) {
      rightFieldIndex = i;
    }
  }
  if (saveSessionButton->isEnabled()) {
    updateTexts();
  }
}

void MainDialog::assignAttribute() {
  if (currentAttribute != "") {
    std::vector <std::vector <std::string> >::iterator sIt;
    bool foundAttribute = false;
    bool foundEvent = false;
    std::vector <std::vector <std::string> >::size_type attributeIndex;
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedIncidentAttributes.size(); i++) {
      std::vector<std::string> currentGroup = dataInterface->assignedIncidentAttributes[i];
      if (currentGroup[0] == currentAttribute.toStdString()) {
	foundAttribute = true;
	attributeIndex = i;
	std::vector<std::string>::iterator pIt;
	for (pIt = currentGroup.begin() + 1; pIt != currentGroup.end(); pIt++) {
	  std::vector <std::vector <std::string> >::size_type currentIndex;
	  std::istringstream (*pIt) >> currentIndex;
	  if (currentIndex == eventIndex) {
	    foundEvent = true;
	  }
	}
      }
    }
    if (!foundAttribute) {
      std::vector <std::string> tempPair;
      std::stringstream ss;
      ss << eventIndex;
      std::string newIndex = ss.str();
      tempPair.push_back(currentAttribute.toStdString());
      tempPair.push_back(newIndex);
      dataInterface->assignedIncidentAttributes.push_back(tempPair);
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "Assigned attribute " + currentAttribute + " to incident " + QString::number(eventIndex + 1) +
 	", while inspecting " + selectLeftEventComboBox->currentText() + " and " + selectRightEventComboBox->currentText();
      logger->addToLog(newLog);
    } else if (foundAttribute && !foundEvent) {
      std::stringstream ss;
      ss << eventIndex;
      std::string newIndex = ss.str();
      dataInterface->assignedIncidentAttributes[attributeIndex].push_back(newIndex);
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "Assigned attribute " + currentAttribute + " to incident " + QString::number(eventIndex + 1) +
	", while inspecting " + selectLeftEventComboBox->currentText() + " and " + selectRightEventComboBox->currentText();
      logger->addToLog(newLog);
    }
    disableAttributeSelection();
    updateTexts();
  }
}
  
void MainDialog::unassignAttribute() {
  if (currentAttribute != "") {
    std::vector <std::vector <std::string> >::iterator sIt;
    std::vector<std::string>::size_type eraseIndex = 0;
    bool foundAttribute = false;
    bool foundEvent = false;
    std::vector <std::vector <std::string> >::size_type attributeIndex;
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedIncidentAttributes.size(); i++) {
      std::vector<std::string> currentGroup = dataInterface->assignedIncidentAttributes[i];
      if (currentGroup[0] == currentAttribute.toStdString()) {
	foundAttribute = true;
	attributeIndex = i;
	for (std::vector<std::string>::size_type j = 1; j != currentGroup.size(); j++) {
	  std::istringstream ss(currentGroup[j]);
	  std::vector <std::vector <std::string> >::size_type currentIndex;
	  ss >> currentIndex;
	  if (currentIndex == eventIndex) {
	    foundEvent = true;
	    eraseIndex = j;
	  }
	}
      }
    }
    if (foundAttribute && foundEvent) {
      dataInterface->assignedIncidentAttributes[attributeIndex].erase(dataInterface->assignedIncidentAttributes[attributeIndex].begin()
								      + eraseIndex);
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "Unassigned attribute " + currentAttribute + " from incident " +
	QString::number(eventIndex + 1);
      logger->addToLog(newLog);
      std::vector <std::vector <std::string> >::iterator it;
      for (it = dataInterface->assignedIncidentAttributes.begin(); it != dataInterface->assignedIncidentAttributes.end();) {
	std::vector<std::string> currentAssigned = *it;
	if (currentAssigned.size() < 2) {
	  dataInterface->assignedIncidentAttributes.erase(it);
	} else {
	  it++;
	}
      }
      for (it = dataInterface->incidentValues.begin(); it != dataInterface->incidentValues.end();) {
	std::vector<std::string> currentValue = *it;
	std::stringstream ss;
	ss << eventIndex;
	if (currentValue[0] == currentAttribute.toStdString() && currentValue[1] == ss.str()) {
	  dataInterface->incidentValues.erase(it);
	} else {
	  it++;
	}
      }
    }
    disableAttributeSelection();
    updateTexts();
  }
}

void MainDialog::jumpPreviousAttribute() {
  if (currentAttribute != "") {
    std::vector <std::vector <std::string> >::iterator it;
    std::vector<std::string> tempAttribute;
    bool found = false;
    for (it = dataInterface->assignedIncidentAttributes.begin(); it != dataInterface->assignedIncidentAttributes.end(); it++) {
      if (*(it->begin()) == currentAttribute.toStdString()) {
	found = true;
	tempAttribute = *it;
	break;
      }
    }
    std::vector<std::string>::iterator it2;
    std::vector <std::vector <std::string> >::size_type index = eventIndex;
    bool changed = false;
    if (found) {
      for (it2 = tempAttribute.begin() + 1; it2 != tempAttribute.end(); it2++) {
	std::vector <std::vector <std::string> >::size_type tempIndex;
	std::istringstream ss(*it2);
	ss >> tempIndex;
	if (tempIndex < eventIndex && !changed) {
	  index = tempIndex;
	  changed = true;
	} else if (tempIndex < eventIndex && changed) {
	  if (tempIndex > index && tempIndex < eventIndex) {
	    index = tempIndex;
	  }
	}
      }
      if (index < eventIndex) {
	eventIndex = index;
      }
      disableAttributeSelection();
      updateIndexIndicator();
      updateTexts();
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "new index (as seen by user): " +
	indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
	QString::number(eventIndex);
      logger->addToLog(newLog);
    }
  }
}

void MainDialog::jumpNextAttribute() {
  if (currentAttribute != "") {
    std::vector <std::vector <std::string> >::iterator it;
    std::vector<std::string> tempAttribute;
    bool found = false;
    for (it = dataInterface->assignedIncidentAttributes.begin(); it != dataInterface->assignedIncidentAttributes.end(); it++) {
      if (*(it->begin()) == currentAttribute.toStdString()) {
	found = true;
	tempAttribute = *it;
	break;
      }
    }
    std::vector<std::string>::iterator it2;
    std::vector <std::vector <std::string> >::size_type index = eventIndex;
    bool changed = false;
    if (found) {
      for (it2 = tempAttribute.begin() + 1; it2 != tempAttribute.end(); it2++) {
	std::vector <std::vector <std::string> >::size_type tempIndex;
	std::istringstream ss(*it2);
	ss >> tempIndex;
	if (tempIndex > eventIndex && !changed) {
	  index = tempIndex;
	  changed = true;
	} else if (tempIndex > eventIndex && changed) {
	  if (tempIndex < index && tempIndex > eventIndex) {
	    index = tempIndex;
	  }
	}
      }
      if (index > eventIndex) {
	eventIndex = index;
      }
      disableAttributeSelection();
      updateIndexIndicator();
      updateTexts();
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "new index (as seen by user): " +
	indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
	QString::number(eventIndex);
      logger->addToLog(newLog);
    }
  }
}

void MainDialog::setValue(const QString &newValue) {
  if (currentAttribute != "") {
    std::string temp = newValue.toStdString();
    temp.erase(std::remove(temp.begin(), temp.end(), ';'), temp.end());
    temp.erase(std::remove(temp.begin(), temp.end(), '|'), temp.end());
    currentValue = QString::fromStdString(temp);
    std::stringstream ss;
    ss << eventIndex;
    std::string tempString = ss.str();
    std::vector <std::vector <std::string> >::iterator it;
    if (currentValue != "") {
      std::vector<std::string> tempValue;
      tempValue.push_back(currentAttribute.toStdString());
      tempValue.push_back(tempString);
      tempValue.push_back(currentValue.toStdString());
      bool found = false;
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->incidentValues.size(); i++) {
	std::vector<std::string> currentValue = dataInterface->incidentValues[i];
	if (currentValue[0] == tempValue[0] && currentValue[1] == tempValue[1]) {
	  dataInterface->incidentValues[i][2] = tempValue[2];
	  found = true;
	}
      }
      if (!found) {
	dataInterface->incidentValues.push_back(tempValue);
      }
    } else {
      for (it = dataInterface->incidentValues.begin(); it != dataInterface->incidentValues.end();) {
	std::vector<std::string> currentValue = *it;
	if (currentValue[0] == currentAttribute.toStdString() && currentValue[1] == tempString) {
	  dataInterface->incidentValues.erase(it);
	} else {
	  it++;
	} 
      }
    }
  }
}

void MainDialog::setRelMemo(const QString &newRelMemo) {
  if (currentRelationship != "") {
    std::string temp = newRelMemo.toStdString();
    temp.erase(std::remove(temp.begin(), temp.end(), ';'), temp.end());
    temp.erase(std::remove(temp.begin(), temp.end(), '|'), temp.end());
    temp.erase(temp.find_last_not_of(" \n\r\t")+1);
    std::vector <std::vector <std::string> >::iterator it;
    if (currentRelMemo != "") {
      std::vector<std::string> tempRelMemo;      
      tempRelMemo.push_back(currentRelationship.toStdString());
      tempRelMemo.push_back(currentRelMemo.toStdString());
      bool found = false;
      for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->relationMemos.size(); i++) {
	std::vector<std::string> currentMemo = dataInterface->relationMemos[i];
	if (currentMemo[0] == tempRelMemo[0]) {
	  dataInterface->relationMemos[i][1] = tempRelMemo[1];
	  found = true;
	}
      }
      if (!found) {
	dataInterface->relationMemos.push_back(tempRelMemo);
      }
    } else {
      for (it = dataInterface->relationMemos.begin(); it != dataInterface->relationMemos.end();) {
	std::vector<std::string> currentMemo = *it;
	if (currentMemo[0] == currentRelationship.toStdString()) {
	  dataInterface->relationMemos.erase(it);
	} else {
	  it++;
	} 
      }
    }
  }
}

void MainDialog::filterAttributes(const QString &text) {
  disableAttributeSelection();
  currentAttributeFilter = text;
  updateTexts();
}

void MainDialog::addAttribute() {
  disableAttributeSelection();
  AttributesDialog *attributesDialog = new AttributesDialog(this, dataInterface, EMPTY, INCIDENT, logger);
  attributesDialog->exec();
  delete attributesDialog;
  updateTexts();
}

void MainDialog::editAttribute() {
  valueField->blockSignals(true);
  valueField->setText("");
  valueField->blockSignals(false);
  valueField->setEnabled(false);
  currentValue = "";
  if (currentAttribute != EMPTY) {
    AttributesDialog *attributesDialog = new AttributesDialog(this, dataInterface, currentAttribute, INCIDENT, logger);
    disableAttributeSelection();
    attributesDialog->exec();
    delete attributesDialog;
    updateTexts();
  }
}

void MainDialog::removeAttributes() {
  disableAttributeSelection();
  std::vector <std::vector <std::string> >::iterator it;
  std::vector <std::vector <std::string> >::iterator it2;
  QString removed = QString();
  for (it = dataInterface->incidentAttributes.begin(); it != dataInterface->incidentAttributes.end();) {
    std::vector<std::string> currentAttribute = *it;
    bool found = false;
    for (it2 = dataInterface->assignedIncidentAttributes.begin(); it2 != dataInterface->assignedIncidentAttributes.end(); it2++) {
      std::vector<std::string> currentAssigned = *it2;
      if (currentAssigned[0] == currentAttribute[0]) {
	found = true;
      }
    }
    if (!found) {
      if (removed.size() == 0) {
	removed.append(QString::fromStdString(*it->begin()));
      } else {
	removed.append(", ");
	removed.append(QString::fromStdString(*it->begin()));
      }
      std::vector <std::vector <std::string> >::iterator it3;
      for (it3 = dataInterface->assignedIncidentAttributeCategories.begin();
	   it3 != dataInterface->assignedIncidentAttributeCategories.end();) {
	std::vector<std::string>::iterator it4;
	for (it4 = it3->begin() + 1; it4 != it3->end();) {
	  if (*it4 == currentAttribute[0]) {
	    it3->erase(it4);
	  } else {
	    it4++;
	  }
	}
	if (it3->size() < 2) {
	  dataInterface->assignedIncidentAttributeCategories.erase(it3);
	} else {
	  it3++;
	}
      }
      dataInterface->incidentAttributes.erase(it);
    } else {
      it++;
    }
  }
  if (removed.size() > 0) {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Removed unused attributes " + removed;
    logger->addToLog(newLog);
  }
  updateTexts();
}

void MainDialog::assignRelationship() {
  if (currentRelationship != "") {
    std::vector <std::vector <std::string> >::iterator sIt;
    bool foundRelationship = false;
    bool foundEvent = false;
    std::vector <std::vector <std::string> >::size_type relationshipIndex;
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedRelationships.size(); i++) {
      std::vector<std::string> currentGroup = dataInterface->assignedRelationships[i];
      if (currentGroup[0] == currentRelationship.toStdString()) {
	foundRelationship = true;
	relationshipIndex = i;
	std::vector<std::string>::iterator pIt;
	for (pIt = currentGroup.begin() + 1; pIt != currentGroup.end(); pIt++) {
	  std::vector <std::vector <std::string> >::size_type currentIndex;
	  std::istringstream (*pIt) >> currentIndex;
	  if (currentIndex == eventIndex) {
	    foundEvent = true;
	  }
	}
      }
    }
    if (!foundRelationship) {
      std::vector <std::string> tempPair;
      std::stringstream ss;
      ss << eventIndex;
      std::string newIndex = ss.str();
      tempPair.push_back(currentRelationship.toStdString());
      tempPair.push_back(newIndex);
      dataInterface->assignedRelationships.push_back(tempPair);
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "Assigned relationship " + currentRelationship + " to incident " +
	QString::number(eventIndex + 1) + ", while inspecting " + selectLeftEventComboBox->currentText() + " and " +
	selectRightEventComboBox->currentText();;
      logger->addToLog(newLog);
    } else if (foundRelationship && !foundEvent) {
      std::stringstream ss;
      ss << eventIndex;
      std::string newIndex = ss.str();
      dataInterface->assignedRelationships[relationshipIndex].push_back(newIndex);
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "Assigned relationship " + currentRelationship + " to incident " +
	QString::number(eventIndex + 1) + ", while inspecting " + selectLeftEventComboBox->currentText() + " and " +
	selectRightEventComboBox->currentText();;
      logger->addToLog(newLog);
    }
    disableAttributeSelection();
    updateTexts();
  }
}

void MainDialog::unassignRelationship() {
  if (currentRelationship != "") {
    std::vector <std::vector <std::string> >::iterator sIt;
    std::vector<std::string>::size_type eraseIndex = 0;
    bool foundRelationship = false;
    bool foundEvent = false;
    std::vector <std::vector <std::string> >::size_type relationshipIndex;
    for (std::vector <std::vector <std::string> >::size_type i = 0; i != dataInterface->assignedRelationships.size(); i++) {
      std::vector<std::string> currentGroup = dataInterface->assignedRelationships[i];
      if (currentGroup[0] == currentRelationship.toStdString()) {
	foundRelationship = true;
	relationshipIndex = i;
	for (std::vector<std::string>::size_type j = 1; j != currentGroup.size(); j++) {
	  std::istringstream ss(currentGroup[j]);
	  std::vector <std::vector <std::string> >::size_type currentIndex;
	  ss >> currentIndex;
	  if (currentIndex == eventIndex) {
	    foundEvent = true;
	    eraseIndex = j;
	  }
	}
      }
    }
    if (foundRelationship && foundEvent) {
      dataInterface->assignedRelationships[relationshipIndex].erase(dataInterface->assignedRelationships[relationshipIndex].begin() +
								    eraseIndex);
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "Unassigned relationship " + currentRelationship + " from incident " +
	QString::number(eventIndex + 1);
      logger->addToLog(newLog);
      std::vector <std::vector <std::string> >::iterator it;
      for (it = dataInterface->assignedRelationships.begin(); it != dataInterface->assignedRelationships.end();) {
	std::vector<std::string> currentAssigned = *it;
	if (currentAssigned.size() < 2) {
	  dataInterface->assignedRelationships.erase(it);
	} else {
	  it++;
	}
      }
    }
    disableAttributeSelection();
    updateTexts();
  }
}


void MainDialog::jumpPreviousRelationship() {
  if (currentRelationship != "") {
    std::vector <std::vector <std::string> >::iterator it;
    std::vector<std::string> tempRelationship;
    bool found = false;
    for (it = dataInterface->assignedRelationships.begin(); it != dataInterface->assignedRelationships.end(); it++) {
      if (*(it->begin()) == currentRelationship.toStdString()) {
	found = true;
	tempRelationship = *it;
	break;
      }
    }
    std::vector<std::string>::iterator it2;
    std::vector <std::vector <std::string> >::size_type index = eventIndex;
    bool changed = false;
    if (found) {
      for (it2 = tempRelationship.begin() + 1; it2 != tempRelationship.end(); it2++) {
	std::vector <std::vector <std::string> >::size_type tempIndex;
	std::istringstream ss(*it2);
	ss >> tempIndex;
	if (tempIndex < eventIndex && !changed) {
	  index = tempIndex;
	  changed = true;
	} else if (tempIndex < eventIndex && changed) {
	  if (tempIndex > index && tempIndex < eventIndex) {
	    index = tempIndex;
	  }
	}
      }
      if (index < eventIndex) {
	eventIndex = index;
      }
      disableAttributeSelection();
      updateIndexIndicator();
      updateTexts();
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "new index (as seen by user): " +
	indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
	QString::number(eventIndex);
      logger->addToLog(newLog);
    }
  }
}

void MainDialog::jumpNextRelationship() {
  if (currentRelationship != "") {
    std::vector <std::vector <std::string> >::iterator it;
    std::vector<std::string> tempRelationship;
    bool found = false;
    for (it = dataInterface->assignedRelationships.begin(); it != dataInterface->assignedRelationships.end(); it++) {
      if (*(it->begin()) == currentRelationship.toStdString()) {
	found = true;
	tempRelationship = *it;
	break;
      }
    }
    std::vector<std::string>::iterator it2;
    std::vector <std::vector <std::string> >::size_type index = eventIndex;
    bool changed = false;
    if (found) {
      for (it2 = tempRelationship.begin() + 1; it2 != tempRelationship.end(); it2++) {
	std::vector <std::vector <std::string> >::size_type tempIndex;
	std::istringstream ss(*it2);
	ss >> tempIndex;
	if (tempIndex > eventIndex && !changed) {
	  index = tempIndex;
	  changed = true;
	} else if (tempIndex > eventIndex && changed) {
	  if (tempIndex < index && tempIndex > eventIndex) {
	    index = tempIndex;
	  }
	}
      }
      if (index > eventIndex) {
	eventIndex = index;
      }
      disableAttributeSelection();
      updateIndexIndicator();
      updateTexts();
      QDateTime time = QDateTime::currentDateTime();
      QString timeText = time.toString(Qt::TextDate);
      QString newLog = timeText + " - " + "new index (as seen by user): " +
	indexIndicatorLabel->text() + ", and new index (as stored in machine): " +
	QString::number(eventIndex);
      logger->addToLog(newLog);
    }
  }
}


void MainDialog::addRelationship() {
  disableAttributeSelection();
  RelationsDialog *relationsDialog = new RelationsDialog(this, dataInterface, EMPTY, logger);
  relationsDialog->exec();
  delete relationsDialog;
  updateTexts();
}

void MainDialog::editRelationship() {
  if (currentRelationship != EMPTY) {
    RelationsDialog * relationsDialog = new RelationsDialog(this, dataInterface, currentRelationship, logger);
    disableAttributeSelection();
    relationsDialog->exec();
    delete relationsDialog;
    updateTexts();
  }
}

void MainDialog::removeRelationships() {
  disableAttributeSelection();
  std::vector <std::vector <std::string> >::iterator it;
  std::vector <std::vector <std::string> >::iterator it2;
  QString removed = QString();
  for (it = dataInterface->relationships.begin(); it != dataInterface->relationships.end();) {
    std::vector<std::string> currentRelationship = *it;
    bool found = false;
    for (it2 = dataInterface->assignedRelationships.begin(); it2 != dataInterface->assignedRelationships.end(); it2++) {
      std::vector<std::string> currentAssigned = *it2;
      if (currentAssigned[0] == currentRelationship[0]) {
	found = true;
      }
    }
    if (!found) {
      if (removed.size() == 0) {
	removed.append(QString::fromStdString(*it->begin()));
      } else {
	removed.append(", ");
	removed.append(QString::fromStdString(*it->begin()));
      }
      dataInterface->relationships.erase(it);
    } else {
      it++;
    }
  }
  if (removed.size() > 0) {
    QDateTime time = QDateTime::currentDateTime();
    QString timeText = time.toString(Qt::TextDate);
    QString newLog = timeText + " - " + "Removed unused relationships " + removed;
    logger->addToLog(newLog);
  }
  updateTexts();
}


void MainDialog::filterRelationships(const QString &text) {
  disableAttributeSelection();
  currentRelationshipFilter = text;
  updateTexts();
}

void MainDialog::setCurrentAttributeUnassigned(QListWidgetItem *item) {
  disableAttributeSelection();
  currentAttribute = item->text();
  valueField->blockSignals(true);
  valueField->setText("");
  valueField->blockSignals(false);
  valueField->setEnabled(false);
  previousAssignedAttributeButton->setEnabled(true);
  nextAssignedAttributeButton->setEnabled(true);
}

void MainDialog::setCurrentAttributeAssigned(QListWidgetItem *item) {
  disableAttributeSelection();
  currentAttribute = item->text();
  std::vector <std::vector <std::string> >::iterator it;
  valueField->setEnabled(true);
  currentValue = "";
  valueField->blockSignals(true);
  valueField->setText(currentValue);
  valueField->blockSignals(false);
  previousAssignedAttributeButton->setEnabled(true);
  nextAssignedAttributeButton->setEnabled(true);
  for (it = dataInterface->incidentValues.begin(); it != dataInterface->incidentValues.end(); it++) {
    std::vector<std::string> currentGroup = *it;
    std::stringstream ss;
    ss << eventIndex;
    if (currentGroup[0] == currentAttribute.toStdString() && currentGroup[1] == ss.str()) {
      currentValue = QString::fromStdString(currentGroup[2]);
      valueField->blockSignals(true);
      valueField->setText(currentValue);
      valueField->blockSignals(false);
      return;
    }
  }
}

void MainDialog::setCurrentRelationship(QListWidgetItem *item) {
  disableAttributeSelection();
  currentRelationship = item->text();
  currentRelMemo = "";
  relMemoField->blockSignals(true);
  relMemoField->setText(currentRelMemo);
  relMemoField->blockSignals(false);
  std::vector <std::vector <std::string> >::iterator it;
  previousAssignedRelationshipButton->setEnabled(true);
  nextAssignedRelationshipButton->setEnabled(true);
  for (it = dataInterface->relationMemos.begin(); it != dataInterface->relationMemos.end(); it++) {
    std::vector<std::string> currentGroup = *it;
    if (currentGroup[0] == currentRelationship.toStdString()) {
      currentRelMemo = QString::fromStdString(currentGroup[1]);
      relMemoField->blockSignals(true);
      relMemoField->setText(currentRelMemo);
      relMemoField->blockSignals(false);
      return;
    }
  }
}

void MainDialog::updateTexts() {
  leftEventField->setPlainText(QString::fromUtf8((dataInterface->rowData[eventIndex][leftFieldIndex]).c_str()));
  rightEventField->setPlainText(QString::fromUtf8((dataInterface->rowData[eventIndex][rightFieldIndex]).c_str()));
  selectLeftEventComboBox->setCurrentIndex(selectLeftEventComboBox->
					   findText(QString::fromUtf8((dataInterface->header[leftFieldIndex]).c_str())));
  selectRightEventComboBox->setCurrentIndex(selectRightEventComboBox->
					    findText(QString::fromUtf8((dataInterface->header[rightFieldIndex]).c_str())));
  std::vector<bool>::size_type currentRow = eventIndex;
  if (dataInterface->eventFlagIndex[currentRow] == true) {
    eventFlagLabel->setText("(!)");
  } else {
    eventFlagLabel->setText("");
  }
  assignedAttributesListWidget->clear();
  unassignedAttributesListWidget->clear();
  std::vector <std::vector <std::string> >::iterator sIt;
  std::vector <std::vector <std::string> >::iterator sIt2;
  for (sIt = dataInterface->incidentAttributes.begin(); sIt != dataInterface->incidentAttributes.end(); sIt++) {
    bool found = false;
    std::vector<std::string> currentAttr= *sIt;
    for (sIt2 = dataInterface->assignedIncidentAttributes.begin(); sIt2 != dataInterface->assignedIncidentAttributes.end(); sIt2++) {
      std::vector<std::string> currentAssigned = *sIt2;
      if (currentAssigned[0] == currentAttr[0] && !found) {
	for (std::vector<std::string>::size_type i = 1; i != currentAssigned.size(); i++) {
	  std::string currentIndex = currentAssigned[i];
	  std::vector <std::vector <std::string> >::size_type tempIndex;
	  std::stringstream (currentIndex) >> tempIndex;
	  if (tempIndex == eventIndex) {
	    found = true;
	  }
	}
      }
    }
    if (!found) {
      if (currentAttributeFilter != "") {
	std::size_t found = currentAttr[0].find(currentAttributeFilter.toStdString());
	if (found != std::string::npos) {
	  unassignedAttributesListWidget->addItem(QString::fromStdString(currentAttr[0]));
	}
      } else {
	unassignedAttributesListWidget->addItem(QString::fromStdString(currentAttr[0]));
      }
    } else {
      if (currentAttributeFilter != "") {
	std::size_t found = currentAttr[0].find(currentAttributeFilter.toStdString());
	if (found != std::string::npos) {
	  assignedAttributesListWidget->addItem(QString::fromStdString(currentAttr[0]));	
      	}
      } else {
	assignedAttributesListWidget->addItem(QString::fromStdString(currentAttr[0]));
      }
    }
  }
  for (int i = 0; i != assignedAttributesListWidget->count(); i++) {
    std::string currentItem = assignedAttributesListWidget->item(i)->text().toStdString();
    std::vector <std::vector <std::string> >::iterator it;
    for (it = dataInterface->incidentAttributes.begin(); it != dataInterface->incidentAttributes.end(); it++) {
      std::vector<std::string> currentAttrib = *it;
      if (currentAttrib[0] == currentItem) {
	assignedAttributesListWidget->item(i)->setToolTip(QString::fromStdString(currentAttrib[1]));
      }
    }
  }
  for (int i = 0; i != unassignedAttributesListWidget->count(); i++) {
    std::string currentItem = unassignedAttributesListWidget->item(i)->text().toStdString();
    std::vector <std::vector <std::string> >::iterator it;
    for (it = dataInterface->incidentAttributes.begin(); it != dataInterface->incidentAttributes.end(); it++) {
      std::vector<std::string> currentAttrib = *it;
      if (currentAttrib[0] == currentItem) {
	unassignedAttributesListWidget->item(i)->setToolTip(QString::fromStdString(currentAttrib[1]));
      }
    }
  }
  assignedRelationshipsListWidget->clear();
  unassignedRelationshipsListWidget->clear();
  std::vector <std::vector <std::string> >::iterator rIt;
  std::vector <std::vector <std::string> >::iterator rIt2;
  for (rIt = dataInterface->relationships.begin(); rIt != dataInterface->relationships.end(); rIt++) {
    bool found = false;
    std::vector<std::string> currentRel= *rIt;
    for (rIt2 = dataInterface->assignedRelationships.begin(); rIt2 != dataInterface->assignedRelationships.end(); rIt2++) {
      std::vector<std::string> currentAssigned = *rIt2;
      if (currentAssigned[0] == currentRel[0] && !found) {
	for (std::vector<std::string>::size_type i = 1; i != currentAssigned.size(); i++) {
	  std::string currentIndex = currentAssigned[i];
	  std::vector <std::vector <std::string> >::size_type tempIndex;
	  std::stringstream (currentIndex) >> tempIndex;
	  if (tempIndex == eventIndex) {
	    found = true;
	  }
	}
      }
    }
    if (!found) {
      if (currentRelationshipFilter != "") {
	std::size_t found = currentRel[0].find(currentRelationshipFilter.toStdString());
	if (found != std::string::npos) {
	  unassignedRelationshipsListWidget->addItem(QString::fromStdString(currentRel[0]));
	}
      } else {
	unassignedRelationshipsListWidget->addItem(QString::fromStdString(currentRel[0]));
      }
    } else {
      if (currentRelationshipFilter != "") {
	std::size_t found = currentRel[0].find(currentRelationshipFilter.toStdString());
	if (found != std::string::npos) {
	  assignedRelationshipsListWidget->addItem(QString::fromStdString(currentRel[0]));	
      	}
      } else {
	assignedRelationshipsListWidget->addItem(QString::fromStdString(currentRel[0]));
      }
    }
  }
}

void MainDialog::updateIndexIndicator() {
  indexIndicatorLabel->setText("Incident (" + QString::number(eventIndex + 1)  + " / " +
			       QString::number(dataInterface->rowData.size()) + ")");
}

void MainDialog::disableAttributeSelection() {
  currentAttribute = "";
  currentValue = "";
  currentRelationship = "";
  valueField->setEnabled(false);
  valueField->blockSignals(true);
  valueField->setText(currentValue);
  valueField->blockSignals(false);
  previousAssignedAttributeButton->setEnabled(false);
  nextAssignedAttributeButton->setEnabled(false);
  previousAssignedRelationshipButton->setEnabled(false);
  nextAssignedRelationshipButton->setEnabled(false);
}

void MainDialog::setWorkButtons(const bool status) {
  importCodesButton->setEnabled(status);
  saveSessionButton->setEnabled(status);
  previousEventButton->setEnabled(status);
  nextEventButton->setEnabled(status);
  previousFlaggedButton->setEnabled(status);
  nextFlaggedButton->setEnabled(status);
  jumpToButton->setEnabled(status);
  flagButton->setEnabled(status);
  memoButton->setEnabled(status);
  previousLeftEventFieldButton->setEnabled(status);
  nextLeftEventFieldButton->setEnabled(status);
  previousRightEventFieldButton->setEnabled(status);
  nextRightEventFieldButton->setEnabled(status);
  assignAttributeButton->setEnabled(status);
  unassignAttributeButton->setEnabled(status);
  newAttributeButton->setEnabled(status);
  editAttributeButton->setEnabled(status);
  removeAttributesButton->setEnabled(status);
  assignRelationshipButton->setEnabled(status);
  unassignRelationshipButton->setEnabled(status);
  newRelationshipButton->setEnabled(status);
  editRelationshipButton->setEnabled(status);
  removeRelationshipsButton->setEnabled(status);
  selectLeftEventComboBox->setEnabled(status);
  selectRightEventComboBox->setEnabled(status);
  attributesFilterField->setEnabled(status);
  relationshipsFilterField->setEnabled(status);
  exportDataButton->setEnabled(status);
  previousLeftEventFilterButton->setEnabled(status);
  nextLeftEventFilterButton->setEnabled(status);
  previousRightEventFilterButton->setEnabled(status);
  nextRightEventFilterButton->setEnabled(status);
  relMemoField->setEnabled(status);
  leftEventFilterField->setEnabled(status);
  rightEventFilterField->setEnabled(status);
  valueField->setEnabled(status);
}

void MainDialog::finalBusiness() {
  QDateTime time = QDateTime::currentDateTime();
  QString timeText = time.toString(Qt::TextDate);
  QString logName = "log_" + timeText + ".txt";
  QString path = QCoreApplication::applicationDirPath();
  path.append("/logs/");
  if (!QDir(path).exists()) {
    QDir().mkdir(path);
  }
  path.append(logName);
  logger->writeLog(path);
  delete logger;
  delete dataInterface; 
}

bool MainDialog::eventFilter(QObject *target, QEvent *event) {
  if (target == valueField || target == relMemoField) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if (keyEvent->key() == Qt::Key_Semicolon || keyEvent->key() == Qt::Key_Bar) {
	return true;
      }
    }
  }
  if (target == valueField) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if (keyEvent->key() == Qt::Key_Comma) {
	return true;
      }
    }
  }
  return QObject::eventFilter(target, event);
}
